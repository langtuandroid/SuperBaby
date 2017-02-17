//
//  Round002_04.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/25.
//
//

#include "Round002_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "AppConfig.h"
#include "SocialShareNode.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define BASKET_PNG_FILE     "SL-GK02-4-TM02.png"     // 吊篮图片
#define DASHED_EGG_PNG_FILE "SL-GK02-4-TM01.png"     // 虚线蛋

#define BASKET_TAG          101

//
#define BIRD_GAF_FILE       "res/stage002/SL-GK02-4-GS01/SL-GK02-4-GS01.gaf"
#define BN_PLIST_FILE       "res/stage002/SL-GK02-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE         "res/stage002/SL-GK02-IT.png"               // BN的 png 文件
#define BN_ZORDER           2                                           // BN的z值
#define EGG_ZORDER          3
#define MOV_TIME            0.2f                                        // 答错回原位置移动时间

/*
 *  constructor
 */
Round002_04::Round002_04()
:m_pTargetSp(nullptr),
m_pSSNode(nullptr),
m_correctNumber(2)
{}

/*
 *  destructor
 */
Round002_04::~Round002_04()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round002_04* Round002_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round002_04();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene,pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

/*
 *  初始化
 */
bool Round002_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf(BIRD_GAF_FILE);
    preLoadRes();
    return true;
}

//
void Round002_04::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    // 篮子
    auto basketSp = Sprite::createWithSpriteFrameName(BASKET_PNG_FILE);
    basketSp->setPosition(Vec2(1600.0f, 840.0f));
    m_pBN->addChild(basketSp, 1, BASKET_TAG);
    
    char szBuffer[32];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK02-4-DA%02ld.png", i + 1);
        m_eggSps[i] =Sprite::createWithSpriteFrameName(szBuffer);
        m_eggSps[i]->setUserData((void*)i);
        m_pBN->addChild(m_eggSps[i], EGG_ZORDER);
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[4] = {false, false, false, false};
    unsigned int placed = 0;
    float posx = 800.0f, posy = 320.0f, offsetx = 200.0f;
    while (placed < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_eggSps[randIndex]->setPosition(Vec2(posx, posy));
        m_originalPoints[randIndex] = Vec2(posx, posy);
        posx += offsetx;
        placed++;
    } /*while*/
    
    // 虚线蛋
    auto dashedEggSp = Sprite::createWithSpriteFrameName(DASHED_EGG_PNG_FILE);
    dashedEggSp->setPosition(Vec2(1596.0f, 400.0f));
    m_pBN->addChild(dashedEggSp, 2);
    m_pTargetSp = dashedEggSp;
    
    auto birdGaf = Actor::createWithGafFile(BIRD_GAF_FILE);
    birdGaf->setPosition(Vec2(866.0f, 1500.0f));
    this->addChild(birdGaf, 1);
}

/* 开启触摸 */
void Round002_04::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        
        if (rect.containsPoint(touchPoint))
        {
            m_touchBeganPoint = touch->getLocation();
            m_pBN->reorderChild(target, EGG_ZORDER + 1);
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        unsigned long selIndex = (unsigned long)target->getUserData();
        auto touchPoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoints[selIndex].x + (touchPoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoints[selIndex].y + (touchPoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = m_pTargetSp->convertTouchToNodeSpace(touch);
        unsigned long selIndex = (unsigned long)target->getUserData();
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pTargetSp->getContentSize().width, m_pTargetSp->getContentSize().height);
        target->setScale(1.0f);
        m_pBN->reorderChild(target, EGG_ZORDER);
        // 判断是否 move 到目标答案矩形
        if (rect.containsPoint(touchPoint))
        {
            if (m_correctNumber == selIndex) /*第三只蛋*/
            {
                isCorrect = true;
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    // 答对时主角说话
                    playLeadAnsCorEff();
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                celebrate(5.0f);
                target->setPosition(m_pTargetSp->getPosition());
                m_pTargetSp->setVisible(false);
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    dispatcher->removeEventListenersForTarget(m_eggSps[i]);
                    selIndex = (unsigned long)m_eggSps[i]->getUserData();
                    if (m_correctNumber != selIndex) /*不是第三只蛋*/ {
                        m_eggSps[i]->runAction(FadeOut::create(2.0f));
                    }
                } /*for*/
            }
        }
        
        if (!isCorrect) {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[selIndex])),CallFunc::create([this,target,selIndex]{
                target->setPosition(m_originalPoints[selIndex]);
            }), NULL));
        }
    };
    listener->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        unsigned long selIndex = (unsigned long)target->getUserData();
        m_pBN->reorderChild(target, EGG_ZORDER);
        target->setScale(1.0f);
        target->setPosition(m_originalPoints[selIndex]);
    };
    
    // 绑定监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_eggSps[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_eggSps[1]);
    dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_eggSps[2]);
}

/*
 *  进入回合
 */
void Round002_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
}

//
void Round002_04::celebrate(const float delay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

//回合结束分享调用
void Round002_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    // 篮子上升
    basketUp();
}

//
void Round002_04::basketUpComplete()
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);        //停止分享音效
    if (m_completeCB) {
        m_completeCB();
    }
}

//
void Round002_04::basketUp()
{
    auto basketSp = (Sprite*)m_pBN->getChildByTag(BASKET_TAG);
    Vec2 up = Vec2(0.0f, 1420.0f);
    const float duration = 3.2f;
    
    basketSp->runAction(Sequence::create(MoveBy::create(duration, up),CallFunc::create(CC_CALLBACK_0(Round002_04::basketUpComplete, this)), NULL));
    m_eggSps[2]->runAction(MoveBy::create(duration, up));
}

//分享回调
void Round002_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round002_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round002_04::roundEndShareCallBack, this,true));
}


/*  回合结束回调 */
void Round002_04::endRound() {}

//
//  Round009_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round009_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                5.0f                                    // 庆祝延时

#define GK_09_PNG                       "res/stage009/SL-GK09-IT.png"
#define GK_09_PLIST                     "res/stage009/SL-GK09-IT.plist"

#define BOX_POSITION                    Vec2(1290.0f, 360.0f)

#define SONGSHU_PNG                     "res/stage009/SL-GK09-2-MS01-D+C/SL-GK09-2-MS01-D+C.png"
#define CIWEI_PNG                       "res/stage009/SL-GK09-2-MS02-D+C/SL-GK09-2-MS02-D+C.png"
#define SONGSHU_GAF_CONFIG              "res/stage009/SL-GK09-2-MS01-D+C.json"
#define CIWEI_GAF_CONFIG                "res/stage009/SL-GK09-2-MS02-D+C.json"

#define BOX_TAG                         101
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间


/*
 *  constructor
 */
Round009_02::Round009_02():
m_correctNumber(0),
m_pCiweiActor(nullptr),
m_pSongshuActor(nullptr),
m_pSBN(nullptr)
{
    m_targetRect = Rect(1190.0f, 280.0f, 220.0f, 180.0f);
    m_correctNumber = 1;
}

/*
 *  destructor
 */
Round009_02::~Round009_02() {
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round009_02* Round009_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round009_02();
    if (pRound && pRound->initWithRoundIndex(roundIndex,pStageScene,pos))
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
bool Round009_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
   
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(SONGSHU_PNG);
    appendAsyncLoadTexture(CIWEI_PNG);
    appendAsyncLoadTexture(GK_09_PNG);
    appendAsyncLoadGaf("res/stage009/SL-GK09-2-MS01-D+C/SL-GK09-2-MS01-D+C.gaf");
    appendAsyncLoadGaf("res/stage009/SL-GK09-2-MS02-D+C/SL-GK09-2-MS02-D+C.gaf");
    preLoadRes();
    return true;
}

//
void Round009_02::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(GK_09_PLIST);
    
    m_pSBN = SpriteBatchNode::create(GK_09_PNG);
    addChild(m_pSBN, 2);
    
    char szBuffer[32];
    float posx = 964.0f, posy = 916.0f, offsetx = 356.0f;
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK09-2-DA%02ld.png", i + 1);
        m_targetSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_targetSp[i]->setUserData((void*)i);
        m_pSBN->addChild(m_targetSp[i], 2);
        m_targetSp[i]->setPosition(Vec2(posx, posy));
        m_originalPoints[i] = Vec2(posx, posy);
        posx += offsetx;
    } /*for*/
    
    auto spBox = Sprite::createWithSpriteFrameName("SL-GK09-2-TM01-2.png");
    spBox->setPosition(BOX_POSITION);
    m_pSBN->addChild(spBox, 1, BOX_TAG);
    
    // 刺猬
    m_pCiweiActor = Actor::createWithGafConfigFile(CIWEI_GAF_CONFIG);
    m_pCiweiActor->setPosition(Vec2(810.0f, 630.0f));
    m_pCiweiActor->playWithSequenceAliasName("standby", true);
    addChild(m_pCiweiActor, 1);
    
    // 松鼠
    m_pSongshuActor = Actor::createWithGafConfigFile(SONGSHU_GAF_CONFIG);
    m_pSongshuActor->setPosition(Vec2(1290.0f, 680.0f));
    m_pSongshuActor->playWithSequenceAliasName("standby", true);
    addChild(m_pSongshuActor, 1);
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(GK_09_PLIST);
        m_pStageScene->appendRemovedTexture(GK_09_PNG);
    }
}

//开始回合
void Round009_02::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        m_opentouched = true;
        if (m_jumped) {
            openTouch();
        }
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    
    scaleChangeBox();           //改变盒子大小，突出盒子
}

//突出盒子
void Round009_02::scaleChangeBox()
{
    m_pSBN->getChildByTag(BOX_TAG)->runAction(Sequence::create(ScaleTo::create(0.2f,1.1f),ScaleTo::create(0.2f, 1.0f),ScaleTo::create(0.2f, 1.1f),ScaleTo::create(0.2f, 1.0f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

//
void Round009_02::addListener()
{
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        target->setScale(1.0f);
        unsigned long selIndex = (unsigned long)target->getUserData();
        bool isCorrect = false;

        if (m_targetRect.containsPoint(touchPoint) ) {
            if (selIndex == m_correctNumber) /*答对了*/
            {
                isCorrect = true;
                target->setVisible(false);
                (m_pSBN->getChildByTag(BOX_TAG))->setVisible(false);
                auto spBox = Sprite::createWithSpriteFrameName("SL-GK09-2-TM01-1.png");
                spBox->setPosition(BOX_POSITION);
                m_pSBN->addChild(spBox, 2);
                
                // remove listener
                auto eventDisptacher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    eventDisptacher->removeEventListenersForTarget(m_targetSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                celebrate(3.0f);
            }
        }
        if (!isCorrect)
        {
            // 播放答错音效
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
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event) {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
    };
    auto eventDisptacher = Director::getInstance()->getEventDispatcher();
    eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag, m_targetSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_targetSp[i]);
    } /*for*/
}

//庆祝
void Round009_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/* 开启触摸 */
void Round009_02::openTouch()
{
    // 事件监听
    addListener();
}

/*  回合结束回调 */
void Round009_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


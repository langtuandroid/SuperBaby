//
//  Round008_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#include "Round008_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "SocialShareNode.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_GAF_BG_FILE           "res/common/guanghuanxiaoguo/guanghuanxiaoguo.gaf"
#define CELEBRATE_GAF_FILE              "res/common/wujiaoxing03/wujiaoxing03.gaf"
#define CELEBRATE_DELAY                 5.0f                                    // 庆祝延时

#define BN_PNG_FILE                    "res/stage008/SL-GK08-IT.png"             // BN的png文件

#define BN_ZORDER                       1                                       // BN的z值
#define BACK_SCENE_TIME                 2.5f                                    // 返回关卡选择场景延时时间
#define MOVE_ACTOR_TIME                 3.0f                                    // 分享界面ok过后主角移动时间

#define TREE_PNG_FILE                   "res/stage003/SL-GK03-2-TM01.png"       // 树
#define TREE_BLACK_FILE                 "res/stage003/SL-GK03-2-TM06.png"       // 用于表示树内部的黑色精灵
#define TREE_PNG_FILE7                  "res/stage003/SL-GK03-2-TM07.png"       // 用于遮挡主角
#define DOOR_FRAME                      "SL-GK08-4-IT01.png"

#define TREE_POSX                       1840.0f
#define TREE_POSY                       820.0f

#define DOOR_TAG                        101
#define OVER_ACTOR_TAG                  103

// 山羊爷爷
#define GOAT_PNG_FILE                   "res/stage008/SL-GK08-4-TM01-D+C/SL-GK08-4-TM01-D+C.png"
#define GOAT_CONFIG_FILE                "res/stage008/SL-GK08-4-TM01-D+C.json"
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round008_04::Round008_04():
m_correctNumber(1),
m_pSBN(nullptr)
{}

/*
 *  destructor
 */
Round008_04::~Round008_04() {
    cleanUp();
}

/*
 *  清理
 */

void Round008_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round008_04* Round008_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round008_04();
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
bool Round008_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(GOAT_PNG_FILE);
    appendAsyncLoadTexture(TREE_PNG_FILE);
    appendAsyncLoadTexture(TREE_BLACK_FILE);
    appendAsyncLoadTexture(TREE_PNG_FILE7);
    appendRemovedTexture(TREE_PNG_FILE);
    appendRemovedTexture(TREE_BLACK_FILE);
    appendRemovedTexture(TREE_PNG_FILE7);
    appendAsyncLoadGaf("res/stage008/SL-GK08-4-TM01-D+C/SL-GK08-4-TM01-D+C.gaf");
    
    m_pSBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pSBN, BN_ZORDER);
    
    // 魔方
    auto magicCubeSp = Sprite::createWithSpriteFrameName("SL-GK08-4-TM01.png");
    magicCubeSp->setPosition(Vec2(960.0f, 560.0f));
    m_pSBN->addChild(magicCubeSp, 1);
    
    // 数字
    char szBuffer[32];
    float posx = 660.0f,posy = 150.0f,offsetx = 260.0f;
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK08-4-DA%02ld.png", i + 1);
        m_numberSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pSBN->addChild(m_numberSp[i], 1);
        m_numberSp[i]->setUserData((void*)i);
    } /*for*/
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_numberSp[all]->setPosition(Vec2(posx+offsetx*Index, posy));
        all++;
    } /*while*/
    
    preLoadRes();
    return true;
}

//
void Round008_04::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();

    m_pOverActorNode = Node::create();
    m_pStageScene->addOverLeaderNode(m_pOverActorNode, this->getPosition(), OVER_ACTOR_TAG);
    
    // 树
    auto treeSp = Sprite::create(TREE_PNG_FILE);
    addChild(treeSp, 3);
    treeSp->setPosition(Vec2(TREE_POSX, TREE_POSY));
    
    // 门
    auto doorSp = Sprite::createWithSpriteFrameName(DOOR_FRAME);
    doorSp->setPosition(Vec2(1458.0f, 480.0f));
    addChild(doorSp, 2, DOOR_TAG);
    
    // 树洞阴影
    auto treeBlackSp = Sprite::create(TREE_BLACK_FILE);
    treeBlackSp->setPosition(Vec2(1498.0f, 480.0f));
    addChild(treeBlackSp, 1);
    
    // 添加用于遮挡主角的树皮，达到主角从树中穿过的效果
    m_pTree7Sp = Sprite::create(TREE_PNG_FILE7);
    m_pOverActorNode->addChild(m_pTree7Sp, 1);
    m_pTree7Sp->setPosition(Vec2(TREE_POSX + 105.0f, TREE_POSY - 305.0f));
    
    // 山羊爷爷
    m_pGoatActor = Actor::createWithGafConfigFile(GOAT_CONFIG_FILE);
    m_pGoatActor->setPosition(Vec2(670.0f, 1120.0f));
    m_pOverActorNode->addChild(m_pGoatActor, 2);
    m_pGoatActor->playWithSequenceAliasName("standby", true);
}

//开始回合
void Round008_04::onEnterRound()
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
void Round008_04::addListener()
{
    auto listenerTouch = EventListenerTouchOneByOne::create();
    listenerTouch->setSwallowTouches(true);
    listenerTouch->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerTouch->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint))
        {
            unsigned long touchIndex = (unsigned long)target->getUserData();
            if (touchIndex == m_correctNumber) /*答对*/ {
                auto eventDisptacher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    eventDisptacher->removeEventListenersForTarget(m_numberSp[i]);
                } /*for*/
                m_pGoatActor->playWithSequenceAliasName("trigger", true);
                for (unsigned int i = 0; i < 3; i++) {
                    m_numberSp[i]->runAction(FadeOut::create(CELEBRATE_DELAY));
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    //答对时主角说话
                    playLeadAnsCorEff();
                }else
                {
                    //答对时主角说话
                    playLeadAnsCorEff();
                }
                this->celebrate(CELEBRATE_DELAY);
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    //答错时主角说话
                    playLeadAnsErrEff();
                }else
                {
                    //答错时主角说话
                    playLeadAnsErrEff();
                }
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    auto eventDisptacher = Director::getInstance()->getEventDispatcher();
    eventDisptacher->addEventListenerWithSceneGraphPriority(listenerTouch, m_numberSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        eventDisptacher->addEventListenerWithSceneGraphPriority(listenerTouch->clone(), m_numberSp[i]);
    } /*for*/
}

// 庆祝
void Round008_04::celebrate(const float celebrateDelay)
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
void Round008_04::roundEndShareCallBack(bool isShare)
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
    // 门上升
    auto doorSp = (Sprite*)getChildByTag(DOOR_TAG);
    auto doorUpCompletedCB = CallFunc::create([this] {
        moveActor();                        //移动主角
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round008_04::fadeOutLayer), BACK_SCENE_TIME-1.5f);
    });
    doorSp->runAction(Sequence::create(MoveBy::create(5.0f, Vec2(0.0f, 550.0f)), doorUpCompletedCB, NULL));
}

// 移动主角退出回合
void Round008_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000.0f, 0));
}

/* 开启触摸 */
void Round008_04::openTouch()
{
    addListener();
}

/*  回合结束回调 */
void Round008_04::endRound()
{

}

//渐变变黑退出
void Round008_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor,COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round008_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round008_04::roundEndShareCallBack, this, false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round008_04::roundEndShareCallBack, this, true));
}

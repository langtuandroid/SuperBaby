//
//  Round021_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#include "Round021_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "AppConfig.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"

#define MS_GAF_FILE                 "res/stage021/XQ-GK09-1-MS01-D+C/XQ-GK09-1-MS01-D+C.gaf"    // stage gaf
#define MS_GAF_POS                  Vec2(1497,399)
#define MS_GAF_ZORDER               2
#define MS_GAF_CONFIG_FILE          "config/stage021/ms.json"

// 庆祝延时
#define CELEBRATE_DELAY             5.0f
#define STANDBY                     "standby"                                   // 待机动画
#define TRIGGER                     "trigger"                                    // 触发动画

#define BN_PLIST_FILE               "res/stage021/XQ-GK09-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                 "res/stage021/XQ-GK09-IT.png"               // BN的 png 文件
#define ANSWERS_ZORDER              3

// 柱子
#define CYLINDER_PNG_FILE           "res/stage013/XQ-GK01-0-IT-D.png"
#define BRIDGE_POS                  Vec2(1179,376)
#define BRIDGE_PNG_FILE             "XQ-GK09-1-IT01.png"
#define BRIDGE_ZORDER               1

#define OVER_PNG_FILE               "res/stage021/XQ-GK09-BGA1.png"
#define OVER_TAG                    200
#define OVER_POS                    Vec2(718,116)

#define ROPE_PNG_FILE               "XQ-GK09-1-TM01.png"
#define PASSROPE_PNG_FILE           "XQ-GK09-1-TM02.png"
#define ROPE_POS                    Vec2(1324,1091)
#define ROPE_ZORDER                 4

/*
 *  constructor
 */
Round021_01::Round021_01(){
    
    m_answersPos[0] = Vec2(494, 927);
    m_answersPos[1] = Vec2(778, 927);
    m_answersPos[2] = Vec2(1062, 927);
    
}

/*
 *  destructor
 */
Round021_01::~Round021_01()
{
    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
}

//
Round021_01* Round021_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round021_01();
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
bool Round021_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pOverSp = Sprite::create(OVER_PNG_FILE);
    m_pStageScene->addBelowLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    
    m_pLightPillarSp = Sprite::create(CYLINDER_PNG_FILE);
    m_pLightPillarSp->setPosition(Vec2(-905, 1629));
    m_pLightPillarSp->setAnchorPoint(Vec2(0.0f, 1.0f));
    this->addChild(m_pLightPillarSp);
    m_pLightPillarSp->setRotation(-30);

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pRope = Sprite::createWithSpriteFrameName(ROPE_PNG_FILE);
    m_pRope->setPosition(ROPE_POS);
    this->addChild(m_pRope, ROPE_ZORDER);
    
    m_pPassRope = Sprite::createWithSpriteFrameName(PASSROPE_PNG_FILE);
    m_pPassRope->setPosition(ROPE_POS);
    this->addChild(m_pPassRope, ROPE_ZORDER);
    m_pPassRope->setOpacity(0);
    
    m_pBridgeSp = Sprite::createWithSpriteFrameName(BRIDGE_PNG_FILE);
    m_pBridgeSp->setPosition(BRIDGE_POS);
    this->addChild(m_pBridgeSp, BRIDGE_ZORDER);
    m_pBridgeSp->setOpacity(0);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK09-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->setScale(0.0f);
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    
    while (placed < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pAnswersSp[placed]->setPosition(m_answersPos[randIndex]);
        placed++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

//
void Round021_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round021_01::changeScaleNumbersSchedule), 0.5f, -1, 0);
}

//改变大小出现
void Round021_01::changeScaleNumbersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round021_01::changeScaleNumbersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}

/* 开启触摸 */
void Round021_01::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        unsigned long index = (unsigned long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
                changeRope();
                changeActor();
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
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
            }else
            {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//答对后改变绳子
void Round021_01::changeRope()
{
    m_pRope->runAction(FadeOut::create(1.0f));
    m_pPassRope->runAction(FadeIn::create(1.0f));
}

//答对后改变actor动作
void Round021_01::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        m_pMSActor->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0.0f, 1500)),DelayTime::create(1.0f), NULL));
        fadeOutBridge();
        fadeAll();
    });
}

//渐现桥
void Round021_01::fadeOutBridge()
{
    m_pBridgeSp->runAction(FadeIn::create(1.0f));
}

//渐隐消失所有答案
void Round021_01::fadeAll()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswersSp[i]->runAction(FadeOut::create(0.5f));
    }
}

//
void Round021_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(true);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round021_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
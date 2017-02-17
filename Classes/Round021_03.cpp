//
//  Round021_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#include "Round021_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "LeadingActor.h"
#include "AppConfig.h"

#define CELEBRATE_DELAY                 5.0f
#define ANSWER_ZORDER                   3
#define ANSWER_GAF_CONFIG_FILE          "config/stage021/msthree.json"
#define ANSWER_GAF_FILE                 "res/stage021/XQ-GK09-3-MS01-D+C/XQ-GK09-3-MS01-D+C.gaf"
#define ANSWER_GAF_TAG                  200
#define ANSWER_ONE_TAG                  201
#define ANSWER_TWO_TAG                  202

#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGERONE                      "triggerone"                                    // 触发一动画
#define TRIGGERTWO                      "triggertwo"                                    // 触发二动画
#define BOARD_PNG_FILE                  "XQ-GK09-3-TM01.png"
#define BOARD_POS                       Vec2(1197,999)
#define BOARD_ZORDER                    2

// 柱子
#define CYLINDER_PNG_FILE               "res/stage013/XQ-GK01-0-IT-D.png"

#define OVER_PNG_FILE                   "res/stage021/XQ-GK09-BGA2.png"
#define OVER_POS                        Vec2(7757,117)
#define OVER_TAG                        300

#define BRIDGE_PNG_FILE                 "XQ-GK09-4-IT01.png"
#define BRIDGE_POS                      Vect(3176,376)
// 柱子 gaf
#define CYLINDER_GAF                    "res/stage013/XQ-GK01-0-IT-D/XQ-GK01-0-IT-D.gaf"
/*
 *  constructor
 */
Round021_03::Round021_03()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(7461, 593);
    m_answersPos[1] = Vec2(7752, 593);
    m_answersPos[2] = Vec2(8043, 593);
    
    m_answersRect[0] = Rect(779, 417, 284, 353);
    m_answersRect[1] = Rect(1076, 406, 271, 362);
    m_answersRect[2] = Rect(1383, 423, 240, 340);
}

/*
 *  destructor
 */
Round021_03::~Round021_03()
{
    cleanUp();
}

//
void Round021_03::cleanUp()
{
    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    m_pStageScene->removeOBLeaderNode(ANSWER_GAF_TAG);
    m_pStageScene->removeOBLeaderNode(ANSWER_ONE_TAG);
    m_pStageScene->removeOBLeaderNode(ANSWER_TWO_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(ANSWER_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(CYLINDER_PNG_FILE);
}

//
Round021_03* Round021_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round021_03();
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
bool Round021_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pOverSp = Sprite::create(OVER_PNG_FILE);
    m_pStageScene->addBelowLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    m_pLightPillarSp = Sprite::create(CYLINDER_PNG_FILE);
    m_pLightPillarSp->setPosition(Vec2(1924, 1855));
    m_pLightPillarSp->setAnchorPoint(Vec2(0.0f, 1.0f));
    this->addChild(m_pLightPillarSp);
    m_pLightPillarSp->setRotation(30);
    
    m_pBoardSp = Sprite::createWithSpriteFrameName(BOARD_PNG_FILE);
    m_pBoardSp->setPosition(BOARD_POS);
    this->addChild(m_pBoardSp, BOARD_ZORDER);
    
    m_pAnswerActor = Actor::createWithGafConfigFile(ANSWER_GAF_CONFIG_FILE);
    m_pAnswerActor->playWithSequenceAliasName(STANDBY,true);
    m_pStageScene->addBelowLeaderNode(m_pAnswerActor, m_answersPos[2], ANSWER_GAF_TAG);
    m_pAnswerActor->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "XQ-GK09-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        if (i == 0) {
            m_pStageScene->addBelowLeaderNode(m_pAnswersSp[i], m_answersPos[0], ANSWER_ONE_TAG);
        }else
        {
            m_pStageScene->addBelowLeaderNode(m_pAnswersSp[i], m_answersPos[1], ANSWER_TWO_TAG);
        }
    }
    
    m_pBridgeSp = Sprite::createWithSpriteFrameName(BRIDGE_PNG_FILE);
    m_pBridgeSp->setPosition(BRIDGE_POS);
    this->addChild(m_pBridgeSp);
    
    return true;
}

// 进入本回合
void Round021_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round021_03::changeScaleAnswersSchedule), 0.5f, -1, 0.0f);
    
}

//依次放大答案，以提示
void Round021_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round021_03::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    if (m_InitAniIndex == 2) {
        m_pAnswerActor->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
    }else
    {
        m_pAnswersSp[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
    }
    m_InitAniIndex++;
}

/* 开启触摸 */
void Round021_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        for (unsigned int i = 0; i < 3; i++) {
            if (m_answersRect[i].containsPoint(this->convertTouchToNodeSpace(touch))) {
                if (i == 2) {
                    m_pAnswerActor->setScale(1.1f);
                }else
                {
                    m_pAnswersSp[i]->setScale(1.1f);
                }
                return true;
            }
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        for (unsigned int i = 0; i < 3; i++) {
            if (m_answersRect[i].containsPoint(this->convertTouchToNodeSpace(touch))) {
                if (i == 2) {
                    m_pAnswerActor->setScale(1.0f);
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
                }else
                {
                    m_pAnswersSp[i]->setScale(1.0f);
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
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, this);
}

//答对后改变actor动作
void Round021_03::changeActor()
{
    m_pAnswerActor->playWithSequenceAliasName(TRIGGERONE);
    m_pAnswerActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pAnswerActor->playWithSequenceAliasName(TRIGGERTWO,true);
    });
}

/*
 *  庆祝
 */
void Round021_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round021_03::endRound()
{
    auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
    auto moveDuration = 2260.0f / bgScrollSpeed;
    scheduleOnce([this](float){
        m_pOverSp->runAction(FadeOut::create(0.2f));
    }, moveDuration, "delaytime");
    m_pStageScene->manualScroll(2260.0f, [this]
    {
        if (m_completeCB) {
            m_completeCB();
        }
    });
}
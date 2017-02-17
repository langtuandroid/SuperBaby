//
//  Round022_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#include "Round022_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "LeadingActor.h"

#define CELEBRATE_DELAY                 5.0f
#define ANSWER_ZORDER                   3
#define CAKE_GAF_CONFIG_FILE            "config/stage022/cake.json"
#define CAKE_GAF_FILE                   "res/stage022/XQ-GK10-3-TM01-D+C/XQ-GK10-3-TM01-D+C.gaf"
#define CAKE_GAF_POS                    Vec2(1205,1220)
#define CAKE_ZORDER                     2

#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发一动画

#define BRIDGE_PNG_FILE                 "XQ-GK10-2-IT01.png"
#define BRIDGE_POS                      Vec2(1131,372)
#define BRIDGE_ZORDER                   1

#define STAGE_PNG_FILE                  "XQ-GK10-4-IT01.png"
#define STAGE_POS                       Vec2(4080,200)

/*
 *  constructor
 */
Round022_03::Round022_03()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(789, 464);
    m_answersPos[1] = Vec2(1066, 464);
    m_answersPos[2] = Vec2(1321, 464);
}

/*
 *  destructor
 */
Round022_03::~Round022_03()
{
    cleanUp();
}

//
void Round022_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(CAKE_GAF_FILE);
}

//
Round022_03* Round022_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round022_03();
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
bool Round022_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pCakeActor = Actor::createWithGafConfigFile(CAKE_GAF_CONFIG_FILE);
    m_pCakeActor->playWithSequenceAliasName(STANDBY,true);
    m_pCakeActor->setPosAtNode(Vec2(-529, 600));
    m_pCakeActor->setPosition(CAKE_GAF_POS);
    this->addChild(m_pCakeActor, CAKE_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK10-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWER_ZORDER);
        m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/
    
    m_pBridgeSp = Sprite::createWithSpriteFrameName(BRIDGE_PNG_FILE);
    m_pBridgeSp->setPosition(BRIDGE_POS);
    this->addChild(m_pBridgeSp);
    m_pBridgeSp->setOpacity(0);
    
    m_pStageSp = Sprite::createWithSpriteFrameName(STAGE_PNG_FILE);
    m_pStageSp->setPosition(STAGE_POS);
    this->addChild(m_pStageSp);
    
    return true;
}

// 进入本回合
void Round022_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round022_03::changeScaleAnswersSchedule), 0.6f, -1, 0.0f);
    
}

//依次放大答案，以提示
void Round022_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round022_03::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round022_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScaleY(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScaleY(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//答对后桥显示
void Round022_03::fadeBridge()
{
    m_pBridgeSp->runAction(FadeIn::create(0.5f));
}

//答对后改变actor动作
void Round022_03::changeActor()
{
    m_pCakeActor->playWithSequenceAliasName(TRIGGER);
    m_pCakeActor->setAnimationFinishedPlayDelegate([this]
    {
        fadeBridge();
        fadeAnswers();
        m_pCakeActor->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0.0f, 1000)), NULL));
    });
}

//答对后渐隐消失
void Round022_03::fadeAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswersSp[i]->runAction(FadeOut::create(0.5f));
    }
}

/*
 *  庆祝
 */
void Round022_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round022_03::endRound()
{
    m_pStageScene->manualScroll(2600.0f, [this]
    {
        m_pStageScene->moveLeadingActorCB(2.0f, Vec2(900.0f, 0.0f),[this]{
            if (m_completeCB) {
                m_completeCB();
            }
        });
    });
}
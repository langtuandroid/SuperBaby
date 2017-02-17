//
//  Round038_03.cpp
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#include "Round038_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define STAGE_PNG_FILE                  "GC-GK02-4-IT01.png"
#define STAGE_ZORDER                    1
#define STAGE_POS                       Vec2(3400,-9)

/*
 *  constructor
 */
Round038_03::Round038_03() {
    
    m_answersPos[0] = Vec2(700, 1158);
    m_answersPos[1] = Vec2(788, 900);
    m_answersPos[2] = Vec2(1070, 1106);
    m_answersPos[3] = Vec2(1146, 830);
    m_answersPos[4] = Vec2(1412, 1060);
}

/*
 *  destructor
 */
Round038_03::~Round038_03()
{
    cleanUp();
}

//
void Round038_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round038_03* Round038_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round038_03();
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
bool Round038_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    srand((unsigned)time(0));
    m_randNum = rand() % 4;
    
    unsigned long number = 0;
    char buffer[64];
    for (unsigned long i = 0; i < 5; i++) {
        if (m_randNum == i || m_randNum+1 == i) {
            sprintf(buffer, "GC-GK03-TM%02ld.png",number+1);
            if (m_randNum+1 == i) {
                number++;
            }
        }else
        {
            sprintf(buffer, "GC-GK03-TM%02ld.png",number+1);
            number++;
        }
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/
    
    m_pStageSp = Sprite::createWithSpriteFrameName(STAGE_PNG_FILE);
    m_pStageSp->setPosition(STAGE_POS);
    this->addChild(m_pStageSp, STAGE_ZORDER);

    return true;
}

// 进入本回合
void Round038_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round038_03::scaleChangeAnswersSchedule), 0.6f, -1, 0);   //
}

//上升答案
void Round038_03::scaleChangeAnswersSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_03::scaleChangeAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round038_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
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
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height+100);
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == m_randNum || index == m_randNum+1) {   //答对
                m_curCorrectNum++;
        
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                if (m_curCorrectNum == 2) {
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    celebrate(CELEBRATE_DELAY);
                    fadeOutAnswers();
                    for (unsigned int i = 0; i < 5; i++) {
                        if (i != m_randNum || i != m_randNum) {
                            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                        }
                    } /*for*/
                }else
                {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
                }

            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//答对后渐隐答案
void Round038_03::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 5; i++) {
        m_pAnswersSp[i]->runAction(FadeOut::create(0.3f));
    }
}

/*
 *  庆祝
 */
void Round038_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

//移动主角
void Round038_03::moveHeadActor()
{
    m_pStageScene->manualScroll(2185.0f,[this]{
        m_pStageScene->moveAimiActor(2.0f, Vec2(615.0f, 0.0f),true);
        m_pStageScene->moveBeibeiActorCB(2.0f, Vec2(615.0f, 0.0f),[this]{
            if (m_completeCB) {
                m_completeCB();
            }
        });
        
    });
}

/*  回合结束回调 */
void Round038_03::endRound()
{
    moveHeadActor();
}
//
//  Round043_02.cpp
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#include "Round043_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  2
#define BOARD_PNG_FILE                  "GC-GC07-2-TM01.png"
#define BOARD_POS                       Vec2(1000,843)
#define BOARD_ZORDER                    1
#define NUMBER_PNG_FILE                 "GC-GC07-2-TM02.png"
#define NUMBER_POS                      Vec2(994,892)
#define NUMBER_ZORDER                   2

/*
 *  constructor
 */
Round043_02::Round043_02()
{
    m_answersPos[0] = Vec2(825, -100);
    m_answersPos[1] = Vec2(1063, -100);
    m_answersPos[2] = Vec2(1301, -100);
}

/*
 *  destructor
 */
Round043_02::~Round043_02()
{
    cleanUp();
}

//
Round043_02* Round043_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round043_02();
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
bool Round043_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoardSp = Sprite::createWithSpriteFrameName(BOARD_PNG_FILE);
    m_pBoardSp->setPosition(BOARD_POS);
    this->addChild(m_pBoardSp, BOARD_ZORDER);
    
    m_pNumberSp =Sprite::createWithSpriteFrameName(NUMBER_PNG_FILE);
    m_pNumberSp->setPosition(NUMBER_POS);
    m_pNumberSp->setOpacity(0);
    this->addChild(m_pNumberSp, NUMBER_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GC07-2-DA%02ld.png",i+1);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswers[i]->setUserData((void*)i);
        addChild(m_pAnswers[i],ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswers[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round043_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round043_02::upAnswersSchedule), 0.4f, -1, 0);
}

//上升答案
void Round043_02::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round043_02::upAnswersSchedule));
        showNumber();
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, 300.0f))));
}

void Round043_02::showNumber()
{
    m_pNumberSp->runAction(Sequence::create(FadeIn::create(0.3f),
                                            FadeOut::create(0.3f),
                                            FadeIn::create(0.3f),
                                            FadeOut::create(0.3f),
                                            CallFunc::create([this]{
                                                m_jumped = true;
                                                if (m_opentouched) {
                                                    openTouch();
                                                }
                                            }), NULL));
}

//
void Round043_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

/* 开启触摸 */
void Round043_02::openTouch()
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
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 2) {   //答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswers[i]);
                } /*for*/
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswers[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswers[i]);
    } /*for*/
}

void Round043_02::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round043_02::downAnswersSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(Sequence::create(MoveBy::create(0.3f, Vec2(0.0f, 50.0f)),MoveBy::create(0.3f, Vec2(0.0f, -350.0f)), NULL));
}

/*
 *  庆祝
 */
void Round043_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round043_02::endRound()
{
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round043_02::downAnswersSchedule), 0.6f, -1, 0);
}
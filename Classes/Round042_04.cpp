//
//  Round042_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#include "Round042_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define APPLE_PNG_FILE                  "GC-GK06-3-TM01.png"
#define APPLE_ZORDER                    1

/*
 *  constructor
 */
Round042_04::Round042_04()
{
    m_answersPos[0] = Vec2(808, -100);
    m_answersPos[1] = Vec2(1076, -100);
    m_answersPos[2] = Vec2(1344, -100);
    
    m_applesPos[0] = Vec2(370, 990);
    m_applesPos[1] = Vec2(472, 1116);
    m_applesPos[2] = Vec2(554, 1034);
    m_applesPos[3] = Vec2(638, 1162);
    m_applesPos[4] = Vec2(770, 988);
    m_applesPos[5] = Vec2(787, 1192);
    m_applesPos[6] = Vec2(920, 1066);
    m_applesPos[7] = Vec2(1140, 950);
    m_applesPos[8] = Vec2(1192, 1156);
    m_applesPos[9] = Vec2(1304, 1066);
    m_applesPos[10] = Vec2(1371, 1262);
    m_applesPos[11] = Vec2(1458, 1138);
    m_applesPos[12] = Vec2(1590, 1056);
    m_applesPos[13] = Vec2(1608, 1192);
    m_applesPos[14] = Vec2(1772, 1046);

}

/*
 *  destructor
 */
Round042_04::~Round042_04()
{
    cleanUp();
}

//
void Round042_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round042_04* Round042_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round042_04();
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
bool Round042_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    
    for (unsigned int i = 0; i < 15; i++) {
        m_pApplesSp[i] = Sprite::createWithSpriteFrameName(APPLE_PNG_FILE);
        m_pApplesSp[i]->setPosition(m_applesPos[i]);
        this->addChild(m_pApplesSp[i],APPLE_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK06-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
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
    
    return true;
}

// 进入本回合
void Round042_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round042_04::upAnswersSchedule), 0.4f, -1, 0);   //
}

//上升答案
void Round042_04::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round042_04::upAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round042_04::changeScaleApplesSchedule), 1.0f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, 300))));
}

void Round042_04::changeScaleApplesSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round042_04::changeScaleApplesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    for (unsigned int i = 0; i < 15; i++) {
        m_pApplesSp[i]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f), NULL));
    }
    m_InitAniIndex++;
}

/* 开启触摸 */
void Round042_04::openTouch()
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 0) {   //答对
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round042_04::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round042_04::downAnswersSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(MoveBy::create(0.4f, Vec2(0.0f, 50)),MoveBy::create(0.4f, Vec2(0.0f, -350.0f)),NULL));
}

/*
 *  庆祝
 */
void Round042_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round042_04::endRound()
{
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round042_04::downAnswersSchedule), 0.8f, -1, 0);   //
}
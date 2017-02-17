//
//  Round039_02.cpp
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#include "Round039_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  2
#define MONKEY_PNG_FILE                 "GC-GK03-2-MS.png"
#define MONKEY_POS                      Vec2(1585,485)
#define MONKEY_ZORDER                   1
#define STAGE_PNG_FILE                  "res/stage039/GC-GK03-2-IT.png"
#define STAGE_ZORDER                    1
#define STAGE_POS                       Vec2(3500,-39)

/*
 *  constructor
 */
Round039_02::Round039_02()
{
    m_answersPos[0] = Vec2(785, 1005);
    m_answersPos[1] = Vec2(1141, 721);
    m_answersPos[2] = Vec2(1205, 1161);
    m_answersPos[3] = Vec2(1629, 1065);
    
}

/*
 *  destructor
 */
Round039_02::~Round039_02()
{
    cleanUp();
}

//
Round039_02* Round039_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round039_02();
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
bool Round039_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMonkeySp = Sprite::createWithSpriteFrameName(MONKEY_PNG_FILE);
    m_pMonkeySp->setPosition(MONKEY_POS);
    this->addChild(m_pMonkeySp, MONKEY_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "GC-GK03-2-DA%02ld.png",i+1);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswers[i]->setUserData((void*)i);
        addChild(m_pAnswers[i],ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswers[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/
    
    m_pStageSp = Sprite::create(STAGE_PNG_FILE);
    m_pStageSp->setPosition(STAGE_POS);
    this->addChild(m_pStageSp, STAGE_ZORDER);
    return true;
}

// 进入本回合
void Round039_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round039_02::scaleChangeAnswersSchedule), 0.4f, -1, 0);
}

//改变答案的大小
void Round039_02::scaleChangeAnswersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_02::scaleChangeAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

//
void Round039_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    Director::getInstance()->getTextureCache()->removeTextureForKey(STAGE_PNG_FILE);
}

/* 开启触摸 */
void Round039_02::openTouch()
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
                
                for (unsigned int i = 0; i < 4; i++) {
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
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswers[i]);
    } /*for*/
}

void Round039_02::fadeOutAll()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_pAnswers[i]->runAction(FadeOut::create(0.3f));
    }
    m_pMonkeySp->runAction(Sequence::create(FadeOut::create(0.3f),CallFunc::create([this]{
        moveLeadActor();
    }), NULL));
}

/*
 *  庆祝
 */
void Round039_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

void Round039_02::moveLeadActor()
{
    m_pStageScene->manualScroll(2345.0f,[this]{
        m_pStageScene->moveAimiActor(4.0f, Vec2(725.0f, 0.0f),true);
        m_pStageScene->moveBeibeiActorCB(4.0f, Vec2(725.0f, 0.0f),[this]{
            if (m_completeCB) {
                m_completeCB();
            }
        });
        
    });
}

/*  回合结束回调 */
void Round039_02::endRound()
{
    fadeOutAll();
}
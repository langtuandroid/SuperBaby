//
//  Round038_02.cpp
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#include "Round038_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define ANSWERS_ZORDER                  2
#define BAT_GAF_CONFIG_FILE             "res/stage038/bat.json"
#define BAT_GAF_FILE                    "res/stage038/GC-GK02-2-MS01-D+C/GC-GK02-2-MS01-D+C.gaf"
#define BAT_GAF_ZORDER                  3
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画

/*
 *  constructor
 */
Round038_02::Round038_02()
{
    m_answersPos[0] = Vec2(868, -100);
    m_answersPos[1] = Vec2(1126, -100);
    m_answersPos[2] = Vec2(1384, -100);
    
    m_batsPos[0] = Vec2(620, 1237);
    m_batsPos[1] = Vec2(1145, 1150);
    m_batsPos[2] = Vec2(1405, 1264);
    m_batsPos[3] = Vec2(1584, 1310);
    m_batsPos[4] = Vec2(1787, 1352);
}

/*
 *  destructor
 */
Round038_02::~Round038_02()
{
    cleanUp();
}

//
Round038_02* Round038_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round038_02();
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
bool Round038_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ2-%02ld.png",i*2+3);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswers[i]->setUserData((void*)i);
        addChild(m_pAnswers[i]);
    }
    
    for (unsigned long i = 0; i < 5; i++) {
        m_pBatsActor[i] = Actor::createWithGafConfigFile(BAT_GAF_CONFIG_FILE);
        m_pBatsActor[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        m_pBatsActor[i]->setPosition(m_batsPos[i]);
        addChild(m_pBatsActor[i],BAT_GAF_ZORDER);
        if (i == 0) {
            m_pBatsActor[i]->playWithSequenceAliasName(TRIGGER,true);
        }else
        {
            m_pBatsActor[i]->playWithSequenceAliasName(STANDBY,true);
        }
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
void Round038_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round038_02::upAnswersSchedule), 0.4f, -1, 0);
}

//答案上升
void Round038_02::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_02::upAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round038_02::scaleChangeBatsSchedule), 0.6f, -1, 0);
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, 300.0f))));
}

//改变蝙蝠的大小
void Round038_02::scaleChangeBatsSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_02::scaleChangeBatsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pBatsActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//
void Round038_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(BAT_GAF_FILE);
}

/* 开启触摸 */
void Round038_02::openTouch()
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
            if (index == 1) {   //答对
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

//答案下降
void Round038_02::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_02::downAnswersSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, -300.0f))));
}


/*
 *  庆祝
 */
void Round038_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

/*  回合结束回调 */
void Round038_02::endRound()
{
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round038_02::downAnswersSchedule), 0.4f, -1, 0);
}
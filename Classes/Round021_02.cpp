//
//  Round021_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#include "Round021_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "CustomMessage.h"
#include "NpcData.h"
#include "NpcDataManager.h"

#define CELEBRATE_DELAY             5.0f
#define PARTS_ZORDER                3
#define MS_GAF_CONFIG_FILE       "config/stage021/mstwo.json"
#define MS_GAF_POS               Vec2(1592,415)
#define MS_GAF_ZORDER            1
#define MS_GAF_FILE              "res/stage021/XQ-GK09-2-MS01-D+C/XQ-GK09-2-MS01-D+C.gaf"
#define STANDBY                     "standby"                                   // 待机动画
#define TRIGGER                     "trigger"                                    // 触发动画

#define MOV_TIME                   0.2f                                        // 答错回原位置移动时间
#define NUMBER_OF_ANSWER_RIGHT     2                                            // 答题正确数量达到该数字怎本回合结束
#define REPEATACTION_TAG           100
#define ANSWER_ZORDER              2
#define TOPIC_ZORDER               3

#define BRIDGE_PNG_FILE             "XQ-GK09-3-IT01.png"
#define BRIDGE_POS                  Vec2(2516,386)

/*
 *  constructor
 */
Round021_02::Round021_02()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(773, 467);
    m_answersPos[1] = Vec2(1046, 467);
    m_answersPos[2] = Vec2(1319, 467);
    
}

/*
 *  destructor
 */
Round021_02::~Round021_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}



//
Round021_02* Round021_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round021_02();
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
bool Round021_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    
    char buffer[64];
    float topicX = 913; float offSetX = 300; float topicY = 979;
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "XQ-GK09-2-TM01.png");
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(Vec2(topicX+i*offSetX, topicY));
        this->addChild(m_pTopicsSp[i], TOPIC_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK09-2-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], PARTS_ZORDER);
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
    
    return true;
}

// 进入本回合
void Round021_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round021_02::changeScaleAnswersSchedule), 0.6f, -1, 0);
}

//答案改变大小以提示
void Round021_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round021_02::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round021_02::changeScaleTopicsSchedule), 0.6f, -1, 0);
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//题目改变大小以提示
void Round021_02::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round021_02::changeScaleTopicsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round021_02::openTouch()
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
                changeActor();
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round021_02::fadeOutAll), 0.5f);
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

//答对后渐隐all
void Round021_02::fadeOutAll(float)
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswersSp[i]->runAction(FadeOut::create(0.4f));
    }
    
    for (unsigned int j = 0; j < 2; j++) {
        m_pTopicsSp[j]->runAction(FadeOut::create(0.4f));
    }
}

//答对后改变actor动作
void Round021_02::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER,true);
}

/*
 *  庆祝
 */
void Round021_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round021_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
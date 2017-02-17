//
//  Round022_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#include "Round022_02.h"
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

#define CELEBRATE_DELAY                         5.0f
#define FLOWER_CORRECT_GAF_CONFIG_FILE          "config/stage022/flower.json"
#define FLOWER_CORRECT_GAF_FILE                 "res/stage022/XQ-GK10-2-TM02-D+C/XQ-GK10-2-TM02-D+C.gaf"
#define FLOWER_GAF_FILE                         "res/stage022/XQ-GK10-2-TM01-D/XQ-GK10-2-TM01-D.gaf"
#define FLOWER_GAF_ZORDER                       1
#define STANDBY                                 "standby"                                   // 待机动画
#define TRIGGER                                 "trigger"                                    // 触发动画

/*
 *  constructor
 */
Round022_02::Round022_02()
:m_InitAniIndex(0)
{
    m_flowersPos[0] = Vec2(752, 637);
    m_flowersPos[1] = Vec2(1086, 538);
    m_flowersPos[2] = Vec2(1442, 637);
    m_flowersPos[3] = Vec2(1765, 538);
}

/*
 *  destructor
 */
Round022_02::~Round022_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(FLOWER_CORRECT_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(FLOWER_GAF_FILE);
    
}



//
Round022_02* Round022_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round022_02();
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
bool Round022_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    for (unsigned long i = 0; i < 4; i++) {
        if (i == 0) {
            m_pFlowersActor[i] = Actor::createWithGafConfigFile(FLOWER_CORRECT_GAF_CONFIG_FILE);
            m_pFlowersActor[i]->playWithSequenceAliasName(STANDBY,true);
        }else
        {
            m_pFlowersActor[i] = Actor::createWithGafFile(FLOWER_GAF_FILE);
            m_pFlowersActor[i]->setSize(Size(240, 400));
            m_pFlowersActor[i]->setPosAtNode(Vec2(-175, 539));
        }
        
        m_pFlowersActor[i]->setUserData((void*)i);
        this->addChild(m_pFlowersActor[i], FLOWER_GAF_ZORDER);
        m_pFlowersActor[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pFlowersActor[all]->setPosition(m_flowersPos[randIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round022_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round022_02::changeScaleTopicsSchedule), 0.6f, -1, 0);
}

//题目改变大小以提示
void Round022_02::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round022_02::changeScaleTopicsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pFlowersActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round022_02::openTouch()
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
            if (index == 0) {
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
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pFlowersActor[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pFlowersActor[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pFlowersActor[i]);
    } /*for*/
    
}

//答对后改变actor动作
void Round022_02::changeActor()
{
    m_pFlowersActor[0]->playWithSequenceAliasName(TRIGGER,true);
}

/*
 *  庆祝
 */
void Round022_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round022_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


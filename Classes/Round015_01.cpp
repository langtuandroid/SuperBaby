//
//  Round015_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/14.
//
//

#include "Round015_01.h"
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

#define NPC_GAF_FILE                "res/stage015/XQ-GK03-1-DA02/XQ-GK03-1-DA02.gaf"    // NPC gaf
#define NPC_KEY                     "NPC-070"
#define NPC_COLLECT_DURATION        1.5f

#define ANSWER_GAF_CONFIG_FILE      "config/stage015/robot.json"
#define RED_ROBOT_GAF_FILE          "res/stage015/XQ-GK03-1-DA03/XQ-GK03-1-DA03.gaf"

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"
#define STANDBY                        "standby"                                // 待机动画
#define TRIGGER                        "trigger"                                // 触发动画

/*
 *  constructor
 */
Round015_01::Round015_01(){

    m_robotsPos[0] = Vec2(825, 603);
    m_robotsPos[1] = Vec2(1258, 603);
    m_robotsPos[2] = Vec2(1691, 603);
}

/*
 *  destructor
 */
Round015_01::~Round015_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(RED_ROBOT_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage015/XQ-GK03-1-DA01-D+C/XQ-GK03-1-DA01-D+C.gaf");
}

//
Round015_01* Round015_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round015_01();
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
bool Round015_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 0) {
            m_pRobotActors[i] = Actor::createWithGafConfigFile(ANSWER_GAF_CONFIG_FILE);
            m_pRobotActors[i]->playWithSequenceAliasName(STANDBY,true);
        }else if (i == 1)
        {
            m_pRobotActors[i] = Actor::createWithGafFile(NPC_GAF_FILE);
            m_pRobotActors[i]->setSize(Size(360, 440));
            m_pRobotActors[i]->setPosAtNode(Vec2(-103, 666));
        }else
        {
            m_pRobotActors[i] = Actor::createWithGafFile(RED_ROBOT_GAF_FILE);
            m_pRobotActors[i]->setSize(Size(360, 440));
            m_pRobotActors[i]->setPosAtNode(Vec2(-94, 641));
        }
        m_pRobotActors[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
        m_pRobotActors[i]->setUserData((void*)i);
        if (i == 1) {
            this->addChild(m_pRobotActors[i],2);
        }else
        {
            this->addChild(m_pRobotActors[i]);
        }
        
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    
    while (placed < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pRobotActors[placed]->setPosition(m_robotsPos[randIndex]);
        placed++;
    } /*while*/
    
    return true;
}

//
void Round015_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        MyAudioEngine::getInstance()->playAudio(NPC_ENTER_BAG_MUSIC_FILE);
        processNpcCollect();
        std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
        MyAudioEngine::getInstance()->playAudio(effStr.c_str());
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
}

/* 开启触摸 */
void Round015_01::openTouch()
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
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            unsigned long index = (unsigned long)target->getUserData();
            if (index == 0) /*答对*/ {
                changeActor();                                              //改变actor动作
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                this->celebrate(CELEBRATE_DELAY);
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    dispatcher->removeEventListenersForTarget(m_pRobotActors[i]);
                } /*for*/
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
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pRobotActors[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pRobotActors[i]);
    } /*for*/
}

//答对后改变actor动作
void Round015_01::changeActor()
{
    m_pRobotActors[0]->playWithSequenceAliasName(TRIGGER,true);
}

//
void Round015_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round015_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round015_01::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(100, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pRobotActors[1]->setVisible(false);
    }), NULL);
    m_pRobotActors[1]->runAction(npcActions);
    
    //
    scheduleOnce([this] (float dt)
    {
         auto pNpcManager = NpcDataManager::getInstance();
         auto pNpcData = pNpcManager->getNpcDataByKey(NPC_KEY);
         if (pNpcData != nullptr ) {
             pNpcData->collect();
         }
         m_pStageScene->getRoundUINode()->setCollectNpcKey(NPC_KEY);
         m_pStageScene->getRoundUINode()->changeBagGafActor(2);
    }, NPC_COLLECT_DURATION, "npcCollected");
}

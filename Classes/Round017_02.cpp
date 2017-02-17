//
//  Round017_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round017_02.h"
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

#define CELEBRATE_DELAY                 5.0f
#define NUMBERS_ZORDER                  1
#define MAGICCUBE_GAF_CONFIG_FILE       "config/stage017/magiccube.json"
#define MAGICCUBE_GAF_POS               Vec2(850,635)
#define MAGICCUBE_GAF_ZORDER            2
#define MAGICCUBE_GAF_FILE              "res/stage017/XQ-GK05-2-MS-D+C/XQ-GK05-2-MS-D+C.gaf"
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画

#define NPC_GAF_FILE                "res/npc/NPC-065-D+C/NPC-065-D+C.gaf"    // NPC gaf
#define NPC_KEY                     "NPC-065"
#define NPC_COLLECT_DURATION        1.5f
#define NPC_GAF_POS                 Vec2(1338,1775)
#define NPC_GAF_ZORDER              3
#define NPC_GAF_CONFIG_FILE         "res/npc/NPC-065-D+C.json"
#define NPC_ENTER_BAG_MUSIC_FILE   "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round017_02::Round017_02()
:m_InitAniIndex(0)
{
    m_answerNumbersPos[0] = Vec2(880, 190);
    m_answerNumbersPos[1] = Vec2(1128, 190);
    m_answerNumbersPos[2] = Vec2(1376, 190);
}

/*
 *  destructor
 */
Round017_02::~Round017_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(MAGICCUBE_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
}



//, const Vec2& pos
Round017_02* Round017_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round017_02();
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
bool Round017_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pAirShipActor = Actor::createWithGafConfigFile(MAGICCUBE_GAF_CONFIG_FILE);
    m_pAirShipActor->setPosition(MAGICCUBE_GAF_POS);
    this->addChild(m_pAirShipActor, MAGICCUBE_GAF_ZORDER);
    m_pAirShipActor->playWithSequenceAliasName(STANDBY,true);

    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ6-%02ld.png",i+2);
        m_pAnswerNumbersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswerNumbersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswerNumbersSp[i], NUMBERS_ZORDER);
        m_pAnswerNumbersSp[i]->setScale(0.0f);
        m_pAnswerNumbersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswerNumbersSp[all]->setPosition(m_answerNumbersPos[randIndex]);
        all++;
    } /*while*/
    
    m_pNpcActor = Actor::createWithGafConfigFile(NPC_GAF_CONFIG_FILE);
    m_pNpcActor->playWithSequenceAliasName(STANDBY,true);
    m_pNpcActor->setPosition(NPC_GAF_POS);
    this->addChild(m_pNpcActor, NPC_GAF_ZORDER);
    
    return true;
}

// 进入本回合
void Round017_02::onEnterRound()
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
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        changeActor();
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round017_02::changeScaleNumbersSchedule), 0.4f, -1, 0);
}

//改变大小出现
void Round017_02::changeScaleNumbersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round017_02::changeScaleNumbersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswerNumbersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}


/* 开启触摸 */
void Round017_02::openTouch()
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
            if (index == 2) {
                
                scaleChangeNumbers();
                
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
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
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswerNumbersSp[i]);
                } /*for*/
            }else
            {
                // 播放答错音效
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswerNumbersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswerNumbersSp[i]);
    } /*for*/
    
}


//答对后改变actor动作
void Round017_02::changeActor()
{
    m_pAirShipActor->playWithSequenceAliasName(TRIGGER);
    m_pAirShipActor->setAnimationFinishedPlayDelegate([this]
    {
        upAirShip();
    });
}

//上升飞船
void Round017_02::upAirShip()
{
    m_pAirShipActor->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0.0f, 1000)),CallFunc::create([this]{
        
        MyAudioEngine::getInstance()->playAudio(NPC_ENTER_BAG_MUSIC_FILE);
        processNpcCollect();
        std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
        MyAudioEngine::getInstance()->playAudio(effStr.c_str());
    }), NULL));
}

//答对后隐藏答案
void Round017_02::scaleChangeNumbers()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswerNumbersSp[i]->runAction(ScaleTo::create(0.4f, 0.0f));
    }
}

/*
 *  庆祝
 */
void Round017_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round017_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round017_02::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(100, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pNpcActor->setVisible(false);
    }), NULL);
    m_pNpcActor->runAction(npcActions);
    
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
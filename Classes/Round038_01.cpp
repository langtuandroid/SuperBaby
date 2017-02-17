//
//  Round038_01.cpp
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#include "Round038_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "CustomMessage.h"
#include "MyAudioEngine.h"
#include "AppConfig.h"

#define BN_PLIST_FILE                  "res/stage038/GC-GK02-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage038/GC-GK02-IT.png"               // BN的 png 文件
#define TOOLS_ZORDER                    2

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define NPC_GAF_CONFIG_FILE             "res/npc/NPC-003-D+C.json"
#define NPC_GAF_FILE                    "res/npc/NPC-003-D+C/NPC-003-D+C.gaf"    // NPC gaf
#define NPC_KEY                         "NPC-003"
#define NPC_COLLECT_DURATION            1.5f
#define NPC_ZORDER                      2
#define NPC_POS                         Vec2(1486,409)
#define NPC_ENTER_BAG_MUSIC_FILE        "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round038_01::Round038_01()
{
    m_toolsPos[0] = Vec2(734, 617);
    m_toolsPos[1] = Vec2(923, 897);
    m_toolsPos[2] = Vec2(1266, 759);
    
}

/*
 *  destructor
 */
Round038_01::~Round038_01()
{
    cleanUp();
}

//
Round038_01* Round038_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round038_01();
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
bool Round038_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pNpcActor = Actor::createWithGafConfigFile(NPC_GAF_CONFIG_FILE);
    m_pNpcActor->setPosition(NPC_POS);
    m_pNpcActor->playWithSequenceAliasName("standby",true);
    this->addChild(m_pNpcActor, NPC_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK02-DA%02ld.png",i+1);
        m_pToolsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pToolsSp[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        addChild(m_pToolsSp[i], TOOLS_ZORDER);
        m_pToolsSp[i]->setUserData((void*)i);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pToolsSp[all]->setPosition(m_toolsPos[randIndex]);
        all++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }

    return true;
}

// 进入本回合
void Round038_01::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round038_01::scaleChangeToolSchedule), 0.4f, -1, 0);
}

//改变山的大小，以提示
void Round038_01::scaleChangeToolSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_01::scaleChangeToolSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    m_pToolsSp[m_InitAniIndex++]->runAction(ScaleTo::create(0.4f, 1.0f));
}

//
void Round038_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
}

/* 开启触摸 */
void Round038_01::openTouch()
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
        long index = (long)target->getUserData();
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 2) {   //答对
                
                target->runAction(Sequence::create(FadeOut::create(0.3f),CallFunc::create([this]{
                    fadeOutTools();
                }), NULL));
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
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
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pToolsSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pToolsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pToolsSp[i]);
    } /*for*/
}

//答对后渐隐
void Round038_01::fadeOutTools()
{
    for (unsigned long i = 0; i < 3; i++) {
        if (i != 2) {
            m_pToolsSp[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

/*
 *  庆祝
 */
void Round038_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round038_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round038_01::processNpcCollect()
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



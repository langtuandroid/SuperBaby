//
//  RoundNpc.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/09.
//
//

#include "RoundNpc.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "NpcActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "CustomMessage.h"
#include "AppConfig.h"
#include "GafAssetCache.h"
//#include "adjustPosNode.h"

#define NPC_COLLECT_DURATION        1.5f
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
RoundNpc::RoundNpc() {}

/*
 *  destructor
 */
RoundNpc::~RoundNpc()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    
    GafAssetCache::getInstance()->removeGafAssetForKey(m_npcGafFile);
}

//
RoundNpc* RoundNpc::createWithNpc(const int roundIndex, const std::string& npcKey,
                                  const Vec2& npcPosition, StageScene* pStageScene, const Vec2& nodePos)
{
    auto pRound = new RoundNpc();
    if (pRound && pRound->initWithNpc(roundIndex, npcKey, npcPosition, pStageScene, nodePos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

bool RoundNpc::initWithNpc(const int roundIndex, const std::string& npcKey,
                           const Vec2& npcPosition, StageScene* pStageScene, const Vec2& nodePos)
{
    setAnchorPoint(Vec2::ZERO);
    setPosition(nodePos);
    
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    m_npcKey = npcKey;
    m_npcPos = npcPosition;
    
    std::string gafName = m_npcKey + "-D+C";
    
    m_npcGafFile = "res/npc/" + gafName + "/" + gafName + ".gaf";
    m_npcGafConfig = "res/npc/" + gafName + ".json";
    m_pNpcActor = NpcActor::createWithGafConfigFile(m_npcGafConfig, true);
    m_pNpcActor->setPosition(m_npcPos);
    m_pNpcActor->standby();
    addChild(m_pNpcActor, 20);
    m_pNpcActor->showNpcBase(true);
    
//    auto anode = adjustPosNode::createWithNode(m_pNpcActor);
//    addChild(anode, 100);
    
    return true;
}

//
void RoundNpc::onEnterRound()
{
    auto npcListen = EventListenerTouchOneByOne::create();
    npcListen->setSwallowTouches(true);
    npcListen->onTouchBegan = [this] (Touch* pTouch, Event* pEvent) ->bool
    {
        auto pTarget = (NpcActor*)pEvent->getCurrentTarget();
        auto touchPoint = pTarget->convertTouchToNodeSpace(pTouch);
        auto touchRect = Rect(0.0f, 0.0f, pTarget->getSize().width, pTarget->getSize().height);
        if (touchRect.containsPoint(touchPoint))
        {
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pNpcActor);
            MyAudioEngine::getInstance()->playAudio(NPC_ENTER_BAG_MUSIC_FILE);
            m_pNpcActor->showNpcBase(false);
            processNpcCollect();
            std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
            MyAudioEngine::getInstance()->playAudio(effStr.c_str());
        }
        return false;
    };
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(npcListen, m_pNpcActor);
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
                                       {
                                           auto dispatcher = Director::getInstance()->getEventDispatcher();
                                           dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
                                           endRound();
                                       });
}

void RoundNpc::addBlowNode(Node* pNode,const Vec2& pos)
{
    CCASSERT(pNode != nullptr, "invalid node");
    pNode->setPosition(pos);
    this->addChild(pNode, 10);
}

//
void RoundNpc::processNpcCollect()
{
    auto npcCollectedCB = CallFunc::create([this]
                                           {
                                               auto pNpcManager = NpcDataManager::getInstance();
                                               auto pNpcData = pNpcManager->getNpcDataByKey(m_npcKey);
                                               if (pNpcData != nullptr ) {
                                                   pNpcData->collect();
                                               }
                                               m_pStageScene->getRoundUINode()->setCollectNpcKey(m_npcKey);
                                               m_pStageScene->getRoundUINode()->changeBagGafActor(2);
                                               m_pNpcActor->showNpcBase(false);
                                           } );
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(400, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pNpcActor->setVisible(false);
    }), NULL);
    m_pNpcActor->runAction(Sequence::create(npcActions, npcCollectedCB, NULL));
    m_pNpcActor->showNpcArrow(false);
}

void RoundNpc::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


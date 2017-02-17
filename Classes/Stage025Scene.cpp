//
//  Stage025Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#include "Stage025Scene.h"
#include "Player.h"
#include "Round025_01.h"
#include "Round025_02.h"
#include "Round025_03.h"
#include "Round025_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 220.0f)
#define NPC_ONE_KEY                 "NPC-025"
#define NPC_TWO_KEY                 "NPC-075"

/*
 *  constructor
 */
Stage025Scene::Stage025Scene() {}

/*
 *  destructor
 */
Stage025Scene::~Stage025Scene(){}

/*
 *  生成场景
 */
Stage025Scene* Stage025Scene::createWithStageData(const StageData* pStageData)
{
    Stage025Scene* pStage025Scene = new Stage025Scene();
    if (pStage025Scene && pStage025Scene->initWithStageData(pStageData))
    {
        pStage025Scene->autorelease();
        return pStage025Scene;
    } else {
        CC_SAFE_DELETE(pStage025Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage025Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round025_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round025_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round025_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round025_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage025Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage025Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage025Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
  
    return roundNode;
}

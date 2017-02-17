//
//  Stage032Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/06/09.
//
//

#include "Stage032Scene.h"
#include "Player.h"
#include "Round032_01.h"
#include "Round032_02.h"
#include "Round032_03.h"
#include "Round032_04.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 220.0f)
#define NPC_ONE_KEY                 "NPC-062"

/*
 *  constructor
 */
Stage032Scene::Stage032Scene() {}

/*
 *  destructor
 */
Stage032Scene::~Stage032Scene(){}

/*
 *  生成场景
 */
Stage032Scene* Stage032Scene::createWithStageData(const StageData* pStageData)
{
    Stage032Scene* pStage032Scene = new Stage032Scene();
    if (pStage032Scene && pStage032Scene->initWithStageData(pStageData))
    {
        pStage032Scene->autorelease();
        return pStage032Scene;
    } else {
        CC_SAFE_DELETE(pStage032Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage032Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round032_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round032_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round032_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round032_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage032Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage032Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage032Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
  
    return roundNode;
}
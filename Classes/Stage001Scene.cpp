//
//  Stage001Scene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//

#include "Stage001Scene.h"
#include "Player.h"
#include "Round001_01.h"
#include "Round001_03.h"
#include "Round001_05.h"
#include "Round001_06.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-024"
#define NPC_TWO_KEY                 "NPC-022"

Stage001Scene::Stage001Scene() {}

Stage001Scene::~Stage001Scene() {}

Stage001Scene* Stage001Scene::createWithStageData(const StageData* pStageData)
{
    Stage001Scene* pStage001Scene = new Stage001Scene();
    if (pStage001Scene && pStage001Scene->initWithStageData(pStageData))
    {
        pStage001Scene->autorelease();
        return pStage001Scene;
    } else {
        CC_SAFE_DELETE(pStage001Scene);
        return nullptr;
    }
}

RoundBase* Stage001Scene::loadRound()
{
    // 获取当前回合滚动数据
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round001_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round001_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_POS, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round001_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round001_06::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage001Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage001Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage001Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}


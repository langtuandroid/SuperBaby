//
//  Stage031Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#include "Stage031Scene.h"
#include "Player.h"
#include "Round031_01.h"
#include "Round031_02.h"
#include "Round031_03.h"
#include "Round031_04.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 220.0f)
#define NPC_ONE_KEY                 "NPC-063"

/*
 *  constructor
 */
Stage031Scene::Stage031Scene() {}

/*
 *  constructor
 */
Stage031Scene::~Stage031Scene(){}

/*
 *  生成场景
 */
Stage031Scene* Stage031Scene::createWithStageData(const StageData* pStageData)
{
    Stage031Scene* pStage031Scene = new Stage031Scene();
    if (pStage031Scene && pStage031Scene->initWithStageData(pStageData))
    {
        pStage031Scene->autorelease();
        return pStage031Scene;
    } else {
        CC_SAFE_DELETE(pStage031Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage031Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round031_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round031_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round031_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round031_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage031Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage031Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage031Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
//
//  Stage042Scene.cpp
//  superBaby
//
//  Created by Administrator on 2015/07/30.
//
//

#include "Stage042Scene.h"
#include "Player.h"
#include "Round042_01.h"
#include "Round042_02.h"
#include "Round042_03.h"
#include "Round042_04.h"
#include "Round042_05.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-012"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)
#define NPC_TWO_KEY                     "NPC-014"
#define NPC_TWO_POS                     Vec2(1015.0f, 317.0f)

/*
 *  constructor
 */
Stage042Scene::Stage042Scene() {}

/*
 *  destructor
 */
Stage042Scene::~Stage042Scene(){}

/*
 *  生成场景
 */
Stage042Scene* Stage042Scene::createWithStageData(const StageData* pStageData)
{
    Stage042Scene* pStage042Scene = new Stage042Scene();
    if (pStage042Scene && pStage042Scene->initWithStageData(pStageData))
    {
        pStage042Scene->autorelease();
        return pStage042Scene;
    } else {
        CC_SAFE_DELETE(pStage042Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage042Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round042_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round042_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round042_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round042_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (6 == m_currRoundIndex) {
        roundNode = Round042_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage042Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage042Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage042Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
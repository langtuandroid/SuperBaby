//
//  Stage037Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#include "Stage037Scene.h"
#include "Player.h"
#include "Round037_01.h"
#include "Round037_02.h"
#include "Round037_03.h"
#include "Round037_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 220.0f)
#define NPC_ONE_KEY                 "NPC-027"
#define NPC_TWO_KEY                 "NPC-049"

/*
 *  constructor
 */
Stage037Scene::Stage037Scene() {}

/*
 *  destructor
 */
Stage037Scene::~Stage037Scene(){}

/*
 *  生成场景
 */
Stage037Scene* Stage037Scene::createWithStageData(const StageData* pStageData)
{
    Stage037Scene* pStage037Scene = new Stage037Scene();
    if (pStage037Scene && pStage037Scene->initWithStageData(pStageData))
    {
        pStage037Scene->autorelease();
        return pStage037Scene;
    } else {
        CC_SAFE_DELETE(pStage037Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage037Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round037_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round037_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round037_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round037_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage037Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage037Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage037Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}

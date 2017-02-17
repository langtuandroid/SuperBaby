//
//  Stage016Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#include "Stage016Scene.h"
#include "Player.h"
#include "Round016_01.h"
#include "Round016_02.h"
#include "Round016_03.h"
#include "Round016_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(980.0f, 850.0f)
#define NPC_ONE_KEY                 "NPC-071"

/*
 *  constructor
 */
Stage016Scene::Stage016Scene() {}

/*
 *  constructor
 */
Stage016Scene::~Stage016Scene(){}

/*
 *  生成场景
 */
Stage016Scene* Stage016Scene::createWithStageData(const StageData* pStageData)
{
    Stage016Scene* pStage016Scene = new Stage016Scene();
    if (pStage016Scene && pStage016Scene->initWithStageData(pStageData))
    {
        pStage016Scene->autorelease();
        return pStage016Scene;
    } else {
        CC_SAFE_DELETE(pStage016Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage016Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round016_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round016_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round016_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round016_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage016Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage016Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage016Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    
    return roundNode;
}
//
//  Stage027Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#include "Stage027Scene.h"
#include "Player.h"
#include "Round027_01.h"
#include "Round027_02.h"
#include "Round027_03.h"
#include "Round027_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-086"

/*
 *  constructor
 */
Stage027Scene::Stage027Scene() {}

/*
 *  destructor
 */
Stage027Scene::~Stage027Scene(){}

/*
 *  生成场景
 */
Stage027Scene* Stage027Scene::createWithStageData(const StageData* pStageData)
{
    Stage027Scene* pStage027Scene = new Stage027Scene();
    if (pStage027Scene && pStage027Scene->initWithStageData(pStageData))
    {
        pStage027Scene->autorelease();
        return pStage027Scene;
    } else {
        CC_SAFE_DELETE(pStage027Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage027Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round027_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round027_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round027_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round027_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage027Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage027Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage027Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
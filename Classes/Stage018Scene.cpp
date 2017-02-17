//
//  Stage018Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Stage018Scene.h"
#include "Player.h"
#include "Round018_01.h"
#include "Round018_02.h"
#include "Round018_03.h"
#include "Round018_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 250.0f)
#define NPC_ONE_KEY                 "NPC-059"

/*
 *  constructor
 */
Stage018Scene::Stage018Scene() {}

/*
 *  constructor
 */
Stage018Scene::~Stage018Scene(){}

/*
 *  生成场景
 */
Stage018Scene* Stage018Scene::createWithStageData(const StageData* pStageData)
{
    Stage018Scene* pStage018Scene = new Stage018Scene();
    if (pStage018Scene && pStage018Scene->initWithStageData(pStageData))
    {
        pStage018Scene->autorelease();
        return pStage018Scene;
    } else {
        CC_SAFE_DELETE(pStage018Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage018Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round018_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round018_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round018_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round018_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage018Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage018Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage018Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
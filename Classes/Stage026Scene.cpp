//
//  Stage026Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#include "Stage026Scene.h"
#include "Player.h"
#include "Round026_01.h"
#include "Round026_02.h"
#include "Round026_03.h"
#include "Round026_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-040"

/*
 *  constructor
 */
Stage026Scene::Stage026Scene() {}

/*
 *  constructor
 */
Stage026Scene::~Stage026Scene(){}

/*
 *  生成场景
 */
Stage026Scene* Stage026Scene::createWithStageData(const StageData* pStageData)
{
    Stage026Scene* pStage026Scene = new Stage026Scene();
    if (pStage026Scene && pStage026Scene->initWithStageData(pStageData))
    {
        pStage026Scene->autorelease();
        return pStage026Scene;
    } else {
        CC_SAFE_DELETE(pStage026Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage026Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round026_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round026_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round026_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round026_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage026Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage026Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage026Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
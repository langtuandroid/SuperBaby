//
//  Stage022Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#include "Stage022Scene.h"
#include "Player.h"
#include "Round022_01.h"
#include "Round022_02.h"
#include "Round022_03.h"
#include "Round022_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(980.0f, 850.0f)
#define NPC_ONE_KEY                 "NPC-091"

/*
 *  constructor
 */
Stage022Scene::Stage022Scene() {}

/*
 *  constructor
 */
Stage022Scene::~Stage022Scene(){}

/*
 *  生成场景
 */
Stage022Scene* Stage022Scene::createWithStageData(const StageData* pStageData)
{
    Stage022Scene* pStage022Scene = new Stage022Scene();
    if (pStage022Scene && pStage022Scene->initWithStageData(pStageData))
    {
        pStage022Scene->autorelease();
        return pStage022Scene;
    } else {
        CC_SAFE_DELETE(pStage022Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage022Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round022_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round022_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round022_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round022_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage022Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage022Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage022Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    

    return roundNode;
}
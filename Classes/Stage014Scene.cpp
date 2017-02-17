//
//  Stage014Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/12.
//
//

#include "Stage014Scene.h"
#include "Player.h"
#include "Round014_01.h"
#include "Round014_02.h"
#include "Round014_03.h"
#include "Round014_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 300.0f)
#define NPC_ONE_KEY                 "NPC-055"

/*
 *  constructor
 */
Stage014Scene::Stage014Scene() {}

/*
 *  constructor
 */
Stage014Scene::~Stage014Scene(){}

/*
 *  生成场景
 */
Stage014Scene* Stage014Scene::createWithStageData(const StageData* pStageData)
{
    Stage014Scene* pStage014Scene = new Stage014Scene();
    if (pStage014Scene && pStage014Scene->initWithStageData(pStageData))
    {
        pStage014Scene->autorelease();
        return pStage014Scene;
    } else {
        CC_SAFE_DELETE(pStage014Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage014Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round014_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round014_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round014_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round014_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage014Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage014Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage014Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    

    return roundNode;
}
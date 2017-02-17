//
//  Stage033Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#include "Stage033Scene.h"
#include "Player.h"
#include "Round033_01.h"
#include "Round033_02.h"
#include "Round033_03.h"
#include "Round033_04.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-047"

/*
 *  constructor
 */
Stage033Scene::Stage033Scene() {}

/*
 *  constructor
 */
Stage033Scene::~Stage033Scene(){}

/*
 *  生成场景
 */
Stage033Scene* Stage033Scene::createWithStageData(const StageData* pStageData)
{
    Stage033Scene* pStage033Scene = new Stage033Scene();
    if (pStage033Scene && pStage033Scene->initWithStageData(pStageData))
    {
        pStage033Scene->autorelease();
        return pStage033Scene;
    } else {
        CC_SAFE_DELETE(pStage033Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage033Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round033_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round033_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round033_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round033_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage033Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage033Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage033Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
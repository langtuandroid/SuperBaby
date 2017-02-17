//
//  Stage048Scene.cpp
//  superBaby
//
//  Created by Administrator on 2015/08/06.
//
//

#include "Stage048Scene.h"
#include "Player.h"
#include "Round048_01.h"
#include "Round048_02.h"
#include "Round048_03.h"
#include "Round048_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-051"
#define NPC_ONE_POS                     Vec2(1000.0f, 220.0f)
#define NPC_TWO_KEY                     "NPC-028"
#define NPC_TWO_POS                     Vec2(1000.0f, 220.0f)

/*
 *  constructor
 */
Stage048Scene::Stage048Scene() {}

/*
 *  destructor
 */
Stage048Scene::~Stage048Scene(){}

/*
 *  生成场景
 */
Stage048Scene* Stage048Scene::createWithStageData(const StageData* pStageData)
{
    Stage048Scene* pStage048Scene = new Stage048Scene();
    if (pStage048Scene && pStage048Scene->initWithStageData(pStageData))
    {
        pStage048Scene->autorelease();
        return pStage048Scene;
    } else {
        CC_SAFE_DELETE(pStage048Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage048Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round048_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round048_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round048_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round048_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    
    roundNode->setCompleteNotifyCB(std::bind(&Stage048Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage048Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage048Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
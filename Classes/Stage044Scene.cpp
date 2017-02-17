//
//  Stage044Scene.cpp
//  superBaby
//
//  Created by Administrator on 2015/08/03.
//
//

#include "Stage044Scene.h"
#include "Player.h"
#include "Round044_01.h"
#include "Round044_02.h"
#include "Round044_03.h"
#include "Round044_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-041"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)
#define NPC_TWO_KEY                     "NPC-011"
#define NPC_TWO_POS                     Vec2(1000.0f, 270.0f)

/*
 *  constructor
 */
Stage044Scene::Stage044Scene() {}

/*
 *  destructor
 */
Stage044Scene::~Stage044Scene(){}

/*
 *  生成场景
 */
Stage044Scene* Stage044Scene::createWithStageData(const StageData* pStageData)
{
    Stage044Scene* pStage044Scene = new Stage044Scene();
    if (pStage044Scene && pStage044Scene->initWithStageData(pStageData))
    {
        pStage044Scene->autorelease();
        return pStage044Scene;
    } else {
        CC_SAFE_DELETE(pStage044Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage044Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round044_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round044_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round044_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round044_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage044Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage044Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage044Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
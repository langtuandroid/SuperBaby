//
//  Stage047Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#include "Stage047Scene.h"
#include "Player.h"
#include "Round047_01.h"
#include "Round047_02.h"
#include "Round047_03.h"
#include "Round047_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-043"
#define NPC_ONE_POS                     Vec2(1000.0f, 220.0f)

/*
 *  constructor
 */
Stage047Scene::Stage047Scene() {}

/*
 *  destructor
 */
Stage047Scene::~Stage047Scene(){}

/*
 *  生成场景
 */
Stage047Scene* Stage047Scene::createWithStageData(const StageData* pStageData)
{
    Stage047Scene* pStage047Scene = new Stage047Scene();
    if (pStage047Scene && pStage047Scene->initWithStageData(pStageData))
    {
        pStage047Scene->autorelease();
        return pStage047Scene;
    } else {
        CC_SAFE_DELETE(pStage047Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage047Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round047_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round047_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round047_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round047_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    
    roundNode->setCompleteNotifyCB(std::bind(&Stage047Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage047Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage047Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
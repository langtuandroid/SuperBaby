//
//  Stage043Scene.cpp
//  superBaby
//
//  Created by Administrator on 2015/07/31.
//
//

#include "Stage043Scene.h"
#include "Player.h"
#include "Round043_01.h"
#include "Round043_02.h"
#include "Round043_03.h"
#include "Round043_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-042"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)
#define NPC_TWO_KEY                     "NPC-060"
#define NPC_TWO_POS                     Vec2(1015.0f, 317.0f)

/*
 *  constructor
 */
Stage043Scene::Stage043Scene() {}

/*
 *  destructor
 */
Stage043Scene::~Stage043Scene(){}

/*
 *  生成场景
 */
Stage043Scene* Stage043Scene::createWithStageData(const StageData* pStageData)
{
    Stage043Scene* pStage043Scene = new Stage043Scene();
    if (pStage043Scene && pStage043Scene->initWithStageData(pStageData))
    {
        pStage043Scene->autorelease();
        return pStage043Scene;
    } else {
        CC_SAFE_DELETE(pStage043Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage043Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round043_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round043_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round043_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round043_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage043Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage043Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage043Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
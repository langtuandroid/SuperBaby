//
//  Stage038Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#include "Stage038Scene.h"
#include "Player.h"
#include "Round038_01.h"
#include "Round038_02.h"
#include "Round038_03.h"
#include "Round038_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 220.0f)
#define NPC_ONE_KEY                 "NPC-008"

/*
 *  constructor
 */
Stage038Scene::Stage038Scene() {}

/*
 *  destructor
 */
Stage038Scene::~Stage038Scene(){}

/*
 *  生成场景
 */
Stage038Scene* Stage038Scene::createWithStageData(const StageData* pStageData)
{
    Stage038Scene* pStage038Scene = new Stage038Scene();
    if (pStage038Scene && pStage038Scene->initWithStageData(pStageData))
    {
        pStage038Scene->autorelease();
        return pStage038Scene;
    } else {
        CC_SAFE_DELETE(pStage038Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage038Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round038_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round038_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round038_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round038_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage038Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage038Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage038Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
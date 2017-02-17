//
//  Stage035Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#include "Stage035Scene.h"
#include "Player.h"
#include "Round035_01.h"
#include "Round035_02.h"
#include "Round035_03.h"
#include "Round035_04.h"
#include "Round035_05.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-074"

/*
 *  constructor
 */
Stage035Scene::Stage035Scene() {}

/*
 *  destructor
 */
Stage035Scene::~Stage035Scene(){}

/*
 *  生成场景
 */
Stage035Scene* Stage035Scene::createWithStageData(const StageData* pStageData)
{
    Stage035Scene* pStage035Scene = new Stage035Scene();
    if (pStage035Scene && pStage035Scene->initWithStageData(pStageData))
    {
        pStage035Scene->autorelease();
        return pStage035Scene;
    } else {
        CC_SAFE_DELETE(pStage035Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage035Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round035_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round035_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round035_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round035_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round035_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage035Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage035Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage035Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
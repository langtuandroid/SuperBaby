//
//  Stage034Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#include "Stage034Scene.h"
#include "Player.h"
#include "Round034_01.h"
#include "Round034_02.h"
#include "Round034_03.h"
#include "Round034_04.h"
#include "Round034_05.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-088"

/*
 *  constructor
 */
Stage034Scene::Stage034Scene() {}

/*
 *  destructor
 */
Stage034Scene::~Stage034Scene(){}

/*
 *  生成场景
 */
Stage034Scene* Stage034Scene::createWithStageData(const StageData* pStageData)
{
    Stage034Scene* pStage034Scene = new Stage034Scene();
    if (pStage034Scene && pStage034Scene->initWithStageData(pStageData))
    {
        pStage034Scene->autorelease();
        return pStage034Scene;
    } else {
        CC_SAFE_DELETE(pStage034Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage034Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round034_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round034_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round034_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round034_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round034_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage034Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage034Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage034Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
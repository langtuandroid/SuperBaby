//
//  Stage036Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#include "Stage036Scene.h"
#include "Player.h"
#include "Round036_01.h"
#include "Round036_02.h"
#include "Round036_03.h"
#include "Round036_04.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-064"

/*
 *  constructor
 */
Stage036Scene::Stage036Scene() {}

/*
 *  destructor
 */
Stage036Scene::~Stage036Scene(){}

/*
 *  生成场景
 */
Stage036Scene* Stage036Scene::createWithStageData(const StageData* pStageData)
{
    Stage036Scene* pStage036Scene = new Stage036Scene();
    if (pStage036Scene && pStage036Scene->initWithStageData(pStageData))
    {
        pStage036Scene->autorelease();
        return pStage036Scene;
    } else {
        CC_SAFE_DELETE(pStage036Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage036Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
 
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round036_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round036_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round036_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round036_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage036Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage036Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage036Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
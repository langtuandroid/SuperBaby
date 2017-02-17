//
//  Stage003Scene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//

#include "Stage003Scene.h"
#include "Player.h"
#include "Round003_01.h"
#include "Round003_02.h"
#include "Round003_04.h"
#include "Round003_05.h"
#include "Round003_06.h"
#include "RoundNpc.h"
#include "StageData.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_KEY                     "NPC-017"

/*
 *  constructor
 */
Stage003Scene::Stage003Scene() {}

/*
 *  constructor
 */
Stage003Scene::~Stage003Scene() {}

/*
 *  生成场景
 */
Stage003Scene* Stage003Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage003Scene();
    if (pStageScene && pStageScene->initWithStageData(pStageData))
    {
        pStageScene->autorelease();
        return pStageScene;
    } else {
        CC_SAFE_DELETE(pStageScene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage003Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round003_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round003_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round003_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round003_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round003_06::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage003Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage003Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage003Scene::answerErrorCB, this));
    // 添加回合结点
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}



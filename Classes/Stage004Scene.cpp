//
//  Stage004Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#include "Stage004Scene.h"
#include "Round004_01.h"
#include "Round004_02.h"
#include "Round004_04.h"
#include "Round004_05.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_KEY                     "NPC-009"

/*
 *  constructor
 */
Stage004Scene::Stage004Scene() {}

/*
 *  constructor
 */
Stage004Scene::~Stage004Scene() {}

/*
 *  生成场景
 */
Stage004Scene* Stage004Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage004Scene();
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
RoundBase* Stage004Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round004_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round004_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round004_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round004_05::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage004Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage004Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage004Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
//
//  Stage005Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/1.
//
//

#include "Stage005Scene.h"
#include "StageData.h"
#include "Round005_01.h"
#include "Round005_03.h"
#include "Round005_04.h"
#include "Round005_06.h"
#include "Actor.h"
#include "RoundNpc.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                     "NPC-029"
#define NPC_TWO_KEY                     "NPC-013"

/*
 *  constructor
 */
Stage005Scene::Stage005Scene() {}

/*
 *  constructor
 */
Stage005Scene::~Stage005Scene() {}

/*
 *  生成场景
 */
Stage005Scene* Stage005Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage005Scene();
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
RoundBase* Stage005Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round005_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round005_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round005_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round005_06::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage005Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage005Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage005Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}

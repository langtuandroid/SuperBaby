//
//  Stage011Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/9.
//
//

#include "Stage011Scene.h"
#include "StageData.h"
#include "Round011_01.h"
#include "Round011_02.h"
#include "Round011_03.h"
#include "Round011_04.h"
#include "Actor.h"
#include "RoundNpc.h"

#define NPC_ONE_KEY                     "NPC-033"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)
#define NPC_TWO_KEY                     "NPC-067"
#define NPC_TWO_POS                     Vec2(1000.0f, 320.0f)

/*
 *  constructor
 */
Stage011Scene::Stage011Scene() {}

/*
 *  constructor
 */
Stage011Scene::~Stage011Scene() {}

/*
 *  生成场景
 */
Stage011Scene* Stage011Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage011Scene();
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
RoundBase* Stage011Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round011_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round011_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round011_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round011_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage011Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage011Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage011Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    
    return roundNode;
}

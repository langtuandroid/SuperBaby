//
//  Stage040Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#include "Stage040Scene.h"
#include "Player.h"
#include "Round040_01.h"
#include "Round040_02.h"
#include "Round040_03.h"
#include "Round040_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-036"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)
#define NPC_TWO_KEY                     "NPC-037"
#define NPC_TWO_POS                     Vec2(1015.0f, 324.0f)

/*
 *  constructor
 */
Stage040Scene::Stage040Scene() {}

/*
 *  destructor
 */
Stage040Scene::~Stage040Scene(){}

/*
 *  生成场景
 */
Stage040Scene* Stage040Scene::createWithStageData(const StageData* pStageData)
{
    Stage040Scene* pStage040Scene = new Stage040Scene();
    if (pStage040Scene && pStage040Scene->initWithStageData(pStageData))
    {
        pStage040Scene->autorelease();
        return pStage040Scene;
    } else {
        CC_SAFE_DELETE(pStage040Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage040Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round040_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round040_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round040_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex)
    {
        roundNode = Round040_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage040Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage040Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage040Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
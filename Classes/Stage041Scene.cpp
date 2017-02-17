//
//  Stage041Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#include "Stage041Scene.h"
#include "Player.h"
#include "Round041_01.h"
#include "Round041_02.h"
#include "Round041_03.h"
#include "Round041_04.h"
#include "Round041_05.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-039"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)
#define NPC_TWO_KEY                     "NPC-061"
#define NPC_TWO_POS                     Vec2(1015.0f, 317.0f)

/*
 *  constructor
 */
Stage041Scene::Stage041Scene() {}

/*
 *  destructor
 */
Stage041Scene::~Stage041Scene(){}

/*
 *  生成场景
 */
Stage041Scene* Stage041Scene::createWithStageData(const StageData* pStageData)
{
    Stage041Scene* pStage041Scene = new Stage041Scene();
    if (pStage041Scene && pStage041Scene->initWithStageData(pStageData))
    {
        pStage041Scene->autorelease();
        return pStage041Scene;
    } else {
        CC_SAFE_DELETE(pStage041Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage041Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round041_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round041_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round041_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_TWO_KEY, NPC_TWO_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex)
    {
        roundNode = Round041_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }  else if (6 == m_currRoundIndex)
    {
        roundNode = Round041_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage041Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage041Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage041Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
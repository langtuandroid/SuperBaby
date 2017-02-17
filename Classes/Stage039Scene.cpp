//
//  Stage039Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#include "Stage039Scene.h"
#include "Player.h"
#include "Round039_01.h"
#include "Round039_02.h"
#include "Round039_03.h"
#include "Round039_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 220.0f)
#define NPC_ONE_KEY                 "NPC-016"

/*
 *  constructor
 */
Stage039Scene::Stage039Scene() {}

/*
 *  destructor
 */
Stage039Scene::~Stage039Scene(){}

/*
 *  生成场景
 */
Stage039Scene* Stage039Scene::createWithStageData(const StageData* pStageData)
{
    Stage039Scene* pStage039Scene = new Stage039Scene();
    if (pStage039Scene && pStage039Scene->initWithStageData(pStageData))
    {
        pStage039Scene->autorelease();
        return pStage039Scene;
    } else {
        CC_SAFE_DELETE(pStage039Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage039Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round039_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round039_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round039_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round039_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage039Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage039Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage039Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
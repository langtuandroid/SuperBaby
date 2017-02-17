//
//  Stage030Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Stage030Scene.h"
#include "Player.h"
#include "Round030_01.h"
#include "Round030_02.h"
#include "Round030_03.h"
#include "Round030_04.h"
#include "Round030_05.h"
#include "RoundNpc.h"
#include "StageData.h"
#include "Actor.h"

#define NPC_POS                     Vec2(1000.0f, 320.0f)
#define NPC_ONE_KEY                 "NPC-076"

/*
 *  constructor
 */
Stage030Scene::Stage030Scene() {}

/*
 *  destructor
 */
Stage030Scene::~Stage030Scene(){}

/*
 *  生成场景
 */
Stage030Scene* Stage030Scene::createWithStageData(const StageData* pStageData)
{
    Stage030Scene* pStage030Scene = new Stage030Scene();
    if (pStage030Scene && pStage030Scene->initWithStageData(pStageData))
    {
        pStage030Scene->autorelease();
        return pStage030Scene;
    } else {
        CC_SAFE_DELETE(pStage030Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage030Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round030_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round030_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round030_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round030_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (5 == m_currRoundIndex) {
        roundNode = Round030_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage030Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage030Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage030Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
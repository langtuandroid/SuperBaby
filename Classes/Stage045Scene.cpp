//
//  Stage045Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#include "Stage045Scene.h"
#include "Player.h"
#include "Round045_01.h"
#include "Round045_02.h"
#include "Round045_03.h"
#include "Round045_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage045Scene::Stage045Scene() {}

/*
 *  constructor
 */
Stage045Scene::~Stage045Scene(){}

/*
 *  生成场景
 */
Stage045Scene* Stage045Scene::createWithStageData(const StageData* pStageData)
{
    Stage045Scene* pStage045Scene = new Stage045Scene();
    if (pStage045Scene && pStage045Scene->initWithStageData(pStageData))
    {
        pStage045Scene->autorelease();
        return pStage045Scene;
    } else {
        CC_SAFE_DELETE(pStage045Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage045Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round045_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round045_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round045_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round045_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    
    roundNode->setCompleteNotifyCB(std::bind(&Stage045Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage045Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage045Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
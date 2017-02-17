//
//  Stage015Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#include "Stage015Scene.h"
#include "Player.h"
#include "Round015_01.h"
#include "Round015_02.h"
#include "Round015_03.h"
#include "Round015_04.h"
#include "Round015_05.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage015Scene::Stage015Scene() {}

/*
 *  constructor
 */
Stage015Scene::~Stage015Scene(){}

/*
 *  生成场景
 */
Stage015Scene* Stage015Scene::createWithStageData(const StageData* pStageData)
{
    Stage015Scene* pStage015Scene = new Stage015Scene();
    if (pStage015Scene && pStage015Scene->initWithStageData(pStageData))
    {
        pStage015Scene->autorelease();
        return pStage015Scene;
    } else {
        CC_SAFE_DELETE(pStage015Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage015Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round015_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round015_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round015_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round015_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round015_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage015Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage015Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage015Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    
    return roundNode;
}
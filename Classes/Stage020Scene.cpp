//
//  Stage020Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/20.
//
//

#include "Stage020Scene.h"
#include "Player.h"
#include "Round020_01.h"
#include "Round020_02.h"
#include "Round020_03.h"
#include "Round020_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage020Scene::Stage020Scene() {}

/*
 *  constructor
 */
Stage020Scene::~Stage020Scene(){}

/*
 *  生成场景
 */
Stage020Scene* Stage020Scene::createWithStageData(const StageData* pStageData)
{
    Stage020Scene* pStage020Scene = new Stage020Scene();
    if (pStage020Scene && pStage020Scene->initWithStageData(pStageData))
    {
        pStage020Scene->autorelease();
        return pStage020Scene;
    } else {
        CC_SAFE_DELETE(pStage020Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage020Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round020_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round020_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round020_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round020_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage020Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage020Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage020Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    
    return roundNode;
}
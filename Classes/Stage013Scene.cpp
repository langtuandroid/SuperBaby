//
//  Stage013Scene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//

#include "Stage013Scene.h"
#include "Player.h"
#include "Round013_01.h"
#include "Round013_02.h"
#include "Round013_03.h"
#include "Round013_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage013Scene::Stage013Scene() {}

/*
 *  constructor
 */
Stage013Scene::~Stage013Scene(){}

/*
 *  生成场景
 */
Stage013Scene* Stage013Scene::createWithStageData(const StageData* pStageData)
{
    Stage013Scene* pStage001Scene = new Stage013Scene();
    if (pStage001Scene && pStage001Scene->initWithStageData(pStageData))
    {
        pStage001Scene->autorelease();
        return pStage001Scene;
    } else {
        CC_SAFE_DELETE(pStage001Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage013Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round013_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round013_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round013_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round013_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage013Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage013Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage013Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    
    return roundNode;
}

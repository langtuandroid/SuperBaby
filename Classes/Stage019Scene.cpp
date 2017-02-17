//
//  Stage019Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#include "Stage019Scene.h"
#include "Player.h"
#include "Round019_01.h"
#include "Round019_02.h"
#include "Round019_03.h"
#include "Round019_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage019Scene::Stage019Scene() {}

/*
 *  constructor
 */
Stage019Scene::~Stage019Scene(){}

/*
 *  生成场景
 */
Stage019Scene* Stage019Scene::createWithStageData(const StageData* pStageData)
{
    Stage019Scene* pStage019Scene = new Stage019Scene();
    if (pStage019Scene && pStage019Scene->initWithStageData(pStageData))
    {
        pStage019Scene->autorelease();
        return pStage019Scene;
    } else {
        CC_SAFE_DELETE(pStage019Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage019Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round019_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round019_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round019_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round019_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage019Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage019Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage019Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    
    return roundNode;
}
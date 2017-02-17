//
//  Stage029Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Stage029Scene.h"
#include "Player.h"
#include "Round029_01.h"
#include "Round029_02.h"
#include "Round029_03.h"
#include "Round029_04.h"
#include "StageData.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage029Scene::Stage029Scene() {}

/*
 *  constructor
 */
Stage029Scene::~Stage029Scene(){}

/*
 *  生成场景
 */
Stage029Scene* Stage029Scene::createWithStageData(const StageData* pStageData)
{
    Stage029Scene* pStage029Scene = new Stage029Scene();
    if (pStage029Scene && pStage029Scene->initWithStageData(pStageData))
    {
        pStage029Scene->autorelease();
        return pStage029Scene;
    } else {
        CC_SAFE_DELETE(pStage029Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage029Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round029_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round029_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round029_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round029_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage029Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage029Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage029Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
//
//  Stage028Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Stage028Scene.h"
#include "Player.h"
#include "Round028_01.h"
#include "Round028_02.h"
#include "Round028_03.h"
#include "Round028_04.h"
#include "Round028_05.h"
#include "StageData.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage028Scene::Stage028Scene() {}

/*
 *  constructor
 */
Stage028Scene::~Stage028Scene(){}

/*
 *  生成场景
 */
Stage028Scene* Stage028Scene::createWithStageData(const StageData* pStageData)
{
    Stage028Scene* pStage028Scene = new Stage028Scene();
    if (pStage028Scene && pStage028Scene->initWithStageData(pStageData))
    {
        pStage028Scene->autorelease();
        return pStage028Scene;
    } else {
        CC_SAFE_DELETE(pStage028Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage028Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round028_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round028_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round028_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round028_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round028_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage028Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage028Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage028Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
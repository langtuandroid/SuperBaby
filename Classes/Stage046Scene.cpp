//
//  Stage046Scene.cpp
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#include "Stage046Scene.h"
#include "Player.h"
#include "Round046_01.h"
#include "Round046_02.h"
#include "Round046_03.h"
#include "Round046_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage046Scene::Stage046Scene() {}

/*
 *  constructor
 */
Stage046Scene::~Stage046Scene(){}

/*
 *  生成场景
 */
Stage046Scene* Stage046Scene::createWithStageData(const StageData* pStageData)
{
    Stage046Scene* pStage046Scene = new Stage046Scene();
    if (pStage046Scene && pStage046Scene->initWithStageData(pStageData))
    {
        pStage046Scene->autorelease();
        return pStage046Scene;
    } else {
        CC_SAFE_DELETE(pStage046Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage046Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round046_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round046_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round046_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round046_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    
    roundNode->setCompleteNotifyCB(std::bind(&Stage046Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage046Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage046Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
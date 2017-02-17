//
//  Stage008Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/7.
//
//

#include "Stage008Scene.h"
#include "StageData.h"
#include "Round008_01.h"
#include "Round008_02.h"
#include "Round008_03.h"
#include "Round008_04.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage008Scene::Stage008Scene() {}

/*
 *  constructor
 */
Stage008Scene::~Stage008Scene() {}

/*
 *  生成场景
 */
Stage008Scene* Stage008Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage008Scene();
    if (pStageScene && pStageScene->initWithStageData(pStageData))
    {
        pStageScene->autorelease();
        return pStageScene;
    } else {
        CC_SAFE_DELETE(pStageScene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage008Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round008_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round008_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round008_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round008_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage008Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage008Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage008Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    
     return roundNode;
}

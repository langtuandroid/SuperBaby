//
//  Stage012Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Stage012Scene.h"
#include "StageData.h"
#include "Round012_01.h"
#include "Round012_02.h"
#include "Round012_03.h"
#include "Round012_04.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage012Scene::Stage012Scene() {}

/*
 *  constructor
 */
Stage012Scene::~Stage012Scene() {}

/*
 *  生成场景
 */
Stage012Scene* Stage012Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage012Scene();
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
RoundBase* Stage012Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round012_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round012_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round012_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round012_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage012Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage012Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage012Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    
    return roundNode;
}

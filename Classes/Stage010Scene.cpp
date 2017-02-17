//
//  Stage010Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/8.
//
//

#include "Stage010Scene.h"
#include "StageData.h"
#include "Round010_01.h"
#include "Round010_02.h"
#include "Round010_03.h"
#include "Round010_04.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage010Scene::Stage010Scene() {}

/*
 *  constructor
 */
Stage010Scene::~Stage010Scene() {}

/*
 *  生成场景
 */
Stage010Scene* Stage010Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage010Scene();
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
RoundBase* Stage010Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round010_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round010_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round010_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round010_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage010Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage010Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage010Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
   
    
    return roundNode;
}

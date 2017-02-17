//
//  Stage002Scene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//

#include "Stage002Scene.h"
#include "Player.h"
#include "Round002_01.h"
#include "Round002_02.h"
#include "Round002_03.h"
#include "Round002_04.h"
#include "Round002_05.h"
#include "StageData.h"

/*
 *  constructor
 */
Stage002Scene::Stage002Scene() {}

/*
 *  constructor
 */
Stage002Scene::~Stage002Scene() {}

/*
 *  生成场景
 */
Stage002Scene* Stage002Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage002Scene();
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
RoundBase* Stage002Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round002_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round002_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round002_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round002_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round002_05::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage002Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage002Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage002Scene::answerErrorCB, this));
    // 添加回合结点
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}



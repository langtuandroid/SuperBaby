//
//  Stage009Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Stage009Scene.h"
#include "StageData.h"
#include "Round009_01.h"
#include "Round009_02.h"
#include "Round009_03.h"
#include "Round009_04.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage009Scene::Stage009Scene() {}

/*
 *  constructor
 */
Stage009Scene::~Stage009Scene() {}

/*
 *  生成场景
 */
Stage009Scene* Stage009Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage009Scene();
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
RoundBase* Stage009Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);

    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round009_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round009_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round009_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round009_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage009Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage009Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage009Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}

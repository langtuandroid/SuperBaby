//
//  Stage023Scene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//

#include "Stage023Scene.h"
#include "Player.h"
#include "Round023_01.h"
#include "Round023_02.h"
#include "Round023_03.h"
#include "Round023_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

/*
 *  constructor
 */
Stage023Scene::Stage023Scene() {}

/*
 *  constructor
 */
Stage023Scene::~Stage023Scene(){}

/*
 *  生成场景
 */
Stage023Scene* Stage023Scene::createWithStageData(const StageData* pStageData)
{
    Stage023Scene* pStageScene = new Stage023Scene();
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
RoundBase* Stage023Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round023_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round023_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round023_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round023_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage023Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage023Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage023Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    
    return roundNode;
}

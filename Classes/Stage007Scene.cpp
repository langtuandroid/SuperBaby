//
//  Stage007Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/3.
//
//

#include "Stage007Scene.h"
#include "StageData.h"
#include "Round007_01.h"
#include "Round007_02.h"
#include "Round007_03.h"
#include "Round007_04.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_ONE_KEY                     "NPC-032"
#define NPC_ONE_POS                     Vec2(1000.0f, 320.0f)

/*
 *  constructor
 */
Stage007Scene::Stage007Scene() {}

/*
 *  constructor
 */
Stage007Scene::~Stage007Scene() {}

/*
 *  生成场景
 */
Stage007Scene* Stage007Scene::createWithStageData(const StageData* pStageData)
{
    auto pStageScene = new Stage007Scene();
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
RoundBase* Stage007Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round007_01::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round007_02::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_ONE_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round007_03::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round007_04::createWithRoundIndex(m_currRoundIndex,this, m_roundNodePos);
    } 
    roundNode->setCompleteNotifyCB(std::bind(&Stage007Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage007Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage007Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}

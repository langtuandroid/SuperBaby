//
//  Stage024Scene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/5.
//
//

#include "Stage024Scene.h"
#include "Player.h"
#include "Round024_01.h"
#include "Round024_02.h"
#include "Round024_03.h"
#include "Round024_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(980.0f, 300.0f)
#define NPC_ONE_KEY                 "NPC-004"

/*
 *  constructor
 */
Stage024Scene::Stage024Scene() {}

/*
 *  destructor
 */
Stage024Scene::~Stage024Scene(){}

/*
 *  生成场景
 */
Stage024Scene* Stage024Scene::createWithStageData(const StageData* pStageData)
{
    Stage024Scene* pStageScene = new Stage024Scene();
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
RoundBase* Stage024Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round024_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = Round024_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (2 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round024_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round024_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage024Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage024Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage024Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}

//
//  Stage021Scene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#include "Stage021Scene.h"
#include "Player.h"
#include "Round021_01.h"
#include "Round021_02.h"
#include "Round021_03.h"
#include "Round021_04.h"
#include "StageData.h"
#include "RoundNpc.h"
#include "Actor.h"

#define NPC_POS                     Vec2(980.0f, 850.0f)
#define NPC_ONE_KEY                 "NPC-020"
#define BRIDGE_PNG_FILE             "XQ-GK09-2-IT01.png"
#define BRIDGE_POS                  Vec2(1120.0f,820.0f)

/*
 *  constructor
 */
Stage021Scene::Stage021Scene() {}

/*
 *  destructor
 */
Stage021Scene::~Stage021Scene(){}

/*
 *  生成场景
 */
Stage021Scene* Stage021Scene::createWithStageData(const StageData* pStageData)
{
    Stage021Scene* pStage021Scene = new Stage021Scene();
    if (pStage021Scene && pStage021Scene->initWithStageData(pStageData))
    {
        pStage021Scene->autorelease();
        return pStage021Scene;
    } else {
        CC_SAFE_DELETE(pStage021Scene);
        return nullptr;
    }
}

/*
 *  加载回合
 */
RoundBase* Stage021Scene::loadRound()
{
    // 获取当前回合索引
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    m_roundNodePos.x += std::abs(rsp.m_scroll);
    
    RoundBase* roundNode = nullptr;
    if (0 == m_currRoundIndex) {
        roundNode = Round021_01::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (1 == m_currRoundIndex) {
        roundNode = RoundNpc::createWithNpc(m_currRoundIndex, NPC_ONE_KEY, NPC_POS, this, m_roundNodePos);
        auto bridgeSp = Sprite::createWithSpriteFrameName(BRIDGE_PNG_FILE);
        ((RoundNpc*)roundNode)->addBlowNode(bridgeSp, BRIDGE_POS);
    } else if (2 == m_currRoundIndex) {
        roundNode = Round021_02::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (3 == m_currRoundIndex) {
        roundNode = Round021_03::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    } else if (4 == m_currRoundIndex) {
        roundNode = Round021_04::createWithRoundIndex(m_currRoundIndex, this, m_roundNodePos);
    }
    roundNode->setCompleteNotifyCB(std::bind(&Stage021Scene::roundCompleteCB, this));
    roundNode->setCelebrateNotifyCB(std::bind(&Stage021Scene::celebrateCB, this, std::placeholders::_1));
    roundNode->setAnswerErrorNotifyCB(std::bind(&Stage021Scene::answerErrorCB, this));
    m_pContainerNode->addChild(roundNode, ROUND_ZORDER);
    
    return roundNode;
}
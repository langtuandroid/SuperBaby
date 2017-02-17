//
//  Round003_01.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round003_01.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"

#define TREE_PNG_FILE               "res/stage002/SL-GK02-4-IT01.png"

#define TREE_TARGET_POSX            756.0f
#define TREE_TARGET_POSY            -200.0f
#define TREE_DOWN_OFFSET            -750.0f
#define TREE_TAG                    101

/*
 *  constructor
 */
Round003_01::Round003_01(){}

/*
 *  destructor
 */
Round003_01::~Round003_01(){}

/*
 *  初始化
 */
Round003_01* Round003_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round003_01();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene,pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

/*
 *  初始化
 */
bool Round003_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendRemovedTexture(TREE_PNG_FILE);
    auto treeSp = Sprite::create(TREE_PNG_FILE);
    treeSp->setPosition(Vec2(TREE_TARGET_POSX, TREE_TARGET_POSY + std::abs(TREE_DOWN_OFFSET)));
    addChild(treeSp, 1, TREE_TAG);
    return true;
}

//
void Round003_01::onEnterRound()
{
    const float duration = 2.5f;
    if (m_pStageScene) {
        m_pStageScene->moveLeadingActor(duration, Vec2(0.0f, TREE_DOWN_OFFSET));
    }
    
    auto treeSp = (Sprite*)this->getChildByTag(TREE_TAG);
    auto treeScrollCompletedCB = CallFunc::create([this] {
        if (m_completeCB) {
            m_completeCB();
        }
    });
    auto treeMoveAction = Sequence::create(MoveBy::create(duration, Vec2(0.0f, TREE_DOWN_OFFSET)),
                                       treeScrollCompletedCB, NULL);
    treeSp->runAction(treeMoveAction);
}

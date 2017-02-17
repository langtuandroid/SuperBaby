//
//  Round034_01.cpp
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#include "Round034_01.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"

#define STAGE_PNG_FILE               "res/stage033/taizi.png"

#define STAGE_TARGET_POSX            856.0f
#define STAGE_TARGET_POSY            0.0f
#define STAGE_DOWN_OFFSET            -650.0f
#define STAGE_TAG                    101

/*
 *  constructor
 */
Round034_01::Round034_01(){}

/*
 *  destructor
 */
Round034_01::~Round034_01(){
    Director::getInstance()->getTextureCache()->removeTextureForKey(STAGE_PNG_FILE);
}

/*
 *  初始化
 */
Round034_01* Round034_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round034_01();
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

bool Round034_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto stageSp = Sprite::create(STAGE_PNG_FILE);
    stageSp->setPosition(Vec2(STAGE_TARGET_POSX, STAGE_TARGET_POSY + std::abs(STAGE_DOWN_OFFSET)));
    addChild(stageSp, 1, STAGE_TAG);
    return true;
}

//
void Round034_01::onEnterRound()
{
    const float duration = 2.5f;
    if (m_pStageScene) {
        m_pStageScene->moveLeadingActor(duration, Vec2(0.0f, STAGE_DOWN_OFFSET));
    }
    
    auto stageSp = (Sprite*)this->getChildByTag(STAGE_TAG);
    auto stageScrollCompletedCB = CallFunc::create([this] {
        if (m_completeCB) {
            m_completeCB();
        }
    });
    auto stageMoveAction = Sequence::create(MoveBy::create(duration, Vec2(0.0f, STAGE_DOWN_OFFSET)),
                                           stageScrollCompletedCB, NULL);
    stageSp->runAction(stageMoveAction);
}

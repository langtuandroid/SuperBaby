//
//  Round041_01.cpp
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#include "Round041_01.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"

#define BN_PLIST_FILE                  "res/stage041/GC-GK05-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage041/GC-GK05-IT.png"               // BN的 png 文件

#define STAGE_PNG_FILE               "GC-GK05-00-IT01.png"
#define STAGE_POS                   Vec2(504,150)
#define UP_OFFSET                   500
#define STAGE_TAG                   200
/*
 *  constructor
 */
Round041_01::Round041_01(){}

/*
 *  destructor
 */
Round041_01::~Round041_01(){
}

/*
 *  初始化
 */
Round041_01* Round041_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round041_01();
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
bool Round041_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    auto stageSp = Sprite::createWithSpriteFrameName(STAGE_PNG_FILE);
    stageSp->setPosition(Vec2(STAGE_POS.x, STAGE_POS.y - UP_OFFSET));
    addChild(stageSp, 1, STAGE_TAG);
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    return true;
}

//
void Round041_01::onEnterRound()
{
    const float duration = 2.5f;
    if (m_pStageScene) {
        m_pStageScene->moveLeadingActor(duration, Vec2(0.0f, UP_OFFSET));
    }
    
    auto stageSp = (Sprite*)this->getChildByTag(STAGE_TAG);
    auto stageScrollCompletedCB = CallFunc::create([this] {
        if (m_completeCB) {
            m_completeCB();
        }
    });
    auto stageMoveAction = Sequence::create(MoveBy::create(duration, Vec2(0.0f, UP_OFFSET)),
                                           stageScrollCompletedCB, NULL);
    stageSp->runAction(stageMoveAction);
}

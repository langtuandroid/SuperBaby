//
//  Round042_01.cpp
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#include "Round042_01.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"

#define BN_PLIST_FILE                  "res/stage042/GC-GK06-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage042/GC-GK06-IT.png"               // BN的 png 文件

#define OVER_PNG_FILE                   "res/stage042/GC-BGIT01.png"
#define OVER_POS                        Vec2(250,543)
#define OVER_TAG                        203

/*
 *  constructor
 */
Round042_01::Round042_01(){}

/*
 *  destructor
 */
Round042_01::~Round042_01(){
    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
}

/*
 *  初始化
 */
Round042_01* Round042_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round042_01();
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
bool Round042_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    auto m_pOverSp = Sprite::create(OVER_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }

    return true;
}

//
void Round042_01::onEnterRound()
{
    auto speed = AppConfig::getInstance()->getBgScrollSpeed();
    auto time = 700/speed;
    m_pStageScene->moveLeadingActorCB(time, Vec2(700, 0.0f),[this]{
        if (m_completeCB) {
            m_completeCB();
        }
    });
}
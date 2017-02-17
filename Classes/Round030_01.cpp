//
//  Round030_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round030_01.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"

#define BN_PLIST_FILE                  "res/stage030/HY-GK06-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage030/HY-GK06-IT.png"               // BN的 png 文件

#define SCREW_PNG_FILE                  "HY-GK06-1-IT04.png"
#define SCREW_POS                       Vec2(328,557)
#define SCREW_ZORDER                    2
#define DOOR_PNG_FILE                   "HY-GK06-1-IT01.png"
#define DOOR_POS                        Vec2(475,355)
#define DOOR_ZORDER                     3
#define SHAOW_PNG_FILE                  "HY-GK06-1-IT02.png"
#define SHAOW_POS                       Vec2(430,407)
#define SHAOW_ZORDER                    1
#define OVER_PNG_FILE                   "HY-GK06-1-IT03.png"
#define OVER_POS                        Vec2(251,556)
#define OVER_TAG                        203

/*
 *  constructor
 */
Round030_01::Round030_01(){}

/*
 *  destructor
 */
Round030_01::~Round030_01(){
    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    Director::getInstance()->getTextureCache()->removeTextureForKey(SCREW_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOOR_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(SHAOW_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
}

/*
 *  初始化
 */
Round030_01* Round030_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round030_01();
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
bool Round030_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pScrewSp = Sprite::createWithSpriteFrameName(SCREW_PNG_FILE);
    m_pScrewSp->setPosition(SCREW_POS);
    this->addChild(m_pScrewSp, SCREW_ZORDER);
    
    m_pDoorSp = Sprite::createWithSpriteFrameName(DOOR_PNG_FILE);
    m_pDoorSp->setPosition(DOOR_POS);
    this->addChild(m_pDoorSp, DOOR_ZORDER);
    
    m_pShadowSp = Sprite::createWithSpriteFrameName(SHAOW_PNG_FILE);
    m_pShadowSp->setPosition(SHAOW_POS);
    this->addChild(m_pShadowSp, SHAOW_ZORDER);
    
    m_pOverSp = Sprite::createWithSpriteFrameName(OVER_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

//
void Round030_01::onEnterRound()
{
    m_pDoorSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(-m_pDoorSp->getContentSize().width + 16.0f, 50.0f)), Hide::create(), CallFunc::create([this]{
        auto speed = AppConfig::getInstance()->getBgScrollSpeed();
        auto time = 700/speed;
        m_pStageScene->moveLeadingActorCB(time, Vec2(700, 0.0f),[this]{
            if (m_completeCB) {
                m_completeCB();
            }
        });
    }), NULL));
    
}
//
//  Round002_05.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/26.
//
//

#include "Round002_05.h"
#include "StageScene.h"

#define TREE_PNG_FILE       "res/stage002/SL-GK02-4-IT01.png"
#define TREE_TAG            101
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round002_05::Round002_05() {}

/*
 *  destructor
 */
Round002_05::~Round002_05() {
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
}


//
Round002_05* Round002_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round002_05();
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
bool Round002_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendRemovedTexture(TREE_PNG_FILE);
    auto treeSp = Sprite::create(TREE_PNG_FILE);
    treeSp->setPosition(Vec2(1520.0f, -200.0f));
    addChild(treeSp, 1, TREE_TAG);
    
    return true;
}

//
void Round002_05::stageOver(float dt)
{
    auto downOffset = Vec2(0.0f, -800.0f);
    const float duration = 5.0f;
    auto treeSp = (Sprite*)this->getChildByTag(TREE_TAG);
    
    if (m_pStageScene) {
        m_pStageScene->moveLeadingActor(duration, downOffset);
    }
    
    auto callfunc = CallFunc::create(CC_CALLBACK_0(Round002_05::treeDownComplete, this));
    treeSp->runAction(Sequence::create(MoveBy::create(duration, downOffset), callfunc, nullptr));
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round002_05::fadeOutLayer), 1.0f);
}

//
void Round002_05::treeDownComplete()
{
    // 返回魔法森林大地图
    m_pStageScene->stageClear();
}

//渐变变黑退出
void Round002_05::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor,COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//
void Round002_05::onEnterRound()
{
    auto treeSp = (Sprite*)this->getChildByTag(TREE_TAG);
    auto pCB = CallFunc::create([this] {
        if (m_pStageScene) {
            m_pStageScene->moveLeadingActor(2.5f, Vec2(500.0f, 0.0f), true);
        }
    });
    treeSp->runAction(pCB);
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round002_05::stageOver), 5.0f);
}

//
void Round002_05::celebrate(const float delay) {}

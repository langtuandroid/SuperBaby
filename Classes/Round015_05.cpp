//
//  Round015_05.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#include "Round015_05.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"

#define MOVE_ACTOR_TIME             3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME             2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG              300
#define DOOR_POS                    Vec2(1941,761)
#define DOOR_PNG_FILE               "XQ-GK03-4-IT01.png"
#define DOOR_BEFORE_TAG             400
#define DOOR_BEFORE_POS             Vec2(13530,730)
#define DOOR_BEFORE_PNG_FILE        "XQ-GK03-4-IT02-2.png"
#define BN_PNG_FILE                 "res/stage015/XQ-GK03-IT.png"               // BN的 png 文件
#define BN_ZORDER                   2

/*
 *  constructor
 */
Round015_05::Round015_05(){}

/*
 *  destructor
 */
Round015_05::~Round015_05(){
    m_pStageScene->removeOBLeaderNode(DOOR_BEFORE_TAG);
}

/*
 *  初始化
 */
Round015_05* Round015_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round015_05();
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
bool Round015_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    m_pDoorSP = Sprite::createWithSpriteFrameName(DOOR_PNG_FILE);
    m_pDoorSP->setPosition(DOOR_POS);
    this->addChild(m_pDoorSP);
    
    m_pDoorBeforeSp = Sprite::createWithSpriteFrameName(DOOR_BEFORE_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pDoorBeforeSp, DOOR_BEFORE_POS, DOOR_BEFORE_TAG);

    return true;
}

//
void Round015_05::onEnterRound()
{
    openDoor();
}
/*
 *  分享界面点击ok后移动主角
 */

void Round015_05::moveActor()
{
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0),true);
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round015_05::fadeOutLayer), MOVE_ACTOR_TIME-1.0f);
}

//打开门
void Round015_05::openDoor()
{
    m_pDoorSP->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_pDoorSP->getContentSize().width + 16.0f, 0.0f)), Hide::create(), CallFunc::create([this]{
        moveActor();
    }), NULL));
}

//渐变变黑退出
void Round015_05::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
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
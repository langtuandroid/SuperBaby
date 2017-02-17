//
//  Round017_05.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round017_05.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"

#define MOVE_ACTOR_TIME             3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME             3.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG              300

/*
 *  constructor
 */
Round017_05::Round017_05(){}

/*
 *  destructor
 */
Round017_05::~Round017_05(){
}

/*
 *  初始化
 */
Round017_05* Round017_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round017_05();
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
bool Round017_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    return true;
}

//
void Round017_05::onEnterRound()
{
    moveActor();
}

/*
 *  分享界面点击ok后移动主角
 */

void Round017_05::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveAimiActorCB(0.4f, Vec2(200.0f, 100.0f), [this]{
        m_pStageScene->moveAimiActorCB(4.6f, Vec2(2300.0f, 0.0f),[this]{
            fadeOutLayer();
        },false);
    },false);
    m_pStageScene->moveBeibeiActorCB(0.4f+0.44f, Vec2(420.0f, 100.0f), [this]{
        m_pStageScene->moveBeibeiActor(4.6f, Vec2(2300.0f, 0.0f));
    },false);
}

//渐变变黑退出
void Round017_05::fadeOutLayer()
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
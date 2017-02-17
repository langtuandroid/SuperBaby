//
//  Round009_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round009_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                 5.0f                                    // 庆祝延时

#define BIRD1_GAF_FILE                  "res/stage009/SL-GK09-1-MS01-D/SL-GK09-1-MS01-D.gaf"
#define BIRD2_GAF_FILE                  "res/stage009/SL-GK09-1-MS02-D/SL-GK09-1-MS02-D.gaf"

#define BIRD1_PNG_FILE                  "res/stage009/SL-GK09-1-MS01-D/SL-GK09-1-MS01-D.png"
#define BIRD2_PNG_FILE                  "res/stage009/SL-GK09-1-MS02-D/SL-GK09-1-MS02-D.png"

#define BIRD_WIDTH                      320.0f
#define BIRD_HEIGHT                     200.0f

#define BIRD_GAF_OFFSETX                -120.0f
#define BIRD_GAF_OFFSETY                300.0f

#define BIRDS_GAF_OFFSETX               -30.0f
#define BIRDS_GAF_OFFSETY               260.0f
#define LEFT_BACKGROUDN_PNG            "res/stage009/SL-GK09-BG01-IT.png"      //左边背景图
#define LEFT_BACKGROUND_TAG            1000                                     //左边背景tag值
#define LEFT_BACKGROUND_POS            Vec2(0.0f,0.0f)                              //左边背景的坐标
/*
 *  constructor
 */
Round009_01::Round009_01():
m_correctBirdIndex(0)
{
    m_targerRect[0] = Rect(456.0f,  788.0f,  BIRD_WIDTH, BIRD_HEIGHT);
    m_targerRect[1] = Rect(810.0f,  636.0f,  BIRD_WIDTH, BIRD_HEIGHT);
    m_targerRect[2] = Rect(1188.0f, 528.0f,  BIRD_WIDTH, BIRD_HEIGHT);
    m_targerRect[3] = Rect(1384.0f, 798.0f,  BIRD_WIDTH, BIRD_HEIGHT);
    m_targerRect[4] = Rect(1626.0f, 1066.0f, BIRD_WIDTH, BIRD_HEIGHT);
    for (unsigned int i = 0; i < 5; i++) {
        m_birdPos[i] = Vec2(m_targerRect[i].origin.x, m_targerRect[i].origin.y);
    } /*for*/
}

/*
 *  destructor
 */
Round009_01::~Round009_01()
{
    m_pStageScene->removeOBLeaderNode(LEFT_BACKGROUND_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    Director::getInstance()->getTextureCache()->removeTextureForKey(LEFT_BACKGROUDN_PNG);
}

//
Round009_01* Round009_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round009_01();
    if (pRound && pRound->initWithRoundIndex(roundIndex,pStageScene,pos))
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
bool Round009_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //加载左背景
    m_pLeftBackground = Sprite::create(LEFT_BACKGROUDN_PNG);
    m_pLeftBackground->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addOverLeaderNode(m_pLeftBackground,LEFT_BACKGROUND_POS,LEFT_BACKGROUND_TAG);
    
    appendAsyncLoadTexture(BIRD1_PNG_FILE);
    appendAsyncLoadTexture(BIRD2_PNG_FILE);
    preLoadRes();
    return true;
}

//
void Round009_01::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    srand(unsigned(time(0)));
    m_correctBirdIndex = static_cast<unsigned int>(rand() % 5);
    
    Actor* pBirdActor = nullptr;
    // 加载鸟
    m_pBirdActor[m_correctBirdIndex] = Actor::createWithGafFile(BIRD1_GAF_FILE);
    for (unsigned int i = 0; i < 5; i++)
    {
        if (i != m_correctBirdIndex) {
            if (pBirdActor != nullptr) {
                m_pBirdActor[i] = Actor::createWithActor(pBirdActor);
            } else {
                m_pBirdActor[i] = Actor::createWithGafFile(BIRD2_GAF_FILE);
                pBirdActor = m_pBirdActor[i];
            }
            m_pBirdActor[i]->setPosition(Vec2(m_birdPos[i].x + BIRD_GAF_OFFSETX,
                                              m_birdPos[i].y + BIRD_GAF_OFFSETY) );
        } else {
            m_pBirdActor[i]->setPosition(Vec2(m_birdPos[i].x + BIRDS_GAF_OFFSETX,
                                              m_birdPos[i].y + BIRDS_GAF_OFFSETY) );
        }
        addChild(m_pBirdActor[i], 1);
    } /*for*/
}

//
void Round009_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    for (unsigned int i = 0; i < 5; i++) {
        auto birdAction = Sequence::create(JumpBy::create(0.3f, Vec2(-36.0f, -36.0f), 36.0f, 1),
                                           JumpBy::create(0.3f, Vec2(36.0f, 36.0f), 36.0f, 1),
                                           NULL);
        m_pBirdActor[i]->runAction(birdAction);
    } /*for*/
}

// 庆祝
void Round009_01::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/* 开启触摸 */
void Round009_01::openTouch()
{
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool {
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        for (unsigned int i = 0; i < 5; i++) {
            if (m_targerRect[i].containsPoint(touchPoint)) {
                if (i == m_correctBirdIndex) /*答对*/ {
                    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
                    eventDispatcher->removeEventListenersForTarget(this);
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                    // 播放答对音效
                    playAnswerCorrectEffect();
                    if (getEffIndex() != -1) {
                        MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    }
                    // 答对时主角说话
                    playLeadAnsCorEff();
                    celebrate(3.0f);
                } else {
                    // 播放答错音效
                    playAnswerErrorEffect();
                    if (getEffIndex() != -1) {
                        MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    }
                    // 答错时主角说话
                    playLeadAnsErrEff();
                    if (m_anwerErrorCB) {
                        m_anwerErrorCB();
                    }
                }
            }
        } /*for*/
        return false;
    };
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->addEventListenerWithSceneGraphPriority(listenerRight, this);
}

/*  回合结束回调 */
void Round009_01::endRound()
{
    // 鸟飞走
    m_pBirdActor[0]->runAction(MoveBy::create(1.8f, Vec2(-1800.0f, 1600.0f)));
    m_pBirdActor[1]->runAction(MoveBy::create(1.8f, Vec2(-1800.0f, 980.0f)));
    m_pBirdActor[3]->runAction(MoveBy::create(1.8f, Vec2(-1800.0f, 20.0f)));
    
    scheduleOnce([this](float dt)
    {
     m_pBirdActor[2]->runAction(MoveBy::create(1.8f, Vec2(-1800.0f, 1200.0f)));
     m_pBirdActor[4]->runAction(MoveBy::create(1.8f, Vec2(-1800.0f, 1500.0f)));
    }, 0.6f, "birdFly");
    
    scheduleOnce([this](float dt){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 2.0f, "end");
}


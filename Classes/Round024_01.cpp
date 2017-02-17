//
//  Round024_01.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/06.
//
//

#include "Round024_01.h"
#include "StageScene.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 大图
#define STAGE_IMAGE_FILE        "res/stage024/XQ-GK12-IT.png"
#define STAGE_PLIST_FILE        "res/stage024/XQ-GK12-IT.plist"

// 机器人 gaf
#define ROBOT1_GAF                      "res/stage024/XQ-GK12-1-MS01/XQ-GK12-1-MS01.gaf"
#define ROBOT2_GAF                      "res/stage024/XQ-GK12-1-MS02/XQ-GK12-1-MS02.gaf"
#define ROBOT3_GAF                      "res/stage024/XQ-GK12-1-MS03/XQ-GK12-1-MS03.gaf"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FadeOut_TIME                    2                                       // 数字淡出时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

#define BACKGROUND_MUSIC_FILE           "res/selStage/02/audio/SQ-BGM.mp3"

/*
 *  constructor
 */
Round024_01::Round024_01() {}

/*
 *  destructor
 */
Round024_01::~Round024_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    GafAssetCache* pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(ROBOT1_GAF);
    pGafAssetCache->removeGafAssetForKey(ROBOT2_GAF);
    pGafAssetCache->removeGafAssetForKey(ROBOT3_GAF);
}

//
Round024_01* Round024_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round024_01();
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
bool Round024_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(STAGE_PLIST_FILE);
    m_pStageScene->appendRemovedTexture(STAGE_IMAGE_FILE);
    m_pStageScene->appendRemovedSpriteFrameFile(STAGE_PLIST_FILE);
    
    // 加载机器人
    float posx = 750.0f, posy = 320.0f, offsetX = 360.0f;
    m_robotActors[0] = Actor::createWithGafFile(ROBOT1_GAF, true, 1.0f, Vec2(-920.0f, 878.0f));
    m_robotActors[1] = Actor::createWithGafFile(ROBOT2_GAF, true, 1.0f, Vec2(-900.0f, 884.0f));
    m_robotActors[2] = Actor::createWithGafFile(ROBOT3_GAF, true, 1.0f, Vec2(-976.0f, 916.0f));
    for (long i = 0; i < 3; i++)
    {
        m_robotActors[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        m_robotActors[i]->setSize(Size(196.0f, 296.0f));
        m_robotActors[i]->setUserData((void*)i);
        addChild(m_robotActors[i], 1);
    } /*for*/
    
    // 随机设定位置
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3)
    {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randomIndex]) continue;
        selected[randomIndex] = true;
        m_robotActors[all]->setPosition(Vec2(posx + offsetX * randomIndex, posy));
        all++;
    } /*while*/
    
    auto spHource = Sprite::createWithSpriteFrameName("XQ-GK12-BG-IT02.png");
    m_pStageScene->addOverLeaderNode(spHource, Vec2(3542.0f, 770.0f), 1001);
    auto spHource1 = Sprite::createWithSpriteFrameName("XQ-GK12-BG-IT01.png");
    m_pStageScene->addBelowLeaderNode(spHource1, Vec2(4455.0f, 853.0f), 1002);
    
    return true;
}

// 进入本回合
void Round024_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round024_01::changeScaleRobotsSchedule), 0.6f, -1, 0);
}

void Round024_01::changeScaleRobotsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round024_01::changeScaleRobotsSchedule));
        return;
    }
    m_robotActors[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 
 *  开启触摸
 */
void Round024_01::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerTouch = EventListenerTouchOneByOne::create();
    listenerTouch->setSwallowTouches(true);
    listenerTouch->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getSize().width, target->getSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScaleY(1.1f);
            return true;
        }
        return false;
    };
    listenerTouch->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        target->setScaleY(1.0f);
        Rect rect = Rect(0.0f, 0.0f, target->getSize().width, target->getSize().height);
        if (rect.containsPoint(touchPoint))
        {
            long roboxIndex = (long)target->getUserData();
            if (roboxIndex == m_correctRobotIndex) {
                // 播放答对音效
                playAnswerCorrectEffect();
                
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          // 停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            // 停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++)
                {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_robotActors[i]);
                } /*for*/
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, m_robotActors[0]);
    for (unsigned int i = 1; i < 3; i++)
    {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch->clone(), m_robotActors[i]);
    } /*for*/
}

/*
 *  庆祝 
 */
void Round024_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB)
    {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  
 *  回合结束回调
 */
void Round024_01::endRound()
{
    if (m_completeCB)
    {
        m_completeCB();
    }
}


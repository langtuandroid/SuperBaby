//
//  Round003_02.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round003_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define GOUXUAN_GAF_FILE            "res/common/SL-goxuan/SL-goxuan.gaf"

#define BIRD1_GAF_CONGFIG_FILE      "res/stage003/SL-GK03-1-MS01-D+C.json"
#define BIRD2_GAF_CONGFIG_FILE      "res/stage003/SL-GK03-1-MS02-D+C.json"

#define BIRD1_GAF_FILE              "res/stage003/SL-GK03-1-MS01-D+C/SL-GK03-1-MS01-D+C.gaf"
#define BIRD2_GAF_FILE              "res/stage003/SL-GK03-1-MS02-D+C/SL-GK03-1-MS02-D+C.gaf"

#define BIRD_ZORDER                 1
#define GOUXUAN_ZORDER              2

#define BIRD_FLY_DURATION           4.2
#define BIRD_FLY_VEC2               Vec2(-1580.0f, 1250.0f)

/*
 *  constructor
 */
Round003_02::Round003_02()
{
    for (auto &it : m_answered) {
        it = false;
    } /*for*/
}

/*
 *  destructor
 */
Round003_02::~Round003_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(BIRD1_GAF_FILE);
    pGafAssetCache->removeGafAssetForKey(BIRD2_GAF_FILE);
    pGafAssetCache->removeGafAssetForKey(GOUXUAN_GAF_FILE);
}

//
Round003_02* Round003_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round003_02();
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
bool Round003_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf(BIRD1_GAF_FILE);
    appendAsyncLoadGaf(BIRD2_GAF_FILE);
    preLoadRes();
    return true;
}

//
void Round003_02::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    // 加载两只鸟
    m_pBird1Actor = Actor::createWithGafConfigFile(BIRD1_GAF_CONGFIG_FILE);
    m_pBird1Actor->setPosition(Vec2(624.0f, 960.0f));
    addChild(m_pBird1Actor, BIRD_ZORDER);
    m_pBird1Actor->playWithSequenceAliasName("wait", true);
    m_pBird2Actor = Actor::createWithGafConfigFile(BIRD2_GAF_CONGFIG_FILE);
    m_pBird2Actor->setPosition(Vec2(1284.0f, 1016.0f));
    addChild(m_pBird2Actor, BIRD_ZORDER);
    m_pBird2Actor->playWithSequenceAliasName("wait", true);
    
    m_bird1Rect[0] = Rect(880, 660, 160, 160);
    m_bird1Rect[1] = Rect(1210, 420, 160, 160);
    m_bird1Rect[2] = Rect(870, 480, 120, 120);
    m_bird2Rect[0] = Rect(1520, 660, 160, 160);
    m_bird2Rect[1] = Rect(1880, 420, 160, 160);
    m_bird2Rect[2] = Rect(1540, 480, 120, 120);
    m_bird1GouxuanPos[0] = Vec2(690, 960);
    m_bird1GouxuanPos[1] = Vec2(1020, 720);
    m_bird1GouxuanPos[2] = Vec2(680, 780);
    m_bird2GouxuanPos[0] = Vec2(1340, 960);
    m_bird2GouxuanPos[1] = Vec2(1680, 720);
    m_bird2GouxuanPos[2] = Vec2(1340, 780);
    
    m_touchArea = Rect(700.0f, 240.0f, 1320.0f, 680.0f);
}

/* 开启触摸 */
void Round003_02::openTouch()
{
    // 设置事件监听
    auto eventListen = EventListenerTouchOneByOne::create();
    eventListen->onTouchBegan = [this](Touch *pTouch, Event* pEvent) ->bool
    {
        auto node = pEvent->getCurrentTarget();
        auto touchPoint = node->convertTouchToNodeSpace(pTouch);
        if (!m_touchArea.containsPoint(touchPoint))
        {
            return false;
        }
        bool hasAnswerRight = false;
        for (unsigned int i = 0; i < 3; i++)
        {
            if (m_answered[i]) continue;
            if (m_bird1Rect[i].containsPoint(touchPoint) || m_bird2Rect[i].containsPoint(touchPoint) )
            {
                hasAnswerRight = true;
                if (m_pGouActor != nullptr) {
                    auto actor1 = Actor::createWithActor(m_pGouActor, false);
                    actor1->setPosition(m_bird1GouxuanPos[i]);
                    this->addChild(actor1, GOUXUAN_ZORDER);
                    m_pGouxuanActors.push_back(actor1);
                } else {
                    m_pGouActor = Actor::createWithGafFile(GOUXUAN_GAF_FILE, false);
                    m_pGouActor->setPosition(m_bird1GouxuanPos[i]);
                    this->addChild(m_pGouActor, GOUXUAN_ZORDER);
                    m_pGouxuanActors.push_back(m_pGouActor);
                }
                auto actor2 = Actor::createWithActor(m_pGouActor, false);
                actor2->setPosition(m_bird2GouxuanPos[i]);
                this->addChild(actor2, GOUXUAN_ZORDER);
                m_answered[i] = true;
                m_pGouxuanActors.push_back(actor2);
            }
        } /*for*/
        if (hasAnswerRight) {
            m_pStageScene->playAnswerCorrectEffect(pTouch->getLocation());    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            bool isAll = true;
            for (auto &it : m_answered) {
                if (!it) {
                    isAll = false;
                    break;
                }
            } /*for*/
            if (isAll) /*全部答对*/ {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(1.5f);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
            } else {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
        } else /*本次没有答对*/ {
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
        return false;
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListen, this);
}

//
void Round003_02::onEnterRound()
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
}

//
void Round003_02::celebrate(const float delay)
{
    // 停止事件监听
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
    
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

// 第二只鸟晚点飞
void Round003_02::birdFlySchedule(float dt)
{
    auto callfunc = CallFunc::create(CC_CALLBACK_0(Round003_02::birdFlyedComplete, this));
    m_pBird2Actor->playWithSequenceAliasName("fly", true);
    m_pBird2Actor->runAction(Sequence::create(EaseExponentialIn::create(MoveBy::create(BIRD_FLY_DURATION, BIRD_FLY_VEC2)), callfunc, nullptr));
}

//
void Round003_02::birdFlyedComplete()
{
    m_pBird1Actor->removeFromParent();
    m_pBird2Actor->removeFromParent();
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  回合结束回调 */
void Round003_02::endRound()
{
    for (auto &it : m_pGouxuanActors) {
        it->removeFromParent();
    } /*for*/
    m_pGouxuanActors.clear();
    
    // 鸟飞走
    m_pBird1Actor->playWithSequenceAliasName("fly", true);
    m_pBird1Actor->runAction(EaseExponentialIn::create(MoveBy::create(BIRD_FLY_DURATION, BIRD_FLY_VEC2)));
    
    // 第二只鸟晚点飞
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round003_02::birdFlySchedule), 1.2f);
}


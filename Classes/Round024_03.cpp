//
//  Round024_03.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round024_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "AppConfig.h"

using namespace gaf;

// 庆祝延时
#define CELEBRATE_DELAY                     5.0f
//
// npc
#define ROBOT_GAF_CONFIG            "res/stage024/XQ-GK12-3-MS-D+C.json"
#define ROBOT_GAF_FILE              "res/stage024/XQ-GK12-3-MS-D+C/XQ-GK12-3-MS-D+C.gaf"

//
#define GOUXUAN_GAF_FILE            "res/common/SL-goxuan/SL-goxuan.gaf"
#define GOUXUAN_ZORDER              4

/*
 *  constructor
 */
Round024_03::Round024_03()
{
    m_robot1Rect[0] = Rect(876.0f, 1058.0f, 120.0f, 120.0f);
    m_robot1Rect[1] = Rect(1050.0f, 1008.0f, 120.0f, 120.0f);
    m_robot1Rect[2] = Rect(882.0f, 802.0f, 120.0f, 120.0f);
    m_robot2Rect[0] = Rect(1502.0f, 1058.0f, 120.0f, 120.0f);
    m_robot2Rect[1] = Rect(1676.0f, 1008.0f, 120.0f, 120.0f);
    m_robot2Rect[2] = Rect(1510.0f, 802.0f, 120.0f, 120.0f);
    
    for (int i = 0; i < 3; i++)
    {
        m_robot1GouxuanPos[i].x = m_robot1Rect[i].origin.x - 190.0f;
        m_robot1GouxuanPos[i].y = m_robot1Rect[i].origin.y + 300.0f;
        m_robot2GouxuanPos[i].x = m_robot2Rect[i].origin.x - 190.0f;
        m_robot2GouxuanPos[i].y = m_robot2Rect[i].origin.y + 300.0f;
    } /*for*/
}

/*
 *  destructor
 */
Round024_03::~Round024_03()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("openTouch");
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF_FILE);
    pGafAssetCache->removeGafAssetForKey(GOUXUAN_GAF_FILE);
}

//
Round024_03* Round024_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round024_03();
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
bool Round024_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf(ROBOT_GAF_FILE);
    appendAsyncLoadGaf(GOUXUAN_GAF_FILE);
    
    preLoadRes();
    
    return true;
}

//
void Round024_03::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // npc
    m_pRobotActor = Actor::createWithGafConfigFile(ROBOT_GAF_CONFIG);
    m_pRobotActor->setPosition(Vec2(706.0f, 558.0f));
    addChild(m_pRobotActor, 3);
    m_pRobotActor->playWithSequenceAliasName("standby", true);
}

/*
 *  进入回合
 */
void Round024_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
}

/*
 *  开启触摸
 */
void Round024_03::openTouch()
{
    // 设置事件监听
    auto eventListen = EventListenerTouchOneByOne::create();
    eventListen->onTouchBegan = [this](Touch *pTouch, Event* pEvent) ->bool
    {
        auto node = pEvent->getCurrentTarget();
        auto touchPoint = node->convertTouchToNodeSpace(pTouch);
        auto touchArea = m_pRobotActor->getBoundingBox();
        if (!touchArea.containsPoint(touchPoint))
        {
            return false;
        }
        bool hasAnswerRight = false;
        for (unsigned int i = 0; i < 3; i++)
        {
            if (m_answered[i]) continue;
            if (m_robot1Rect[i].containsPoint(touchPoint) || m_robot2Rect[i].containsPoint(touchPoint) )
            {
                hasAnswerRight = true;
                if (m_pGouActor != nullptr) {
                    auto actor1 = Actor::createWithActor(m_pGouActor, false);
                    actor1->setPosition(m_robot1GouxuanPos[i]);
                    addChild(actor1, GOUXUAN_ZORDER);
                    m_pGouxuanActors.push_back(actor1);
                } else {
                    m_pGouActor = Actor::createWithGafFile(GOUXUAN_GAF_FILE, false);
                    m_pGouActor->setPosition(m_robot1GouxuanPos[i]);
                    addChild(m_pGouActor, GOUXUAN_ZORDER);
                    m_pGouxuanActors.push_back(m_pGouActor);
                }
                auto actor2 = Actor::createWithActor(m_pGouActor, false);
                actor2->setPosition(m_robot2GouxuanPos[i]);
                this->addChild(actor2, GOUXUAN_ZORDER);
                m_answered[i] = true;
                m_pGouxuanActors.push_back(actor2);
            }
        } /*for*/
        if (hasAnswerRight)
        {
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

/*
 *  庆祝
 */
void Round024_03::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB)
    {
        m_celebrateCB(1);
    }
    
    m_pRobotActor->playWithSequenceAliasName("trigger", false);
    m_pRobotActor->setAnimationFinishedPlayDelegate([this]
    {
        // 播放过关庆祝动画
        m_pStageScene->getRoundUINode()->playPassActor(2);
    });
}

/* 
 *  回合结束回调
 */
void Round024_03::endRound()
{
    if (m_completeCB)
    {
        m_completeCB();
    }
}

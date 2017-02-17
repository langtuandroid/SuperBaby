//
//  Round023_01.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/06.
//
//

#include "Round023_01.h"
#include "StageScene.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define GAF_FILE1       "res/stage023/XQ-GK11-1-DA01-D+C/XQ-GK11-1-DA01-D+C.gaf"
#define GAF_FILE2       "res/stage023/XQ-GK11-1-DA02-D+C/XQ-GK11-1-DA02-D+C.gaf"
#define GAF_FILE3       "res/stage023/XQ-GK11-1-DA03-D+C/XQ-GK11-1-DA03-D+C.gaf"
#define GAF_CONFIG1     "res/stage023/XQ-GK11-1-DA01-D+C.json"
#define GAF_CONFIG2     "res/stage023/XQ-GK11-1-DA02-D+C.json"
#define GAF_CONFIG3     "res/stage023/XQ-GK11-1-DA03-D+C.json"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

/*
 *  constructor
 */
Round023_01::Round023_01() {}

/*
 *  destructor
 */
Round023_01::~Round023_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("openTouch");
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(GAF_FILE1);
    pGafAssetCache->removeGafAssetForKey(GAF_FILE2);
    pGafAssetCache->removeGafAssetForKey(GAF_FILE3);
}

//
Round023_01* Round023_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round023_01();
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
bool Round023_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pActors[0] = Actor::createWithGafConfigFile(GAF_CONFIG1);
    m_pActors[1] = Actor::createWithGafConfigFile(GAF_CONFIG2);
    m_pActors[2] = Actor::createWithGafConfigFile(GAF_CONFIG3);
    for (long i = 0; i < 3; i++)
    {
        m_pActors[i]->setUserData((void*)i);
        addChild(m_pActors[i], 1);
        m_pActors[i]->playWithSequenceAliasName("standby", true);
        m_pActors[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
    } /*for*/
    
    // 随机设定位置
    float posx = 782.0f, posy = 532.0f, offsetX = 420.0f;
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3)
    {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randomIndex]) continue;
        selected[randomIndex] = true;
        m_pActors[all]->setPosition(Vec2(posx + offsetX * randomIndex, posy));
        m_pJumpActors[randomIndex] = m_pActors[all];
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round023_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
        dispatcher->removeCustomEventListeners("openTouch");
        m_opentouched = true;
        if (m_jumped) {
            openTouch();
        }
    });
    
    m_pJumpActors[0]->setAnimationFinishedPlayDelegate(std::bind(&Round023_01::jumpCompleted, this));
    m_pJumpActors[0]->playWithSequenceAliasName("trigger", false);
}

//
void Round023_01::jumpCompleted()
{
    if (++m_numberOfJumped == 3)
    {
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pJumpActors[m_numberOfJumped]->setAnimationFinishedPlayDelegate(std::bind(&Round023_01::jumpCompleted, this));
    m_pJumpActors[m_numberOfJumped]->playWithSequenceAliasName("trigger", false);
}

/*
 *  开启触摸
 */
void Round023_01::openTouch()
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
            long selIndex = (long)target->getUserData();
            if (selIndex == m_correctIndex) {
                celebrate(CELEBRATE_DELAY);
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (int i = 0; i < 3; i++) {
                    dispatcher->removeEventListenersForTarget(m_pActors[i]);
                } /*for*/
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          // 停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            // 停止读题目改变动作
                
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
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, m_pActors[0]);
    for (unsigned int i = 1; i < 3; i++)
    {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch->clone(), m_pActors[i]);
    } /*for*/
}

/*
 *  庆祝 
 */
void Round023_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round023_01::endRound()
{
    if (m_completeCB)
    {
        m_completeCB();
    }
}


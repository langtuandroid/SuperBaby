//
//  Round028_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Round028_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(818,868)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define CRAB_ONE_CONFIG_GAF_FILE        "res/stage028/crabone.json"
#define CRAB_TWO_CONFIG_GAF_FILE        "res/stage028/crabtwo.json"
#define CRAB_ONE_GAF_FILE               "res/stage028/HY-GK04-3-MS01/HY-GK04-3-MS01.gaf"
#define CRAB_TWO_GAF_FILE               "res/stage028/HY-GK04-3-MS02/HY-GK04-3-MS02.gaf"
#define CRAB_ZORDER                     2
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define MOVE                            "move"                                       //移动动画

/*
 *  constructor
 */
Round028_04::Round028_04() {
    m_crabsPos[0] = Vec2(750, 339);
    m_crabsPos[1] = Vec2(1088, 187);
    m_crabsPos[2] = Vec2(1426, 369);
    m_crabsPos[3] = Vec2(1764, 247);
}

/*
 *  destructor
 */
Round028_04::~Round028_04()
{
    cleanUp();
}

//
void Round028_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(CRAB_ONE_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(CRAB_TWO_GAF_FILE);
}

//
Round028_04* Round028_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round028_04();
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
bool Round028_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pPaopaoActor = Actor::createWithGafFile(PAOPAO_GAF_FILE);
    m_pPaopaoActor->setPosition(PAOPAO_GAF_POS);
    this->addChild(m_pPaopaoActor, PAOPAO_GAF_ZORDER);
    
    m_pWhiteLightActor = Actor::createWithGafFile(WHITELIGHT_GAF_FILE);
    m_pWhiteLightActor->setPosition(WHITELIGHT_GAF_POS);
    m_pWhiteLightActor->setSize(Size(300, 300));
    m_pWhiteLightActor->setPosAtNode(Vec2(-151, 747));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    for (unsigned long i = 0; i < 4; i++) {
        if (i == 3) {
            m_pCrabsActor[i] = Actor::createWithGafConfigFile(CRAB_TWO_CONFIG_GAF_FILE);
        }else
        {
            m_pCrabsActor[i] = Actor::createWithGafConfigFile(CRAB_ONE_CONFIG_GAF_FILE);
        }
        m_pCrabsActor[i]->setUserData((void*)i);
        m_pCrabsActor[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        m_pCrabsActor[i]->playWithSequenceAliasName(STANDBY,true);
        this->addChild(m_pCrabsActor[i], CRAB_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pCrabsActor[all]->setPosition(m_crabsPos[randIndex]);
        if (randIndex == 1) {
            m_pCrabsActor[all]->setLocalZOrder(CRAB_ZORDER+2);
        } else if (randIndex == 3)
        {
            m_pCrabsActor[all]->setLocalZOrder(CRAB_ZORDER+1);
        }
        if (randIndex == 3) {
            m_curLastPosInex = all;
        }
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round028_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        m_opentouched = true;
        if (m_jumped) {
            openTouch();
        }
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round028_04::changeScaleCrabsSchedule), 0.6f, -1, 0);
}

//螃蟹依次改变大小以提示
void Round028_04::changeScaleCrabsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round028_04::changeScaleCrabsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pCrabsActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round028_04::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 3) {   //答对
                changeActor();
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pCrabsActor[i]);
                } /*for*/
                m_InitAniIndex = 0;
            } else {
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
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pCrabsActor[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pCrabsActor[i]);
    } /*for*/
}

//答对后改变actor
void Round028_04::changeActor()
{
    m_pCrabsActor[3]->playWithSequenceAliasName(TRIGGER,true);
}

//答对后右移球
void Round028_04::actionCrabs()
{
    for (unsigned int i = 0; i < 4; i++) {
        if (m_curLastPosInex != i) {
            if (i == 3) {
                m_pCrabsActor[i]->playWithSequenceAliasName(MOVE,true);
            }else
            {
                m_pCrabsActor[i]->playWithSequenceAliasName(TRIGGER,true);
            }
            m_pCrabsActor[i]->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(1500, 0.0f)),CallFunc::create([this,i]{
                m_pCrabsActor[i]->setVisible(false);
            }), NULL));
        }
    }
    scheduleOnce([this](float){
        if (m_curLastPosInex == 3) {
            m_pCrabsActor[m_curLastPosInex]->playWithSequenceAliasName(MOVE,true);
        }else
        {
            m_pCrabsActor[m_curLastPosInex]->playWithSequenceAliasName(TRIGGER,true);
        }
        
        m_pCrabsActor[m_curLastPosInex]->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(500, 0.0f)),CallFunc::create([this]{
            m_pCrabsActor[m_curLastPosInex]->setVisible(false);
            if (m_completeCB) {
                m_completeCB();
            }
        }), NULL));
    }, 1.0f, "delaytimeMove");
}

/*
 *  庆祝
 */
void Round028_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round028_04::endRound()
{
    actionCrabs();
}
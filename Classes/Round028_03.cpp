//
//  Round028_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Round028_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FISH_ZORDER                     3
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(1218,1068)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define FISH_ONE_GAF_FILE               "res/stage028/HY-GK04-2-MS01-D/HY-GK04-2-MS01-D.gaf"
#define FISH_TWO_GAF_FILE               "res/stage028/HY-GK04-2-MS02-D/HY-GK04-2-MS02-D.gaf"

/*
 *  constructor
 */
Round028_03::Round028_03()
{
    
    m_fishsPos[0] = Vec2(769, 703);
    m_fishsPos[1] = Vec2(854, 1051);
    m_fishsPos[2] = Vec2(1205, 1171);
    m_fishsPos[3] = Vec2(1293, 711);
    m_fishsPos[4] = Vec2(1640, 1187);
}

/*
 *  destructor
 */
Round028_03::~Round028_03()
{
    cleanUp();
}

//
void Round028_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(FISH_ONE_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(FISH_TWO_GAF_FILE);
}

//
Round028_03* Round028_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round028_03();
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
bool Round028_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    for (unsigned long i = 0; i < 5; i++) {
        if (i < 3) {
            m_pFishsActor[i] = Actor::createWithGafFile(FISH_TWO_GAF_FILE);
            m_pFishsActor[i]->setPosAtNode(Vec2(-129, 354));
            if ( i == 2) {
                m_pFishsActor[i]->setRotationSkewY(-180);
            }
        }else
        {
            m_pFishsActor[i] = Actor::createWithGafFile(FISH_ONE_GAF_FILE);
            m_pFishsActor[i]->setPosAtNode(Vec2(-134, 356));
        }
        this->addChild(m_pFishsActor[i], FISH_ZORDER);
        m_pFishsActor[i]->setSize(Size(300, 300));
        m_pFishsActor[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
        m_pFishsActor[i]->setUserData((void*)i);
    }
    
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pFishsActor[all]->setPosition(m_fishsPos[randIndex]);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round028_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round028_03::changeScalefishsSchedule), 0.6f, -1, 0);   //
}

//依次放大船以提示
void Round028_03::changeScalefishsSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round028_03::changeScalefishsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pFishsActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round028_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (2 == index) {   //答对
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pFishsActor[i]);
                } /*for*/
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
        }else
        {
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
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pFishsActor[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pFishsActor[i]);
    } /*for*/
}

//答对移动鱼
void Round028_03::moveFish()
{
    m_pFishsActor[2]->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(-200, 0.0f)),CallFunc::create([this]{
        m_pFishsActor[2]->setRotationSkewY(360);
    }),MoveBy::create(3.1f, Vec2(1600, 0.0f)),CallFunc::create([this]{
        m_pFishsActor[2]->setVisible(false);
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));

    for (unsigned int i = 0; i < 5; i++) {
        if (i != 2) {
            m_pFishsActor[i]->runAction(Sequence::create(MoveBy::create(4.1f, Vec2(1600, 0.0f)),CallFunc::create([this,i]{
                m_pFishsActor[i]->setVisible(false);
            }), NULL));
        }
    }
}

/*
 *  庆祝
 */
void Round028_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round028_03::endRound()
{
    moveFish();
}
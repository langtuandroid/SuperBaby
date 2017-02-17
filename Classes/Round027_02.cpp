//
//  Round027_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#include "Round027_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(224,1026)
#define BOGUANG_GAF_ZORDER              1
#define SHIP_ZORDER                     3
#define SPRAYS_ZORDER                   2
#define SPRAYS_GAF_FILE                 "res/stage027/HY-GK03-2-TX/HY-GK03-2-TX.gaf"

/*
 *  constructor
 */
Round027_02::Round027_02()
:m_InitAniIndex(0)
{
    
    m_shipsPos[0] = Vec2(426, 695);
    m_shipsPos[1] = Vec2(809, 695);
    m_shipsPos[2] = Vec2(1213, 695);
    m_shipsPos[3] = Vec2(1661, 695);
    
    m_spraysPos[0] = Vec2(276, 695);
    m_spraysPos[1] = Vec2(659, 695);
    m_spraysPos[2] = Vec2(1063, 695);
    m_spraysPos[3] = Vec2(1511, 695);
}

/*
 *  destructor
 */
Round027_02::~Round027_02()
{
    cleanUp();
}

//
void Round027_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(SPRAYS_GAF_FILE);
}

//
Round027_02* Round027_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round027_02();
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
bool Round027_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    for (unsigned int i = 0; i < 4; i++) {
        m_pSpraysActor[i] = Actor::createWithGafFile(SPRAYS_GAF_FILE);
        m_pSpraysActor[i]->setPosition(m_spraysPos[i]);
        this->addChild(m_pSpraysActor[i], SPRAYS_ZORDER);
        m_pSpraysActor[i]->setSize(Size(300, 300));
        m_pSpraysActor[i]->setPosAtNode(Vec2(-850, 766));
    }
    
    srand(unsigned(time(0)));
    long randNum = rand() % 3;
    long randPosOne = rand() % 4;
    long randPosTwo = rand() % 4;
    while (randPosTwo == randPosOne) {
        randPosTwo = rand() % 4;
    }
    char buffer[64];
    long index = randNum;
    for (unsigned long i = 0; i < 4; i++) {
        if (i == randPosOne || i == randPosTwo) {
            sprintf(buffer, "HY-GK03-2-DA%02ld.png",randNum+1);
        }else
        {
            if (randNum == 0) {
                sprintf(buffer, "HY-GK03-2-DA%02ld.png",index+2);
                index++;
            }else if(randNum == 1)
            {
                sprintf(buffer, "HY-GK03-2-DA%02ld.png",index);
                index = index+2;
            }else
            {
                sprintf(buffer, "HY-GK03-2-DA%02ld.png",index);
                index--;
            }
        }
        m_pShipsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        if (i == randPosTwo || i == randPosOne) {
            m_pShipsSp[i]->setUserData((void*)10);
        }else
        {
            m_pShipsSp[i]->setUserData((void*)i);
        }
        m_pShipsSp[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        this->addChild(m_pShipsSp[i], SHIP_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pShipsSp[all]->setPosition(m_shipsPos[randIndex]);
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round027_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round027_02::changeScaleShipsSchedule), 0.6f, -1, 0);   //
}

//依次放大船以提示
void Round027_02::changeScaleShipsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round027_02::changeScaleShipsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pShipsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round027_02::openTouch()
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
            if (10 == index) {   //答对
                target->setScale(1.1f);
                m_curCorrectNum++;
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                if (m_curCorrectNum == 2) {
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    celebrate(CELEBRATE_DELAY);
                    for (unsigned int i = 0; i < 4; i++) {
                        Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pShipsSp[i]);
                    } /*for*/
                }else{
                    if (m_celebrateCB) {
                        m_celebrateCB(false);
                    }
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
                }
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pShipsSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pShipsSp[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round027_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round027_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

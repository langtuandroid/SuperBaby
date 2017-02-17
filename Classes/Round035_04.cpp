//
//  Round035_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#include "Round035_04.h"
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
#define BUBBLE_ZORDER                   2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define FLOWERS_ZORDER                  3

/*
 *  constructor
 */
Round035_04::Round035_04() {
    m_flowersPos[0] = Vec2(806, 252);
    m_flowersPos[1] = Vec2(1050, 252);
    m_flowersPos[2] = Vec2(1294, 252);
    m_flowersPos[3] = Vec2(1538, 252);
    
    m_bubblesPos[0] = Vec2(816, 666);
    m_bubblesPos[1] = Vec2(1050, 666);
    m_bubblesPos[2] = Vec2(1284, 666);
    m_bubblesPos[3] = Vec2(1568, 666);
    
}

/*
 *  destructor
 */
Round035_04::~Round035_04()
{
    cleanUp();
}

//
void Round035_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round035_04* Round035_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round035_04();
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
bool Round035_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "HY-GK11-3-IT%02ld.png",i+1);
        m_pBubbleSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBubbleSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        m_pBubbleSp[i]->setPosition(m_bubblesPos[i]);
        this->addChild(m_pBubbleSp[i], BUBBLE_ZORDER);
    }
    
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "HY-GK11-3-DA%02ld.png",i+1);
        m_pFlowersSp[i] =Sprite::createWithSpriteFrameName(buffer);
        m_pFlowersSp[i]->setUserData((void*)i);
        m_pFlowersSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(m_pFlowersSp[i], FLOWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pFlowersSp[all]->setPosition(m_flowersPos[randIndex]);
        m_flowersEndPos[all] = m_flowersPos[randIndex];
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round035_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round035_04::changeScaleFlowersSchedule), 0.8f, -1, 2.0f);
}
//改变花大小
void Round035_04::changeScaleFlowersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round035_04::changeScaleFlowersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round035_04::changeScaleBubblesSchedule), 0.8f, -1, 0.0f);
        return;
    }
    m_pFlowersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f), NULL));
}

//改变气泡大小
void Round035_04::changeScaleBubblesSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round035_04::changeScaleBubblesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pBubbleSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f), NULL));
}

/* 开启触摸 */
void Round035_04::openTouch()
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
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setLocalZOrder(10);
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        int bubbleIndex = 0;
        Rect rect = Rect(0.0f, 0.0f, m_pBubbleSp[0]->getContentSize().width, m_pBubbleSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBubbleSp[1]->getContentSize().width, m_pBubbleSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBubbleSp[2]->getContentSize().width, m_pBubbleSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[2]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 3) {
                isCorrect = true;
                bubbleIndex = 2;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBubbleSp[3]->getContentSize().width, m_pBubbleSp[3]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[3]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
                bubbleIndex = 3;
            }
        }
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pBubbleSp[bubbleIndex]->getPosition());
            m_pBubbleSp[bubbleIndex]->runAction(FadeOut::create(0.3f));
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 4) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_flowersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(FLOWERS_ZORDER);
                target->setPosition(m_flowersEndPos[index]);
            }), NULL));
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pFlowersSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pFlowersSp[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round035_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round035_04::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
//
//  Round029_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Round029_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define SHELLS_ZORDER                   3
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(818,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(518,868)
#define WHITELIGHT_GAF_ZORDER           1
#define BASKET_PNG_FILE                 "HY-GK05-3-IT01.png"
#define BASKET_POS                      Vec2(1150,250)
#define BASKET_ZORDER                   2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round029_03::Round029_03() {
    
    m_shellsPos[0] = Vec2(589, 847);
    m_shellsPos[1] = Vec2(822, 1059);
    m_shellsPos[2] = Vec2(1154, 747);
    m_shellsPos[3] = Vec2(1453, 1051);
}

/*
 *  destructor
 */
Round029_03::~Round029_03()
{
    cleanUp();
}

//
void Round029_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round029_03* Round029_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round029_03();
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
bool Round029_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    m_pWhiteLightActor->setPosAtNode(Vec2(-300, 400));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    m_pBasketSp = Sprite::createWithSpriteFrameName(BASKET_PNG_FILE);
    m_pBasketSp->setPosition(BASKET_POS);
    this->addChild(m_pBasketSp, BASKET_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "HY-GK05-3-DA%02ld.png",i+1);
        m_pShellsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pShellsSp[i]->setUserData((void*)i);
        this->addChild(m_pShellsSp[i], SHELLS_ZORDER);
        if (i == 0) {
            m_pShellsSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else if (i == 1)
        {
            m_pShellsSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(-10.0f, -10.f)),MoveBy::create(0.5f, Vec2(10.0f, 10.0f)), NULL)));
        }else if (i == 2)
        {
            m_pShellsSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(10.0f, -10.f)),MoveBy::create(0.5f, Vec2(-10.0f, 10.0f)), NULL)));
        }else
        {
            m_pShellsSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(10.0f, -10.f)),MoveBy::create(0.5f, Vec2(-10.0f, 10.0f)), NULL)));
        }
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pShellsSp[all]->setPosition(m_shellsPos[randIndex]);
        m_shellsEndPos[all] = m_shellsPos[randIndex];
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round029_03::onEnterRound()
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
    changeScaleShells();
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round029_03::changeScaleBacket), 0.65f);
}

void Round029_03::changeScaleShells()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_pShellsSp[i]->runAction(Sequence::create(ScaleTo::create(0.5f, 1.1f),ScaleTo::create(0.5f, 1.0f), NULL));
    }
}

void Round029_03::changeScaleBacket(float)
{
    m_pBasketSp->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}


/* 开启触摸 */
void Round029_03::openTouch()
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
            target->setLocalZOrder(5);
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
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
        auto touchPoint = m_pBasketSp->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pBasketSp->getContentSize().width, m_pBasketSp->getContentSize().height+100);
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) {   //答对
                target->setPosition(Vec2(m_pBasketSp->getPosition().x, m_pBasketSp->getPosition().y+100));
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
                chooseComplete();
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pShellsSp[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_shellsEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(SHELLS_ZORDER);
                    target->setPosition(m_shellsEndPos[index]);
                }), NULL));
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_shellsEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(SHELLS_ZORDER);
                target->setPosition(m_shellsEndPos[index]);
            }), NULL));
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pShellsSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pShellsSp[i]);
    } /*for*/
}

//答对后相关动作
void Round029_03::chooseComplete()
{
    m_pShellsSp[0]->runAction(Sequence::create(MoveBy::create(0.4f, Vec2(0.0f, 100.0f)),MoveBy::create(0.3f, Vec2(0.0f, -100.0f)),Spawn::create(FadeOut::create(0.4f),CallFunc::create([this]{
        for (unsigned int i = 0; i < 4; i++) {
            if (i != 0) {
                m_pShellsSp[i]->runAction(FadeOut::create(0.4f));
            }
        }
        m_pBasketSp->runAction(FadeOut::create(0.3f));
    }), NULL), NULL));
}

/*
 *  庆祝
 */
void Round029_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round029_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
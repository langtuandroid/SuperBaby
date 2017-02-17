//
//  Round036_03.cpp
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#include "Round036_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define RUBBISHES_ZORDER                5
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(1218,1068)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define TRASHES_ZORDER                  2
#define COVERS_ZORDER                   3
#define MARKS_ZORDER                    4

/*
 *  constructor
 */
Round036_03::Round036_03()
{
    m_rubbishesPos[0] = Vec2(830, 278);
    m_rubbishesPos[1] = Vec2(1366, 278);
    
    m_trashesPos[0] = Vec2(797, 695);
    m_trashesPos[1] = Vec2(1362, 695);
    
    m_coversPos[0] = Vec2(605, 873);
    m_coversPos[1] = Vec2(1168, 873);
    
    m_marksPos[1] = Vec2(716, 658);
    m_marksPos[0] = Vec2(1278, 667);
}

/*
 *  destructor
 */
Round036_03::~Round036_03()
{
    cleanUp();
}

//
void Round036_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round036_03* Round036_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round036_03();
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
bool Round036_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK12-3-DA%02ld.png",i+1);
        m_pRubbishesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pRubbishesSp[i]->setUserData((void*)i);
        this->addChild(m_pRubbishesSp[i], RUBBISHES_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK12-3-TM%02d-tong.png",i+1);
        m_pTrashesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTrashesSp[i]->setPosition(m_trashesPos[i]);
        this->addChild(m_pTrashesSp[i], TRASHES_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK12-3-TM01-gai.png");
        m_pCoversSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pCoversSp[i]->setPosition(m_coversPos[i]);
        m_pCoversSp[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        this->addChild(m_pCoversSp[i], COVERS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK12-3-IT%02d.png",i+1);
        m_pMarksSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pMarksSp[i]->setPosition(m_marksPos[i]);
        m_pMarksSp[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        this->addChild(m_pMarksSp[i], MARKS_ZORDER);
        m_pMarksSp[i]->setOpacity(0);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pRubbishesSp[all]->setPosition(m_rubbishesPos[randIndex]);
        m_rubbishesEndPos[all] = m_rubbishesPos[randIndex];
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round036_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round036_03::rotateRubbishesSchedule), 1.6f, -1, 0);   //
}

//抖动垃圾
void Round036_03::rotateRubbishesSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_03::rotateRubbishesSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round036_03::fadeMarksSchedule), 1.2f, -1, 0);   //
        return;
    }
    m_pRubbishesSp[m_InitAniIndex++]->runAction(Repeat::create(Sequence::create(RotateBy::create(0.2f, 20.0f),RotateBy::create(0.2f, -20.0f),RotateBy::create(0.2f, -20.0f),RotateBy::create(0.2f, 20.0f), NULL), 2));
}

//显隐标志
void Round036_03::fadeMarksSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_03::fadeMarksSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pMarksSp[m_InitAniIndex++]->runAction(Sequence::create(FadeIn::create(0.3f),FadeOut::create(0.3f),FadeIn::create(0.3f),FadeOut::create(0.3f), NULL));
}

/* 开启触摸 */
void Round036_03::openTouch()
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setLocalZOrder(10);
            target->setScale(1.1f);
            openCovers();
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        int bubbleIndex = 0;
        Rect rect = Rect(0.0f, 0.0f, m_pTrashesSp[0]->getContentSize().width, m_pTrashesSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pTrashesSp[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTrashesSp[1]->getContentSize().width, m_pTrashesSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pTrashesSp[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }
        if (isCorrect) {
            m_curCorrectNum++;
            collectRubbishes(target,bubbleIndex);
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 2) {
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
            closeCovers();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_rubbishesEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(RUBBISHES_ZORDER);
                target->setPosition(m_rubbishesEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pRubbishesSp[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pRubbishesSp[1]);
}

//打开盖子
void Round036_03::openCovers()
{
    m_pCoversSp[0]->runAction(RotateTo::create(0.3f, -120));
    m_pCoversSp[1]->runAction(RotateTo::create(0.3f, -120));
}

//关闭盖子
void Round036_03::closeCovers()
{
    m_pCoversSp[0]->runAction(RotateTo::create(0.3f, 0));
    m_pCoversSp[1]->runAction(RotateTo::create(0.3f, 0));
}

//收集
void Round036_03::collectRubbishes(Sprite* rubbish, const unsigned int type)
{
    if (type == 0) {
        rubbish->runAction(Sequence::create(JumpTo::create(0.5f, Vec2(797, 795), 400, 1),CallFunc::create([this,rubbish]{
            rubbish->setLocalZOrder(1);
        }),Spawn::create(MoveBy::create(0.3f, Vec2(0.0f, -200.0f)),Sequence::create(DelayTime::create(0.1f),ScaleTo::create(0.2f, 0.0f), NULL),NULL),CallFunc::create([this]{
            closeCovers();
        }),NULL));
    }else
    {
        rubbish->runAction(Sequence::create(JumpTo::create(0.5f, Vec2(1362, 795), 400, 1),CallFunc::create([this,rubbish]{
            rubbish->setLocalZOrder(1);
        }),Spawn::create(MoveBy::create(0.3f, Vec2(0.0f, -200.0f)),Sequence::create(DelayTime::create(0.1f),ScaleTo::create(0.2f, 0.0f), NULL), NULL),CallFunc::create([this]{
            closeCovers();
        }),NULL));
    }
}

/*
 *  庆祝
 */
void Round036_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round036_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
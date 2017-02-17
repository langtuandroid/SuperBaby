//
//  Round027_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#include "Round027_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define BALLS_ZORDER                    3
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(224,1026)
#define BOGUANG_GAF_ZORDER              1
/*
 *  constructor
 */
Round027_03::Round027_03() {
    
    m_ballsPos[0] = Vec2(855, 643);
    m_ballsPos[1] = Vec2(1202, 643);
    m_ballsPos[2] = Vec2(1557, 643);
}

/*
 *  destructor
 */
Round027_03::~Round027_03()
{
    cleanUp();
}

//
void Round027_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round027_03* Round027_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round027_03();
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
bool Round027_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK03-3-DA%02ld.png",i+1);
        m_pBallsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBallsSp[i]->setUserData((void*)i);
        this->addChild(m_pBallsSp[i], BALLS_ZORDER);
        m_pBallsSp[i]->setOpacity(0);
        m_pBallsSp[i]->setScale(0.0f);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pBallsSp[all]->setPosition(m_ballsPos[randIndex]);
        m_randNums[randIndex] = all;
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round027_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round027_03::fadeScaleBallsSchedule), 1.2f, -1, 0);
}

//上升答案
void Round027_03::fadeScaleBallsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round027_03::fadeScaleBallsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pBallsSp[m_InitAniIndex]->runAction(Sequence::create(Spawn::create(FadeIn::create(0.2f),ScaleTo::create(0.2f, 1.0f),MoveBy::create(0.2f, Vec2(0.0f, -200.0f)), NULL),CallFunc::create([this]{
        m_pBallsSp[m_InitAniIndex]->setScaleX(1.05f);
        m_pBallsSp[m_InitAniIndex]->setScaleY(0.95f);
    }),DelayTime::create(0.05f),CallFunc::create([this]{
        m_pBallsSp[m_InitAniIndex]->setScaleX(1.0f);
        m_pBallsSp[m_InitAniIndex]->setScaleY(1.0f);
    }),MoveBy::create(0.3f, Vec2(0.0f, 200.0f)),CallFunc::create([this]{
        m_pBallsSp[m_InitAniIndex]->setScaleX(0.95f);
        m_pBallsSp[m_InitAniIndex]->setScaleY(1.05f);
    }),MoveBy::create(0.3f, Vec2(0.0f, -200.0f)),CallFunc::create([this]{
        m_pBallsSp[m_InitAniIndex]->setScaleX(1.0f);
        m_pBallsSp[m_InitAniIndex]->setScaleY(1.0f);
        m_InitAniIndex++;
    }), NULL));
}

/* 开启触摸 */
void Round027_03::openTouch()
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
            if (index == 2) {   //答对
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
                scheduleOnce([this](float){
                    actionBallsSchedule();
                }, 1.0f, "delaytimeBall");
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pBallsSp[i]);
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
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pBallsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pBallsSp[i]);
    } /*for*/
}

//答对后下降答案
void Round027_03::actionBallsSchedule()
{
    m_pBallsSp[m_randNums[2]]->runAction(Sequence::create(Repeat::create(Sequence::create(MoveBy::create(0.3f, Vec2(300, 200)),CallFunc::create([this]{
        m_pBallsSp[m_randNums[2]]->setScaleX(1.05f);
        m_pBallsSp[m_randNums[2]]->setScaleY(0.95f);
    }),MoveBy::create(0.3f, Vec2(300, -200)),CallFunc::create([this]{
        m_pBallsSp[m_randNums[2]]->setScaleX(1.0f);
        m_pBallsSp[m_randNums[2]]->setScaleY(1.0f);
    }), NULL), 3),CallFunc::create([this]{
        m_pBallsSp[m_randNums[2]]->setVisible(false);
    }), NULL));
    
    scheduleOnce([this](float){
        m_pBallsSp[m_randNums[1]]->runAction(Sequence::create(Repeat::create(Sequence::create(MoveBy::create(0.3f, Vec2(300, 200)),CallFunc::create([this]{
            m_pBallsSp[m_randNums[1]]->setScaleX(1.05f);
            m_pBallsSp[m_randNums[1]]->setScaleY(0.95f);
        }),MoveBy::create(0.3f, Vec2(300, -200)),CallFunc::create([this]{
            m_pBallsSp[m_randNums[1]]->setScaleX(1.0f);
            m_pBallsSp[m_randNums[1]]->setScaleY(1.0f);
        }), NULL), 3),CallFunc::create([this]{
            m_pBallsSp[m_randNums[1]]->setVisible(false);
        }), NULL));
    }, 0.3f, "delaytimeOne");
    
    scheduleOnce([this](float){
        m_pBallsSp[m_randNums[0]]->runAction(Sequence::create(Repeat::create(Sequence::create(MoveBy::create(0.3f, Vec2(300, 200)),CallFunc::create([this]{
            m_pBallsSp[m_randNums[0]]->setScaleX(1.05f);
            m_pBallsSp[m_randNums[0]]->setScaleY(0.95f);
        }),MoveBy::create(0.3f, Vec2(300, -200)),CallFunc::create([this]{
            m_pBallsSp[m_randNums[0]]->setScaleX(1.0f);
            m_pBallsSp[m_randNums[0]]->setScaleY(1.0f);
        }), NULL), 3),CallFunc::create([this]{
            m_pBallsSp[m_randNums[0]]->setVisible(false);
        }), NULL));
    }, 0.6f, "delaytimeTwo");
}

/*
 *  庆祝
 */
void Round027_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round027_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
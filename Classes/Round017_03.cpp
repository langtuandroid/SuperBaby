//
//  Round017_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round017_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define CELEBRATE_DELAY                 5.0f
#define ROPE_ZORDER                     3
#define ARROWS_ZORDER                   2
#define ARROWS_PNG_FILE                 "XQ-GK05-3-IT02.png"
#define ARROWS_POS                      Vec2(835,804)
#define BOARD_POS                       Vec2(1206,804)
#define BOARD_PNG_FILE                  "XQ-GK05-3-IT01.png"
#define BOARD_ZORDER                    1

/*
 *  constructor
 */
Round017_03::Round017_03()
:m_InitAniIndex(0)
{
    m_answerRopesPos[0] = Vec2(887, 780);
    m_answerRopesPos[1] = Vec2(1216, 780);
    m_answerRopesPos[2] = Vec2(1590, 780);
}

/*
 *  destructor
 */
Round017_03::~Round017_03()
{
    cleanUp();
}

//
void Round017_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round017_03* Round017_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round017_03();
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
bool Round017_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoardSp = Sprite::createWithSpriteFrameName(BOARD_PNG_FILE);
    m_pBoardSp->setPosition(BOARD_POS);
    this->addChild(m_pBoardSp, BOARD_ZORDER);
    
    m_pArrowsSp = Sprite::createWithSpriteFrameName(ARROWS_PNG_FILE);
    m_pArrowsSp->setPosition(ARROWS_POS);
    this->addChild(m_pArrowsSp, ARROWS_ZORDER);
    m_pArrowsSp->setOpacity(0);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK05-3-DA%02ld.png",i+1);
        m_pAnswerRopes[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswerRopes[i]->setUserData((void*)i);
        this->addChild(m_pAnswerRopes[i], ROPE_ZORDER);
        m_pAnswerRopes[i]->setScale(0.0f);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswerRopes[all]->setPosition(m_answerRopesPos[all]);
        all++;
    } /*while*/
    
    
    return true;
}

// 进入本回合
void Round017_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round017_03::changeScaleAnswersSchedule), 0.5f, -1, 1.1f);
    
}

//依次放大答案，以提示
void Round017_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round017_03::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswerRopes[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}

/* 开启触摸 */
void Round017_03::openTouch()
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
                fadeOutAnswers();                   //隐藏答案
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    //答对时主角说话
                    playLeadAnsCorEff();
                }else
                {
                    //答对时主角说话
                    playLeadAnsCorEff();
                }
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswerRopes[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }else
            {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    //答错时主角说话
                    playLeadAnsErrEff();
                }else
                {
                    //答错时主角说话
                    playLeadAnsErrEff();
                }
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswerRopes[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswerRopes[i]);
    } /*for*/
    
}

//答对后隐藏剩余答案
void Round017_03::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswerRopes[i]->runAction(FadeOut::create(0.3f));
    }
}

/*
 *  庆祝
 */
void Round017_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

//移动箭头
void Round017_03::rightMoveArrows()
{
    auto repeat = RepeatForever::create(Sequence::create(Spawn::create(FadeIn::create(0.4f),MoveBy::create(1.0f, Vec2(693.0f, 0.0f)), NULL),FadeOut::create(0.2f),CallFunc::create([this]{
        m_pArrowsSp->setPosition(ARROWS_POS);
    }), NULL));
    m_pArrowsSp->runAction(repeat);
    
    scheduleOnce([this](float){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 2.0f, "delaytime");
}

/*  回合结束回调 */
void Round017_03::endRound()
{
    rightMoveArrows();
}
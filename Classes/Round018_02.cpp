//
//  Round018_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round018_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "CustomMessage.h"
#include "NpcData.h"
#include "NpcDataManager.h"

#define CELEBRATE_DELAY                 5.0f
#define BEANS_ZORDER                    1

/*
 *  constructor
 */
Round018_02::Round018_02()
:m_InitAniIndex(0)
{
    m_beansPos[0] = Vec2(757, 524);
    m_beansPos[1] = Vec2(988, 423);
    m_beansPos[2] = Vec2(1201, 544);
    m_beansPos[3] = Vec2(1432, 423);
    m_beansPos[4] = Vec2(1650, 544);
}

/*
 *  destructor
 */
Round018_02::~Round018_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}



//
Round018_02* Round018_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round018_02();
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
bool Round018_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 5; i++) {
        sprintf(buffer, "XQ-GK06-2-DA%02ld.png",i+1);
        m_pBeansSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBeansSp[i]->setUserData((void*)i);
        this->addChild(m_pBeansSp[i], BEANS_ZORDER);
        m_pBeansSp[i]->setOpacity(0);
    }
    
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pBeansSp[all]->setPosition(m_beansPos[randIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round018_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round018_02::downFadeInSchedule), 1.1f, -1, 0);
}

//下降渐变出现
void Round018_02::downFadeInSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round018_02::downFadeInSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pBeansSp[m_InitAniIndex]->runAction(Sequence::create(Spawn::create(FadeIn::create(0.4f),MoveBy::create(0.4f, Vec2(0.0f, -200.0f)), NULL),CallFunc::create([this]{
        m_pBeansSp[m_InitAniIndex]->setScaleX(1.1f);
        m_pBeansSp[m_InitAniIndex]->setScaleY(0.9f);
    }),DelayTime::create(0.2f),CallFunc::create([this]{
        m_pBeansSp[m_InitAniIndex]->setScaleX(1.0f);
        m_pBeansSp[m_InitAniIndex]->setScaleY(1.0f);
        m_InitAniIndex++;
    }), NULL));
}




/* 开启触摸 */
void Round018_02::openTouch()
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
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pBeansSp[i]);
                } /*for*/
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pBeansSp[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pBeansSp[i]);
    } /*for*/
    
}

//上升渐变隐藏
void Round018_02::upFadeOutSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round018_02::upFadeOutSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pBeansSp[m_InitAniIndex++]->runAction(Spawn::create(FadeOut::create(0.4f),MoveBy::create(0.4f, Vec2(0.0f, 200.0f)), NULL));
}

/*
 *  庆祝
 */
void Round018_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round018_02::endRound()
{
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round018_02::upFadeOutSchedule), 0.4f, -1, 0);
}
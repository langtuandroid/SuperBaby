//
//  Round048_02.cpp
//  superBaby
//
//  Created by Administrator on 15/8/6.
//
//

#include "Round048_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "AppConfig.h"
#include "NpcDataManager.h"
#include "NpcData.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define TOPIC_ZORDER                    1
#define TOPIC_BASKET_PNG_FILE           "GC-GK12-2-TM01.png"
#define TOPIC_PLATE_PNG_FILE            "GC-GK12-2-TM02.png"

/*
 *  constructor
 */
Round048_02::Round048_02()
{
    m_answersPos[0] = Vec2(585, 800);
    m_answersPos[1] = Vec2(965, 800);
    m_answersPos[2] = Vec2(1345, 800);
    
    m_topicsPos[0] = Vec2(867, 100);
    m_topicsPos[1] = Vec2(1274, 100);
    m_topicsPos[2] = Vec2(1597, 100);
    
}

/*
 *  destructor
 */
Round048_02::~Round048_02()
{
    cleanUp();
}

//
void Round048_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round048_02* Round048_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round048_02();
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
bool Round048_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK12-2-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    unsigned int index = 5;
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK12-2-TM%02d.png",index);
        m_pTopicsImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pTopicsImageView[i]->setPosition(m_topicsPos[i]);
        m_pTopicsImageView[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(m_pTopicsImageView[i],TOPIC_ZORDER);
        index--;
    }
    
    return true;
}

// 进入本回合
void Round048_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round048_02::changeScaleAnswersSchedule), 0.4f, -1, 0);   //
}

//放大显示答案
void Round048_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round048_02::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round048_02::changeScaleTopicsSchedule), 1.2f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

void Round048_02::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round048_02::changeScaleTopicsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicsImageView[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round048_02::openTouch()
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
            target->setLocalZOrder(10);
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        int bubbleIndex = 0;
        Rect rect = Rect(0.0f, 0.0f, m_pTopicsImageView[0]->getContentSize().width, m_pTopicsImageView[0]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsImageView[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicsImageView[1]->getContentSize().width, m_pTopicsImageView[1]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsImageView[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicsImageView[2]->getContentSize().width, m_pTopicsImageView[2]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsImageView[2]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
                bubbleIndex = 2;
            }
        }
        if (isCorrect)
        {
            m_curCorrectNum++;
            if (bubbleIndex == 0) {
                m_pTopicsImageView[bubbleIndex]->loadTexture(TOPIC_BASKET_PNG_FILE,ui::Widget::TextureResType::PLIST);
            } else if (bubbleIndex == 1)
            {
                m_pTopicsImageView[bubbleIndex]->loadTexture(TOPIC_PLATE_PNG_FILE,ui::Widget::TextureResType::PLIST);
            }
            target->runAction(FadeOut::create(0.2f));
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            
            if (m_curCorrectNum == 3) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round048_02::fadeOutAnswers), 0.5f);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            } else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }
        } else
        {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round048_02::fadeOutAnswers(float)
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pTopicsImageView[i]->runAction(Spawn::create(FadeOut::create(0.5f),ScaleTo::create(0.5f, 0.0f), NULL));
    }
}

/*
 *  庆祝
 */
void Round048_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round048_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

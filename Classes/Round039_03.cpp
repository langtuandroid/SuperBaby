//
//  Round039_03.cpp
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#include "Round039_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define BOTTLE_ZORDER                   1
#define MARK_ZORDER                     2
#define BOTTLE_PNG_FILE                 "GC-GK03-3-TM-D.png"
#define BOTTLE_END_PNG_FILE             "GC-GK03-3-TM-C.png"
#define BOTTLE_POS                      Vec2(880,761)

/*
 *  constructor
 */
Round039_03::Round039_03()
{
    m_answersPos[0] = Vec2(884, -200);
    m_answersPos[1] = Vec2(1084, -200);
    m_answersPos[2] = Vec2(1284, -200);
    
    m_marksPos[0] = Vec2(1290, 1020);
    m_marksPos[1] = Vec2(1310, 756);
    m_marksPos[2] = Vec2(1305, 510);
}

/*
 *  destructor
 */
Round039_03::~Round039_03()
{
    cleanUp();
}

//
void Round039_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round039_03* Round039_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round039_03();
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
bool Round039_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBottlesImageView = ImageView::create(BOTTLE_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pBottlesImageView->setPosition(BOTTLE_POS);
    m_pBottlesImageView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    this->addChild(m_pBottlesImageView, BOTTLE_ZORDER);
    
    char buffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK03-3-TM02.png");
        m_pMarksSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pMarksSp[i]->setPosition(m_marksPos[i]);
        this->addChild(m_pMarksSp[i], MARK_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK03-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setScale(0.6f);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y+350);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round039_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round039_03::upAnswersSchedule), 0.4f, -1, 0);   //
}

//上升答案
void Round039_03::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_03::upAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round039_03::changeScaleMarksSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(MoveBy::create(0.4f, Vec2(0.0f, 350)));
}

void Round039_03::changeScaleMarksSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_03::changeScaleMarksSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pMarksSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round039_03::openTouch()
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
            target->setLocalZOrder(10);
            target->setScale(0.7f);
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        long index = (long)target->getUserData();
        target->setScale(0.6f);
        
        bool isCorrect = false;
        unsigned int topicNum = 0;
        Rect rect = Rect(0.0f, 0.0f,
                         m_pMarksSp[0]->getContentSize().width,
                         m_pMarksSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pMarksSp[0]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 2)
            {
                topicNum = 0;
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pMarksSp[1]->getContentSize().width,
                    m_pMarksSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pMarksSp[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 0)
            {
                topicNum = 1;
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pMarksSp[2]->getContentSize().width,
                    m_pMarksSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pMarksSp[2]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 1)
            {
                topicNum = 2;
                isCorrect = true;
            }
        }
        
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pMarksSp[topicNum]->getPosition());
            m_pMarksSp[topicNum]->setVisible(false);
            if (m_curCorrectNum < 3) {
                target->runAction(Sequence::create(ScaleTo::create(0.3f, 0.7f),ScaleTo::create(0.15f, 0.6f), NULL));
            }
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (m_curCorrectNum == 3) {
                m_pBottlesImageView->loadTexture(BOTTLE_END_PNG_FILE,ui::Widget::TextureResType::PLIST);
                fadeOutAnswers();
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }else
            {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }
        }else
        {
            // 播放答错音效
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

void Round039_03::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswersSp[i]->setVisible(false);
    }
}

/*
 *  庆祝
 */
void Round039_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round039_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
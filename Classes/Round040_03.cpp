//
//  Round040_03.cpp
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#include "Round040_03.h"
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
#define HINT_ZORDER                     1
#define HINT_LIGHT_PNG_FILE             "GC-GK04-3-TM08.png"
#define HINT_POS                        Vec2(1465,1133)
#define TOPIC_ZORDER                    1
#define ANSWER_POS                      Vec2(1465, 971)

/*
 *  constructor
 */
Round040_03::Round040_03()
{
    m_answersPos[0] = Vec2(881, 300);
    m_answersPos[1] = Vec2(1122, 300);
    m_answersPos[2] = Vec2(1363, 300);
    
    m_topicsPos[0] = Vec2(456, 1238);
    m_topicsPos[1] = Vec2(641, 1202);
    m_topicsPos[2] = Vec2(840, 1166);
    m_topicsPos[3] = Vec2(1065, 1141);
    m_topicsPos[4] = Vec2(1267, 1124);
    m_topicsPos[5] = Vec2(1465, 1133);
    m_topicsPos[6] = Vec2(1678, 1147);
}

/*
 *  destructor
 */
Round040_03::~Round040_03()
{
    cleanUp();
}

//
void Round040_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round040_03* Round040_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round040_03();
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
bool Round040_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pHintImageView = ImageView::create(HINT_LIGHT_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pHintImageView->setPosition(HINT_POS);
    m_pHintImageView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    this->addChild(m_pHintImageView, HINT_ZORDER);
    m_pHintImageView->setVisible(false);
    
    char buffer[64];
    for (unsigned int i = 0; i < 7; i++) {
        sprintf(buffer, "GC-GK04-3-TM%02d.png",i+1);
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_topicsPos[i]);
        m_pTopicsSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        this->addChild(m_pTopicsSp[i], TOPIC_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK04-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
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
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round040_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round040_03::changeScaleAnswersSchedule), 0.6f, -1, 0);   //
}
//改变答案大小
void Round040_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round040_03::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round040_03::changeScaleTopicsSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

void Round040_03::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 7) {
        unschedule(CC_SCHEDULE_SELECTOR(Round040_03::changeScaleTopicsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    if (m_InitAniIndex == 5) {
        m_pHintImageView->setVisible(true);
        m_pHintImageView->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
        m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
            m_pHintImageView->setVisible(false);
        }), NULL));
    }else
    {
        m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
    }
}

/* 开启触摸 */
void Round040_03::openTouch()
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
        auto touchPoint = m_pTopicsSp[5]->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pTopicsSp[5]->getContentSize().width, m_pTopicsSp[5]->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 1) {
                target->setPosition(ANSWER_POS);
                m_pTopicsSp[5]->setVisible(false);
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                fadeOutAnswers();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
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

void Round040_03::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswersSp[i]->setVisible(false);
        }
    }
}

/*
 *  庆祝
 */
void Round040_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round040_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
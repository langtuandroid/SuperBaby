//
//  Round042_02.cpp
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#include "Round042_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  3
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define MS_GAF_FILE                     "res/stage042/GC-GK06-1-MS/GC-GK06-1-MS.gaf"
#define MS_POS                          Vec2(1428,1533)
#define MS_ZORDER                       2
#define TOPIC_ZORDER                    1
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define HINT_PNG_FILE                   "GC-GK06-1-TM01.png"
#define HINT_POS                        Vec2(1541,1068)
#define HINT_ZORDER                     2

/*
 *  constructor
 */
Round042_02::Round042_02()
{
    m_answersPos[0] = Vec2(968, -100);
    m_answersPos[1] = Vec2(1240, -100);
    m_answersPos[2] = Vec2(1512, -100);
    
    m_topicsPos[0] = Vec2(846, 1068);
    m_topicsPos[1] = Vec2(1020, 1068);
    m_topicsPos[2] = Vec2(1194, 1068);
    m_topicsPos[3] = Vec2(1367, 1068);
}

/*
 *  destructor
 */
Round042_02::~Round042_02()
{
    cleanUp();
}

//
void Round042_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

//
Round042_02* Round042_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round042_02();
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
bool Round042_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafFile(MS_GAF_FILE);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    char buffer[64];
    for (unsigned int i = 0; i < 4; i++) {
        if (i == 0) {
            sprintf(buffer, "SZ2-07.png");
        } else if (i == 1)
        {
            sprintf(buffer, "GC-GK06-3-B.png");
        } else if (i == 2)
        {
            sprintf(buffer, "SZ2-01.png");
        } else
        {
            sprintf(buffer, "GC-GK06-3-A.png");
        }
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicsSp[i],TOPIC_ZORDER);
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp,HINT_ZORDER);
    
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 0) {
            sprintf(buffer, "SZ2-%02ld.png",i+6);
        } else
        {
            sprintf(buffer, "SZ2-%02ld.png",i+7);
        }
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
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
        m_answersEndPos[all] = Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y+300);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round042_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round042_02::upAnswersSchedule), 0.4f, -1, 0);   //
    
}

//上升答案以显示
void Round042_02::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round042_02::upAnswersSchedule));
        changeScaleHint();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, 300.0f))));
}

void Round042_02::changeScaleHint()
{
    m_pHintSp->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round042_02::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintSp->getContentSize().width,
                         m_pHintSp->getContentSize().height);
        if (rect.containsPoint(m_pHintSp->convertTouchToNodeSpace(touch)))
        {
            if (index == 1) {   //答对
                target->setPosition(m_pHintSp->getPosition());
                m_pHintSp->setVisible(false);
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
            } else {
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

void Round042_02::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswersSp[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

/*
 *  庆祝
 */
void Round042_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round042_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


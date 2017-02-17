//
//  Round047_03.cpp
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#include "Round047_03.h"
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
#define HINT_PNG_FILE                   "GC-GK11-3-WH.png"
#define HINT_POS                        Vec2(1393,694)
#define HINT_ZORDER                     2
#define MS_GAF_CONFIG_FILE              "res/stage047/ms_02.json"
#define MS_GAF_FILE                     "res/stage047/GC-GK11-3-MS-D+C/GC-GK11-3-MS-D+C.gaf"
#define MS_POS                          Vec2(1023,264)
#define MS_ZORDER                       1
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画

/*
 *  constructor
 */
Round047_03::Round047_03()
{
    m_answersPos[0] = Vec2(464, 1146);
    m_answersPos[1] = Vec2(878, 1146);
    m_answersPos[2] = Vec2(1292, 1146);
}

/*
 *  destructor
 */
Round047_03::~Round047_03()
{
    cleanUp();
}

//
Round047_03* Round047_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round047_03();
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
bool Round047_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK11-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setScale(0.0f);
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
        m_anwersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);

    return true;
}

// 进入本回合
void Round047_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round047_03::ChangeScaleAnswersSchedule), 1.5f, -1, 0);   //
}

//改变答案的大小
void Round047_03::ChangeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round047_03::ChangeScaleAnswersSchedule));
        changeScaleHint();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

void Round047_03::changeScaleHint()
{
    m_pHintSp->runAction(Sequence::create(ScaleTo::create(0.3f, 0.9f),
                                          ScaleTo::create(0.3f, 1.0f),
                                          ScaleTo::create(0.3f, 0.9f),
                                          ScaleTo::create(0.3f, 1.0f),
                                          CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

//
void Round047_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

/* 开启触摸 */
void Round047_03::openTouch()
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
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp->getContentSize().width, m_pHintSp->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 2) {
                target->setPosition(m_pHintSp->getPosition());
                target->setVisible(false);
                m_pHintSp->setVisible(false);
                changeActor();
                changeScaleAnswers();
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_anwersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_anwersEndPos[index]);
                }), NULL));
            }
        } else
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_anwersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_anwersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round047_03::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER,true);
}

void Round047_03::changeScaleAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 2) {
            m_pAnswersSp[i]->runAction(ScaleTo::create(0.3f, 0.0f));
        }
    }
}

/*
 *  庆祝
 */
void Round047_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/*  回合结束回调 */
void Round047_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
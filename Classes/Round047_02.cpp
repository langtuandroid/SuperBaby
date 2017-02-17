//
//  Round047_02.cpp
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#include "Round047_02.h"
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
#define MS_GAF_CONFIG_FILE              "res/stage047/ms_01.json"
#define MS_GAF_FILE                     "res/stage047/GC-GK11-2-MS-D+C1+C2/GC-GK11-2-MS-D+C1+C2.gaf"
#define MS_POS                          Vec2(858,338)
#define MS_ZORDER                       1
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGERONE                      "triggerone"                                 // 触发动画
#define TRIGGERTWO                      "triggertwo"                                 // 触发动画
#define HINT_PNG_FILE                   "GC-GK11-2-WH.png"
#define HINT_ZORDER                     2

/*
 *  constructor
 */
Round047_02::Round047_02()
{
    m_answersPos[0] = Vec2(836, 200);
    m_answersPos[1] = Vec2(1026, 200);
    m_answersPos[2] = Vec2(1216, 200);
    
    m_hintsPos[0] = Vec2(886, 1048);
    m_hintsPos[1] = Vec2(1194, 992);
    
}

/*
 *  destructor
 */
Round047_02::~Round047_02()
{
    cleanUp();
}

//
void Round047_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

//
Round047_02* Round047_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round047_02();
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
bool Round047_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK11-2-DA%02ld.png",i+1);
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
        m_randNum[randIndex] = all;
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    for (unsigned long i = 0; i < 2; i++) {
        m_pHintsSp[i] = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
        m_pHintsSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHintsSp[i],HINT_ZORDER);
    }

    return true;
}

// 进入本回合
void Round047_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round047_02::changeScaleAnswersSchedule), 0.4f, -1, 0);   //
}

//放大显示答案
void Round047_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round047_02::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round047_02::changeScaleHintsSchedule), 1.2f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_randNum[m_InitAniIndex++]]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

void Round047_02::changeScaleHintsSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round047_02::changeScaleHintsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pHintsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),
                                                             ScaleTo::create(0.3f, 1.0f),
                                                             ScaleTo::create(0.3f, 1.1f),
                                                             ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round047_02::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f, m_pHintsSp[0]->getContentSize().width+20, m_pHintsSp[0]->getContentSize().height+20);
        if (rect.containsPoint(m_pHintsSp[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pHintsSp[1]->getContentSize().width+20, m_pHintsSp[1]->getContentSize().height+20);
        if (rect.containsPoint(m_pHintsSp[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }
        if (isCorrect)
        {
            m_curCorrectNum++;
            target->setPosition(m_pHintsSp[bubbleIndex]->getPosition());
            m_pHintsSp[bubbleIndex]->setVisible(false);
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            
            if (m_curCorrectNum == 2) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                changeActor();
                for (unsigned int i = 0; i < 3; i++) {
                        m_pAnswersSp[i]->setVisible(false);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[1]);
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

void Round047_02::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGERTWO,true);
}

/*
 *  庆祝
 */
void Round047_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round047_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

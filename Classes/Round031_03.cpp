//
//  Round031_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round031_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(1218,1068)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define ANSWERS_ZORDER                  3
#define HINTS_ZORDER                    2
#define PLATFORM_ZORDER                 1
#define PLATFORM_PNG_FILE               "HY-GK07-3-TM01.png"
#define PLATFORM_HIGHLIGHT_PNG_FILE     "HY-GK07-3-TM08.png"
#define PLATFORM_POS                    Vec2(1150,459)
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round031_03::Round031_03() {
    
    m_hintsPos[0] = Vec2(1194, 848);
    m_hintsPos[1] = Vec2(839, 577);
    m_hintsPos[2] = Vec2(1437, 537);
    
    m_answersPos[0] = Vec2(568, 883);
    m_answersPos[1] = Vec2(1048, 1175);
    m_answersPos[2] = Vec2(1579, 1147);
}

/*
 *  destructor
 */
Round031_03::~Round031_03()
{
    cleanUp();
}

//
void Round031_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round031_03* Round031_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round031_03();
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
bool Round031_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    m_pPlatformSp = Sprite::createWithSpriteFrameName(PLATFORM_PNG_FILE);
    m_pPlatformSp->setPosition(PLATFORM_POS);
    this->addChild(m_pPlatformSp, PLATFORM_ZORDER);
    
    m_pHighLightPlatformSp = Sprite::createWithSpriteFrameName(PLATFORM_HIGHLIGHT_PNG_FILE);
    m_pHighLightPlatformSp->setPosition(PLATFORM_POS);
    this->addChild(m_pHighLightPlatformSp, PLATFORM_ZORDER);
    m_pHighLightPlatformSp->setVisible(false);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK07-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK07-3-TM%02ld.png",i+2);
        m_pHintsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pHintsSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHintsSp[i], HINTS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK07-3-TM%02ld.png",i+5);
        m_pHighLightHintSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pHighLightHintSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHighLightHintSp[i], HINTS_ZORDER);
        m_pHighLightHintSp[i]->setVisible(false);
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
void Round031_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round031_03::changeScaleHintsSchedule), 1.2f, -1, 0);
}

//改变提示大小
void Round031_03::changeScaleHintsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round031_03::changeScaleHintsSchedule));
        changePlatform();
        return;
    }
    m_pHintsSp[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),
                                                            CallFunc::create([this]{
        m_pHintsSp[m_InitAniIndex]->setVisible(false);
        m_pHighLightHintSp[m_InitAniIndex]->setVisible(true);
    }),
                                                            DelayTime::create(0.3f),
                                                            CallFunc::create([this]{
        m_pHighLightHintSp[m_InitAniIndex]->setVisible(false);
        m_pHintsSp[m_InitAniIndex]->setVisible(true);
    }),
                                                            ScaleTo::create(0.4f, 1.0f),
                                                            CallFunc::create([this]{
        m_InitAniIndex++;
    }),NULL));
}

void Round031_03::changePlatform()
{
    m_pPlatformSp->runAction(Sequence::create(ScaleTo::create(0.4f, 1.03f),
                                                           CallFunc::create([this]{
        m_pPlatformSp->setVisible(false);
        m_pHighLightPlatformSp->setVisible(true);
    }),
                                                           DelayTime::create(0.3f),
                                                           CallFunc::create([this]{
        m_pHighLightPlatformSp->setVisible(false);
        m_pPlatformSp->setVisible(true);
    }),
                                                           ScaleTo::create(0.4f, 1.0f),
                                                           CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }),NULL));
}

/* 开启触摸 */
void Round031_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setLocalZOrder(5);
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pHintsSp[0]->getContentSize().width, m_pHintsSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pHintsSp[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pHintsSp[1]->getContentSize().width, m_pHintsSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pHintsSp[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pHintsSp[2]->getContentSize().width, m_pHintsSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pHintsSp[2]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
            }
        }
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pHintsSp[index]->getPosition());
            m_pHintsSp[index]->runAction(FadeOut::create(0.3f));
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 3) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }
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

/*
 *  庆祝
 */
void Round031_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round031_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
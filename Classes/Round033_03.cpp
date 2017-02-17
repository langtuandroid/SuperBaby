//
//  Round033_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#include "Round033_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(818,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(518,868)
#define WHITELIGHT_GAF_ZORDER           1
#define KITS_ZORDER                     3
#define BOARD_ZORDER                    2
#define BOARD_POS                       Vec2(1033,839)

/*
 *  constructor
 */
Round033_03::Round033_03() {
    
    m_answersPos[0] = Vec2(744, -133);
    m_answersPos[1] = Vec2(1034, -133);
    m_answersPos[2] = Vec2(1318, -133);
    
    m_upKitsPos[0] = Vec2(648, 1117);
    m_upKitsPos[1] = Vec2(894, 1117);
    m_upKitsPos[2] = Vec2(1138, 1117);
    
    m_downKitsPos[0] = Vec2(648, 813);
    m_downKitsPos[1] = Vec2(894, 813);
    m_downKitsPos[2] = Vec2(1138, 813);
    m_downKitsPos[3] = Vec2(1382, 813);
}

/*
 *  destructor
 */
Round033_03::~Round033_03()
{
    cleanUp();
}

//
void Round033_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round033_03* Round033_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round033_03();
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
bool Round033_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    m_pWhiteLightActor->setPosAtNode(Vec2(-300, 400));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    char buffer[64];
    sprintf(buffer, "HY-GK09-3-TM2.png");
    m_pBoardSp = Sprite::createWithSpriteFrameName(buffer);
    m_pBoardSp->setPosition(BOARD_POS);
    this->addChild(m_pBoardSp, BOARD_ZORDER);
    
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK09-3-TM1.png");
        m_pUpKitsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pUpKitsSp[i]->setPosition(m_upKitsPos[i]);
        this->addChild(m_pUpKitsSp[i], KITS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(buffer, "HY-GK09-3-TM1.png");
        m_pDownKitsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pDownKitsSp[i]->setPosition(m_downKitsPos[i]);
        this->addChild(m_pDownKitsSp[i], KITS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ4-%02ld.png",i+6);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
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
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round033_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round033_03::upAnswersSchedule), 0.5f, -1, 0);   //
}

//上升答案
void Round033_03::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round033_03::upAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round033_03::changeColorSchedule), 0.3f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, 600))));
}

void Round033_03::changeColorSchedule(float)
{
    if (m_InitAniIndex >= 8) {
        unschedule(CC_SCHEDULE_SELECTOR(Round033_03::changeColorSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    if (m_InitAniIndex < 4) {
        if (m_InitAniIndex % 2 == 0) {
            for (unsigned int i = 0; i < 3; i++) {
                m_pUpKitsSp[i]->runAction(CallFunc::create([this,i]{
                    m_pUpKitsSp[i]->setColor(Color3B(195, 164, 42));
                }));
            }
        }else if(m_InitAniIndex % 2 == 1)
        {
            for (unsigned int i = 0; i < 3; i++) {
                m_pUpKitsSp[i]->runAction(CallFunc::create([this,i]{
                    m_pUpKitsSp[i]->setColor(Color3B(255, 255, 255));
                }));
            }
        }
    }else
    {
        if (m_InitAniIndex % 2 == 0) {
            for (unsigned int i = 0; i < 4; i++) {
                m_pDownKitsSp[i]->runAction(CallFunc::create([this,i]{
                    m_pDownKitsSp[i]->setColor(Color3B(195, 164, 42));
                }));
            }
        }else if(m_InitAniIndex % 2 == 1)
        {
            for (unsigned int i = 0; i < 4; i++) {
                m_pDownKitsSp[i]->runAction(CallFunc::create([this,i]{
                    m_pDownKitsSp[i]->setColor(Color3B(255, 255, 255));
                }));
            }
        }
    }

    m_InitAniIndex++;
}

/* 开启触摸 */
void Round033_03::openTouch()
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
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height+100);
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {   //答对
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
                m_InitAniIndex = 0;
                this->schedule(CC_SCHEDULE_SELECTOR(Round033_03::downAnswersSchedule), 1.0f, -1, 0);   //
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
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
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round033_03::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round033_03::downAnswersSchedule));
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(1.0f, Vec2(0.0f, -600))));
}

/*
 *  庆祝
 */
void Round033_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round033_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
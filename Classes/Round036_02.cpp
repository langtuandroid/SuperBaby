//
//  Round036_02.cpp
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#include "Round036_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(1118,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define LEG_GAF_CONFIG_FILE             "res/stage036/leg.json"
#define LEG_GAF_FILE                    "res/stage036/HY-GK12-4-IT-D1+C1+C2/HY-GK12-4-IT-D1+C1+C2.gaf"
#define LEG_GAF_POS                     Vec2(933,708)
#define LEG_ZORDER                      3
#define LEGS_ZORDER                     4
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                   // 触发动画
/*
 *  constructor
 */
Round036_02::Round036_02()
{
    
    m_legsPos[0] = Vec2(701, 474);
    m_legsPos[1] = Vec2(1161, 474);
    m_legsPos[2] = Vec2(1606, 474);
    
}

/*
 *  destructor
 */
Round036_02::~Round036_02()
{
    cleanUp();
}

//
void Round036_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(LEG_GAF_FILE);
}

//
Round036_02* Round036_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round036_02();
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
bool Round036_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    m_pLegActor = Actor::createWithGafConfigFile(LEG_GAF_CONFIG_FILE);
    m_pLegActor->playWithSequenceAliasName(STANDBY);
    m_pLegActor->setPosition(LEG_GAF_POS);
    this->addChild(m_pLegActor, LEG_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK12-2-DA%02ld.png",i+1);
        m_pLegsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pLegsSp[i]->setUserData((void*)i);
        this->addChild(m_pLegsSp[i], LEGS_ZORDER);
        m_pLegsSp[i]->setScale(0.0f);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pLegsSp[all]->setPosition(m_legsPos[randIndex]);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round036_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round036_02::changeMaxScaleLegsSchedule), 0.4f, -1, 0);   //
}

//依次放大答案以提示
void Round036_02::changeMaxScaleLegsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_02::changeMaxScaleLegsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pLegsSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

/* 开启触摸 */
void Round036_02::openTouch()
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
        auto index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
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
                celebrate(1.5f);
                changeActor();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pLegsSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pLegsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pLegsSp[i]);
    } /*for*/
}

//依次缩小大腿
void Round036_02::changeSmallScaleLegsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_02::changeSmallScaleLegsSchedule));
        m_pLegActor->setVisible(false);
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pLegsSp[m_InitAniIndex++]->runAction(EaseBackIn::create(ScaleTo::create(0.4f, 0.0f)));
}

//答对后改变actor
void Round036_02::changeActor()
{
    m_pLegActor->playWithSequenceAliasName(TRIGGER);
}

/*
 *  庆祝
 */
void Round036_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round036_02::endRound()
{
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round036_02::changeSmallScaleLegsSchedule), 0.4f, -1, 0);   //
}
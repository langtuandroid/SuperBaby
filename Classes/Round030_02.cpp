//
//  Round030_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round030_02.h"
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

#define ANSWERS_ZORDER                 4
#define SEAHORSE_ZORDER                1
#define STICKS_ZORDER                  3
#define RULERS_ZORDER                  2

/*
 *  constructor
 */
Round030_02::Round030_02()
{
    
    m_answersPos[0] = Vec2(644, 1119);
    m_answersPos[1] = Vec2(1073, 1119);
    m_answersPos[2] = Vec2(1502, 1119);
    
    m_seaHorsesPos[0] = Vec2(782, 645);
    m_seaHorsesPos[1] = Vec2(1200, 645);
    m_seaHorsesPos[2] = Vec2(1659, 645);
    
    m_sticksPos[0][0] = Vec2(569, 805);
    m_sticksPos[0][1] = Vec2(569, 965);
    m_sticksPos[0][2] = Vec2(569, 935);
    
    m_sticksPos[1][0] = Vec2(998, 805);
    m_sticksPos[1][1] = Vec2(998, 965);
    m_sticksPos[1][2] = Vec2(998, 935);
    
    m_sticksPos[2][0] = Vec2(1427, 805);
    m_sticksPos[2][1] = Vec2(1427, 965);
    m_sticksPos[2][2] = Vec2(1427, 935);
    
    m_rulersPos[0] = Vec2(569, 825);
    m_rulersPos[1] = Vec2(998, 825);
    m_rulersPos[2] = Vec2(1427, 825);
    
}

/*
 *  destructor
 */
Round030_02::~Round030_02()
{
    cleanUp();
}

//
void Round030_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round030_02* Round030_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round030_02();
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
bool Round030_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setPosition(m_answersPos[i]);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-1-DA%02ld.png",i+4);
        m_pHighLightAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pHighLightAnswersSp[i]->setPosition(m_answersPos[i]);
        this->addChild(m_pHighLightAnswersSp[i], ANSWERS_ZORDER);
        m_pHighLightAnswersSp[i]->setVisible(false);
        m_pHighLightAnswersSp[i]->setScale(1.1f);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-1-MS01.png");
        m_pSeaHorsesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pSeaHorsesSp[i],SEAHORSE_ZORDER);
        if (i == 2) {
            m_pSeaHorsesSp[i]->setRotation(-20);
        }
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-1-TM01.png");
        m_pRulersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pRulersSp[i], RULERS_ZORDER);
        m_pRulersSp[i]->setPosition(m_rulersPos[i]);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-1-TM02.png");
        m_pSticksSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pSticksSp[i], RULERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pSeaHorsesSp[all]->setPosition(m_seaHorsesPos[randIndex]);
        m_pSticksSp[all]->setPosition(m_sticksPos[randIndex][all]);
        if (all == 1) {
            m_curRandNum = randIndex;
        }
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round030_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round030_02::changeScaleAnswersSchedule), 1.25f, -1, 0);   //
}

//依次放大答案以提示
void Round030_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round030_02::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.5f, 1.28f),CallFunc::create([this]{
        m_pHighLightAnswersSp[m_InitAniIndex]->setVisible(true);
    }),DelayTime::create(0.2f),CallFunc::create([this]{
        m_pHighLightAnswersSp[m_InitAniIndex]->setVisible(false);
    }),ScaleTo::create(0.5f, 1.0f),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
}

/* 开启触摸 */
void Round030_02::openTouch()
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
            if (index == m_curRandNum) {
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round030_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round030_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
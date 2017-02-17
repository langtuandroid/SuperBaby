//
//  Round017_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round017_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "AppConfig.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"

#define MS_GAF_FILE                "res/stage017/XQ-GK05-1-MS-D+C/XQ-GK05-1-MS-D+C.gaf"    // ms gaf
#define MS_GAF_POS                 Vec2(1545,290)
#define MS_GAF_ZORDER              1
#define MS_GAF_CONFIG_FILE         "config/stage017/ms.json"

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define STANDBY                    "standby"                                   // 待机动画
#define TRIGGER                    "trigger"                                    // 触发动画
#define NUMBERS_ZORDER             2
#define HINT_POS                   Vec2(1490,732)
#define HINT_ZORDER                3
#define HINT_PNG_FILE              "XQ-GK05-1-IT.png"
#define BN_PLIST_FILE              "res/stage017/XQ-GK05-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                "res/stage017/XQ-GK05-IT.png"               // BN的 png 文件
#define BRICK_PNG_FILE             "XQ-GK05-1-TM-C.png"
#define BRICK_POS                  Vec2(1390,632)
#define BRICK_ZORDER               4


/*
 *  constructor
 */
Round017_01::Round017_01(){
    
    m_answerNumbersPos[0] = Vec2(824, 192);
    m_answerNumbersPos[1] = Vec2(1090, 192);
    m_answerNumbersPos[2] = Vec2(1346, 192);
}

/*
 *  destructor
 */
Round017_01::~Round017_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

//
Round017_01* Round017_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round017_01();
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
bool Round017_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ6-%02ld.png",i+2);
        m_pAnswerNumbersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswerNumbersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswerNumbersSp[i], NUMBERS_ZORDER);
        m_pAnswerNumbersSp[i]->setScale(0.0f);
        m_pAnswerNumbersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        
    } /*for*/
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    m_pHintSp->setOpacity(0);
    
    m_pBrickSp = Sprite::createWithSpriteFrameName(BRICK_PNG_FILE);
    m_pBrickSp->setPosition(BRICK_POS);
    this->addChild(m_pBrickSp, BRICK_ZORDER);
    m_pBrickSp->setOpacity(0);
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    
    while (placed < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pAnswerNumbersSp[placed]->setPosition(m_answerNumbersPos[randIndex]);
        placed++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

//
void Round017_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round017_01::changeScaleNumbersSchedule), 0.4f, -1, 0);
    
    
}

//改变大小出现
void Round017_01::changeScaleNumbersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round017_01::changeScaleNumbersSchedule));
        fadeHint();
        return;
    }
    m_pAnswerNumbersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

//提示
void Round017_01::fadeHint()
{
    m_pHintSp->runAction(Sequence::create(FadeIn::create(0.4f),
                                          FadeOut::create(0.4f),
                                          FadeIn::create(0.4f),
                                          FadeOut::create(0.4f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round017_01::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        unsigned long index = (unsigned long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) /*答对*/ {
                //改变actor动作
                changeActor();
                scaleChangeNumbers();
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                this->celebrate(CELEBRATE_DELAY);
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    dispatcher->removeEventListenersForTarget(m_pAnswerNumbersSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pAnswerNumbersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pAnswerNumbersSp[i]);
    } /*for*/
}

//答对后隐藏答案
void Round017_01::scaleChangeNumbers()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswerNumbersSp[i]->runAction(ScaleTo::create(0.4f, 0.0f));
    }
}

//答对后改变actor动作
void Round017_01::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]
    {
        moveBrick();
    });
}

//答对后移动砖填补
void Round017_01::moveBrick()
{
    m_pBrickSp->runAction(Sequence::create(FadeIn::create(0.3f),MoveTo::create(0.3f, HINT_POS), NULL));
}

//
void Round017_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

/*  回合结束回调 */
void Round017_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


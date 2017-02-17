//
//  Round019_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#include "Round019_01.h"
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

#define STAGE_GAF_FILE              "res/stage019/XQ-GK07-1-dimian-D+C/XQ-GK07-1-dimian-D+C.gaf"    // stage gaf
#define STAGE_GAF_POS               Vec2(1364,23)
#define STAGE_GAF_ZORDER            1
#define STAGE_GAF_CONFIG_FILE       "config/stage019/stage.json"

#define PILLAR_GAF_FILE             "res/stage019/XQ-GK07-1-zhuzi/XQ-GK07-1-zhuzi.gaf"
#define PILLAR_GAF_POS              Vec2(355,1729)
#define PILLAR_GAFZ_ZORDER          2

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define STANDBY                    "standby"                                   // 待机动画
#define TRIGGER                    "trigger"                                    // 触发动画

#define BN_PLIST_FILE              "res/stage019/XQ-GK07-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                "res/stage019/XQ-GK07-IT.png"               // BN的 png 文件
#define GRAPHS_ZORDER              4
#define TARGET_GRAPHS_ZORDER       3
#define MOV_TIME                   0.2f                                        // 答错回原位置移动时间
#define REPEATACTION_TAG           100

#define NUMBER_OF_ANSWER_RIGHT     3                                            // 答题正确数量达到该数字怎本回合结束
/*
 *  constructor
 */
Round019_01::Round019_01(){
    
    m_graphsPos[0] = Vec2(933, 437);
    m_graphsPos[1] = Vec2(1202, 437);
    m_graphsPos[2] = Vec2(1471, 437);
    
    m_targetGraphsPos[0] = Vec2(819, 1000);
    m_targetGraphsPos[1] = Vec2(1333, 1029);
    m_targetGraphsPos[2] = Vec2(1823, 1065);
}

/*
 *  destructor
 */
Round019_01::~Round019_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(STAGE_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(PILLAR_GAF_FILE);
}

//
Round019_01* Round019_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round019_01();
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
bool Round019_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pStageActor = Actor::createWithGafConfigFile(STAGE_GAF_CONFIG_FILE);
    m_pStageActor->setPosition(STAGE_GAF_POS);
    m_pStageActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pStageActor, STAGE_GAF_ZORDER);
    
    m_pPillarActor = Actor::createWithGafFile(PILLAR_GAF_FILE);
    m_pPillarActor->setPosition(PILLAR_GAF_POS);
    this->addChild(m_pPillarActor, PILLAR_GAFZ_ZORDER);
    m_pPillarActor->setScale(1.05f);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK07-1-DA%02ld.png",i+1);
        m_pGraphsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pGraphsSp[i]->setUserData((void*)i);
        this->addChild(m_pGraphsSp[i], GRAPHS_ZORDER);
        RepeatForever* repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEATACTION_TAG);
        m_pGraphsSp[i]->runAction(repeat);
    } /*for*/
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK07-1-TM%02d.png",(int)i+1);
        m_pTargetGraphsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTargetGraphsSp[i]->setUserData((void*)i);
        this->addChild(m_pTargetGraphsSp[i], TARGET_GRAPHS_ZORDER);
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    
    while (placed < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pGraphsSp[placed]->setPosition(m_graphsPos[randIndex]);
        m_graphsEndPos[placed] = m_graphsPos[randIndex];
        placed++;
    } /*while*/
    
    allplaced[0] = false;
    allplaced[1] = false;
    allplaced[2] = false;
    placed = 0;
    while (placed < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pTargetGraphsSp[placed]->setPosition(m_targetGraphsPos[randIndex]);
        m_targetGraphsEndPos[placed] = m_targetGraphsPos[randIndex];
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
void Round019_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round019_01::changeScaleFlowersSchedule), 0.6f, -1, 0);
    
    
}

//改变大小出现
void Round019_01::changeScaleFlowersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round019_01::changeScaleFlowersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTargetGraphsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round019_01::openTouch()
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        unsigned long index = (unsigned long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pTargetGraphsSp[0]->getContentSize().width, m_pTargetGraphsSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pTargetGraphsSp[0]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && index == 0) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTargetGraphsSp[1]->getContentSize().width, m_pTargetGraphsSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pTargetGraphsSp[1]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && index == 1) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTargetGraphsSp[2]->getContentSize().width, m_pTargetGraphsSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pTargetGraphsSp[2]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && index == 2) {
                isCorrect = true;
            }
        }
        if (isCorrect) {
            target->stopActionByTag(REPEATACTION_TAG);
            target->setPosition(m_targetGraphsEndPos[index]);
            m_pTargetGraphsSp[index]->setVisible(false);
            m_curCorrectNumber++;
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_graphsEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_graphsEndPos[index]);
            }), NULL));
        }
        if (NUMBER_OF_ANSWER_RIGHT == m_curCorrectNumber) {
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            celebrate(CELEBRATE_DELAY);
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pGraphsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pGraphsSp[i]);
    } /*for*/
}

//答对后改变actor动作
void Round019_01::changeActor()
{
    m_pStageActor->playWithSequenceAliasName(TRIGGER);
    m_pStageActor->setAnimationFinishedPlayDelegate([this]
    {
        if (m_completeCB) {
            m_completeCB();
        }
    });
}

//渐隐消失所有答案
void Round019_01::fadeAll()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pGraphsSp[i]->runAction(FadeOut::create(0.3f));
    }
}

//
void Round019_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

/*  回合结束回调 */
void Round019_01::endRound()
{
    changeActor();
    fadeAll();
}

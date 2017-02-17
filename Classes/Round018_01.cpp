//
//  Round018_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round018_01.h"
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

#define MS_GAF_FILE                "res/stage018/XQ-GK06-1-MS-D+C/XQ-GK06-1-MS-D+C.gaf"    // ms gaf
#define MS_GAF_POS                 Vec2(1338,214)
#define MS_GAF_ZORDER              1
#define MS_GAF_CONFIG_FILE         "config/stage018/ms.json"

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define STANDBY                    "standby"                                   // 待机动画
#define TRIGGER                    "trigger"                                    // 触发动画

#define BN_PLIST_FILE              "res/stage018/XQ-GK06-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                "res/stage018/XQ-GK06-IT.png"               // BN的 png 文件
#define FLOWERS_ZORDER             2
/*
 *  constructor
 */
Round018_01::Round018_01(){
    
    m_flowersPos[0] = Vec2(978, 1156);
    m_flowersPos[1] = Vec2(987, 886);
    m_flowersPos[2] = Vec2(1260, 1132);
    m_flowersPos[3] = Vec2(1638, 904);
}

/*
 *  destructor
 */
Round018_01::~Round018_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

//
Round018_01* Round018_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round018_01();
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
bool Round018_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "XQ-GK06-1-TM%02ld.png",i+1);
        m_pFlowersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pFlowersSp[i]->setUserData((void*)i);
        this->addChild(m_pFlowersSp[i], FLOWERS_ZORDER);
    } /*for*/
    
    
    srand(unsigned(time(0)));
    bool allplaced[4] = {false, false, false, false};
    unsigned int placed = 0;
    
    while (placed < 4) {
        auto randIndex = static_cast<unsigned int>(rand() % 4);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pFlowersSp[placed]->setPosition(m_flowersPos[randIndex]);
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
void Round018_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round018_01::changeScaleFlowersSchedule), 0.6f, -1, 0);
    
    
}

//改变大小出现
void Round018_01::changeScaleFlowersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round018_01::changeScaleFlowersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pFlowersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round018_01::openTouch()
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
            if (index == 3) /*答对*/ {
                moveFlower();
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
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
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                this->celebrate(CELEBRATE_DELAY);
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 4; i++) {
                    dispatcher->removeEventListenersForTarget(m_pFlowersSp[i]);
                } /*for*/
            } else {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pFlowersSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pFlowersSp[i]);
    } /*for*/
}

//答对后移动花
void Round018_01::moveFlower()
{
    m_pFlowersSp[3]->runAction(Sequence::create(Spawn::create(MoveTo::create(1.0f, Vec2(1635, 441)),RotateBy::create(1.0f, 360), NULL),CallFunc::create([this]{
        m_pFlowersSp[3]->setVisible(false);
        changeActor();
    }), NULL));
}

//答对后改变actor动作
void Round018_01::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
}

//
void Round018_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

/*  回合结束回调 */
void Round018_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
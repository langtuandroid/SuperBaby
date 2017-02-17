//
//  Round020_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/20.
//
//

#include "Round020_01.h"
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

#define ROBOT_GAF_FILE              "res/stage020/XQ-GK08-1-MS-D+C/XQ-GK08-1-MS-D+C.gaf"    // stage gaf
#define ROBOT_GAF_POS               Vec2(1436,729)
#define ROBOT_GAF_ZORDER            1
#define ROBOT_GAF_CONFIG_FILE       "config/stage020/robotone.json"

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define STANDBY                    "standby"                                   // 待机动画
#define TRIGGER                    "trigger"                                    // 触发动画

#define BN_PLIST_FILE              "res/stage020/XQ-GK08-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                "res/stage020/XQ-GK08-IT.png"               // BN的 png 文件
#define PARTS_ZORDER               2
#define MOV_TIME                   0.2f                                        // 答错回原位置移动时间
#define REPEATACTION_TAG           100

/*
 *  constructor
 */
Round020_01::Round020_01(){
    
    m_partsPos[0] = Vec2(494, 927);
    m_partsPos[1] = Vec2(778, 927);
    m_partsPos[2] = Vec2(1062, 927);
    
    m_targetRect =Rect(1233, 646, 150, 300);
}

/*
 *  destructor
 */
Round020_01::~Round020_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(ROBOT_GAF_FILE);
}

//
Round020_01* Round020_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round020_01();
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
bool Round020_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pRobotActor = Actor::createWithGafConfigFile(ROBOT_GAF_CONFIG_FILE);
    m_pRobotActor->setPosition(ROBOT_GAF_POS);
    m_pRobotActor->playWithSequenceAliasName(STANDBY);
    this->addChild(m_pRobotActor, ROBOT_GAF_ZORDER);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK08-1-DA%02ld.png",i+1);
        m_pPartsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pPartsSp[i]->setUserData((void*)i);
        this->addChild(m_pPartsSp[i], PARTS_ZORDER);
        RepeatForever* repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEATACTION_TAG);
        m_pPartsSp[i]->runAction(repeat);
        m_pPartsSp[i]->setScale(0.0f);
    } /*for*/

    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    
    while (placed < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pPartsSp[placed]->setPosition(m_partsPos[randIndex]);
        m_partsEndPos[placed] = m_partsPos[randIndex];
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
void Round020_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round020_01::changeScalePartsSchedule), 0.5f, -1, 0);
    
    
}

//改变大小出现
void Round020_01::changeScalePartsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round020_01::changeScalePartsSchedule));
        m_pRobotActor->start();
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pPartsSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}

/* 开启触摸 */
void Round020_01::openTouch()
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
        if (m_targetRect.containsPoint(this->convertTouchToNodeSpace(touch))) {
            if (index == 0) {
                target->stopActionByTag(REPEATACTION_TAG);
                target->runAction(FadeOut::create(0.2f));
                changeActor();
                
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
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPartsSp[i]);
                } /*for*/
            }else
            {
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_partsEndPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_partsEndPos[index]);
                }), NULL));
            }
        }else
        {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_partsEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_partsEndPos[index]);
            }), NULL));
        }
     };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pPartsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pPartsSp[i]);
    } /*for*/
}

//答对后改变actor动作
void Round020_01::changeActor()
{
    m_pRobotActor->playWithSequenceAliasName(TRIGGER);
    m_pRobotActor->setAnimationFinishedPlayDelegate([this]
    {
        fadeAll();
    });
}

//渐隐消失所有答案
void Round020_01::fadeAll()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 0) {
            m_pPartsSp[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

//
void Round020_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round020_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
//
//  Round020_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/20.
//
//

#include "Round020_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "CustomMessage.h"
#include "NpcData.h"
#include "NpcDataManager.h"

#define CELEBRATE_DELAY             5.0f
#define PARTS_ZORDER                3
#define ROBOT_GAF_CONFIG_FILE       "config/stage020/robottwo.json"
#define ROBOT_GAF_POS               Vec2(990,1033)
#define ROBOT_GAF_ZORDER            1
#define ROBOT_GAF_FILE              "res/stage020/XQ-GK08-2-MS-D+C/XQ-GK08-2-MS-D+C.gaf"
#define STANDBY                     "standby"                                   // 待机动画
#define TRIGGER                     "trigger"                                    // 触发动画

#define TOPIC_ZORDER                2
#define MOV_TIME                   0.2f                                        // 答错回原位置移动时间
#define NUMBER_OF_ANSWER_RIGHT     2                                            // 答题正确数量达到该数字怎本回合结束
#define REPEATACTION_TAG           100
#define STAGE_PNG_FILE             "res/stage019/XQ-GK07-BGpingtai.png"

/*
 *  constructor
 */
Round020_02::Round020_02()
:m_InitAniIndex(0)
{
    m_partsPos[0] = Vec2(996, 319);
    m_partsPos[1] = Vec2(1238, 319);
    m_partsPos[2] = Vec2(1480, 319);
    
    m_targetRect = Rect(734, 811, 583, 811);
}

/*
 *  destructor
 */
Round020_02::~Round020_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(ROBOT_GAF_FILE);
}



//
Round020_02* Round020_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round020_02();
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
bool Round020_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pRobotActor = Actor::createWithGafConfigFile(ROBOT_GAF_CONFIG_FILE);
    m_pRobotActor->setPosition(ROBOT_GAF_POS);
    this->addChild(m_pRobotActor, ROBOT_GAF_ZORDER);
    m_pRobotActor->playWithSequenceAliasName(STANDBY);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK08-2-DA%02ld.png",i+1);
        m_pPartsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pPartsSp[i]->setUserData((void*)i);
        this->addChild(m_pPartsSp[i], PARTS_ZORDER);
        m_pPartsSp[i]->setScale(0.0f);
        auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEATACTION_TAG);
        m_pPartsSp[i]->runAction(repeat);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pPartsSp[all]->setPosition(m_partsPos[randIndex]);
        m_partsEndPos[all] = m_partsPos[randIndex];
        all++;
    } /*while*/
    
    m_pStageSp = Sprite::create(STAGE_PNG_FILE);
    m_pStageSp->setPosition(Vec2(3563, 210));
    this->addChild(m_pStageSp);
    
    return true;
}

// 进入本回合
void Round020_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round020_02::changeScalePartsSchedule), 0.4f, -1, 0);
}

//改变大小出现
void Round020_02::changeScalePartsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round020_02::changeScalePartsSchedule));
        m_pRobotActor->start();
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pPartsSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

/* 开启触摸 */
void Round020_02::openTouch()
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
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
        if (m_targetRect.containsPoint(this->convertTouchToNodeSpace(touch))) {
            if (index == 0) {
                target->stopActionByTag(REPEATACTION_TAG);
                target->runAction(FadeOut::create(0.2f));
                changeActor();
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round020_02::fadeOutAll), 0.5f);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pPartsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pPartsSp[i]);
    } /*for*/
    
}

//答对后渐隐all
void Round020_02::fadeOutAll(float)
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pPartsSp[i]->runAction(FadeOut::create(0.4f));
    }
}

//答对后改变actor动作
void Round020_02::changeActor()
{
    m_pRobotActor->playWithSequenceAliasName(TRIGGER);
}

/*
 *  庆祝
 */
void Round020_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round020_02::endRound()
{

    m_pStageScene->manualScroll(2810.0f, [this]
    {
        if (m_completeCB) {
            m_completeCB();
        }
        auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
        auto moveDuration = 2810.0f / bgScrollSpeed;
        m_pStageScene->moveLeadingActor(moveDuration, Vec2(80.0f, 0.0f));
        m_pStageSp->runAction(MoveBy::create(moveDuration, Vec2(190.0f, 50.0f)));
    });
}
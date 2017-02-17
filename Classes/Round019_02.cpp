//
//  Round019_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/19.
//
//

#include "Round019_02.h"
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
#define NUMBERS_ZORDER              3
#define LIGHT_GAF_CONFIG_FILE       "config/stage019/light.json"
#define LIGHT_GAF_POS               Vec2(827,751)
#define LIGHT_GAF_ZORDER            1
#define LIGHT_GAF_FILE              "res/stage019/XQ-GK07-2-IT02/XQ-GK07-2-IT02.gaf"
#define STANDBY                     "standby"                                   // 待机动画
#define TRIGGER                     "trigger"                                    // 触发动画

#define NPC_GAF_FILE                "res/stage019/XQ-GK07-2-NPC/XQ-GK07-2-NPC.gaf"    // NPC gaf
#define NPC_KEY                     "NPC-068"
#define NPC_COLLECT_DURATION        1.5f
#define NPC_GAF_POS                 Vec2(1338,775)
#define NPC_GAF_ZORDER              4
#define NPC_ENTER_BAG_MUSIC_FILE   "res/audio/npccollect/NPCjinbeibao.mp3"
#define TOPIC_ZORDER                2
#define MOV_TIME                   0.2f                                        // 答错回原位置移动时间
#define NUMBER_OF_ANSWER_RIGHT     2                                            // 答题正确数量达到该数字怎本回合结束
#define REPEATACTION_TAG           100
#define STAGE_PNG_FILE             "res/stage019/XQ-GK07-BGpingtai.png"

/*
 *  constructor
 */
Round019_02::Round019_02()
:m_InitAniIndex(0)
{
    m_answerNumbersPos[0] = Vec2(741, 621);
    m_answerNumbersPos[1] = Vec2(976, 621);
    m_answerNumbersPos[2] = Vec2(1211, 621);
    
    
}

/*
 *  destructor
 */
Round019_02::~Round019_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(LIGHT_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(STAGE_PNG_FILE);
}



//
Round019_02* Round019_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round019_02();
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
bool Round019_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pLightActor = Actor::createWithGafConfigFile(LIGHT_GAF_CONFIG_FILE);
    m_pLightActor->setPosition(LIGHT_GAF_POS);
    this->addChild(m_pLightActor, LIGHT_GAF_ZORDER);
    m_pLightActor->playWithSequenceAliasName(STANDBY);
    m_pLightActor->stop();
    
    m_pNpcActor = Actor::createWithGafFile(NPC_GAF_FILE);
    m_pNpcActor->setPosition(NPC_GAF_POS);
    this->addChild(m_pNpcActor, NPC_GAF_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 0) {
            sprintf(buffer, "SZ2-04.png");
        }else if (i == 1)
        {
            sprintf(buffer, "SZ2-07.png");
        }else if (i == 2)
        {
            sprintf(buffer, "SZ2-09.png");
        }
        m_pAnswerNumbersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswerNumbersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswerNumbersSp[i], NUMBERS_ZORDER);
        m_pAnswerNumbersSp[i]->setScale(0.0f);
        auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEATACTION_TAG);
        m_pAnswerNumbersSp[i]->runAction(repeat);
    }
    
    float topicX = 519.0f; float topicY = 968.0f; float offSetX = 210.0f;
    for (unsigned int i = 0; i < 6; i++) {
        if (i == 2 || i == 4) {
            sprintf(buffer, "XQ-GK07-2-TM03.png");
        }else
        {
            sprintf(buffer, "SZ2-%02d.png",i+5);
        }
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(Vec2(topicX+offSetX*i, topicY));
        this->addChild(m_pTopicsSp[i], TOPIC_ZORDER);
        m_pTopicsSp[i]->setScale(0.0f);
    }
    

    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswerNumbersSp[all]->setPosition(m_answerNumbersPos[randIndex]);
        m_answerNumbersEndPos[all] = m_answerNumbersPos[randIndex];
        all++;
    } /*while*/
    
    m_pStageSp = Sprite::create(STAGE_PNG_FILE);
    m_pStageSp->setPosition(Vec2(3663, 210));
    this->addChild(m_pStageSp);
    
    return true;
}

// 进入本回合
void Round019_02::onEnterRound()
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
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        processNpcCollect();
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
    m_pLightActor->start();
    this->schedule(CC_SCHEDULE_SELECTOR(Round019_02::changeScaleTopicsSchedule), 0.4f, -1, 0);
}

//改变大小出现
void Round019_02::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 6) {
        unschedule(CC_SCHEDULE_SELECTOR(Round019_02::changeScaleTopicsSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round019_02::changeScaleAnswersSchedule), 0.4f, -1, 0);
        return;
    }
    m_pTopicsSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

//改变大小出现
void Round019_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round019_02::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round019_02::scaleChangeMarksSchedule), 0.65f, -1, 0);
        return;
    }
    m_pAnswerNumbersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

//突出问号以提示
void Round019_02::scaleChangeMarksSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round019_02::scaleChangeMarksSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    if (m_InitAniIndex == 0) {
        m_pTopicsSp[2]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
            m_InitAniIndex++;
        }), NULL));
    }else if (m_InitAniIndex == 1)
    {
        m_pTopicsSp[4]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
            m_InitAniIndex++;
        }), NULL));
    }
    
}

/* 开启触摸 */
void Round019_02::openTouch()
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
        bool isCorrect = false;
        unsigned int numberIndex = 0;
        Vec2 targetPos;
        Rect rect = Rect(0.0f, 0.0f, m_pTopicsSp[2]->getContentSize().width, m_pTopicsSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[2]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && index == 1) {
                isCorrect = true;
                targetPos = m_pTopicsSp[2]->getPosition();
                numberIndex = 2;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicsSp[4]->getContentSize().width, m_pTopicsSp[4]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[4]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && index == 2) {
                isCorrect = true;
                targetPos = m_pTopicsSp[4]->getPosition();
                numberIndex = 4;
            }
        }
        if (isCorrect) {
            target->setPosition(targetPos);
            target->stopActionByTag(REPEATACTION_TAG);
            m_pTopicsSp[numberIndex]->setVisible(false);
            m_curCorrectNumber++;
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
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
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answerNumbersEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_answerNumbersEndPos[index]);
            }), NULL));
        }
        
        if (m_curCorrectNumber == NUMBER_OF_ANSWER_RIGHT) {
            scheduleOnce(CC_SCHEDULE_SELECTOR(Round019_02::fadeOutAll),0.5f);
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            celebrate(CELEBRATE_DELAY);
            for (unsigned int i = 0; i < 3; i++) {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswerNumbersSp[i]);
            } /*for*/
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswerNumbersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswerNumbersSp[i]);
    } /*for*/
    
}

//答对后渐隐all
void Round019_02::fadeOutAll(float)
{
    for (unsigned int i = 0; i < 6; i++) {
        m_pTopicsSp[i]->runAction(FadeOut::create(0.4f));
    }
    
    for (unsigned int j = 0; j < 3; j++) {
        m_pAnswerNumbersSp[j]->runAction(FadeOut::create(0.4f));
    }
    changeActor();
}

//答对后改变actor动作
void Round019_02::changeActor()
{
    m_pLightActor->playWithSequenceAliasName(TRIGGER);
}

/*
 *  庆祝
 */
void Round019_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round019_02::endRound()
{
    m_pStageScene->manualScroll(2810.0f, [this]
    {
        auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
        auto moveDuration = 2810.0f / bgScrollSpeed;
        if (m_completeCB) {
            m_completeCB();
        }
        m_pStageScene->moveLeadingActor(moveDuration, Vec2(80.0f, 0.0f));
        m_pStageSp->runAction(MoveBy::create(moveDuration, Vec2(80.0f, 50.0f)));
    });
}

/*  处理npc收集 */
void Round019_02::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(50, 400), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pNpcActor->setVisible(false);
    }), NULL);
    m_pNpcActor->runAction(npcActions);
    
    //
    scheduleOnce([this] (float dt)
    {
         auto pNpcManager = NpcDataManager::getInstance();
         auto pNpcData = pNpcManager->getNpcDataByKey(NPC_KEY);
         if (pNpcData != nullptr ) {
             pNpcData->collect();
         }
         m_pStageScene->getRoundUINode()->setCollectNpcKey(NPC_KEY);
         m_pStageScene->getRoundUINode()->changeBagGafActor(2);
    }, NPC_COLLECT_DURATION, "npcCollected");
}
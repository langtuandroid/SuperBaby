//
//  Round046_02.cpp
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#include "Round046_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "AppConfig.h"
#include "NpcDataManager.h"
#include "NpcData.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  1
#define MS_GAF_CONFIG_FILE              "res/stage046/ms01.json"
#define MS_GAF_FILE                     "res/stage046/GC-GK10-02-MS-D+C/GC-GK10-02-MS-D+C.gaf"
#define MS_POS                          Vec2(1462,272)
#define MS_ZORDER                       2
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define ANSWER_BEGAN_POS                Vec2(1573,525)
#define NPC_KEY                         "NPC-021"
#define NPC_COLLECT_DURATION            1.5f
#define NPC_ENTER_BAG_MUSIC_FILE        "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round046_02::Round046_02()
{
    m_answersPos[0] = Vec2(473, 1011);
    m_answersPos[1] = Vec2(797, 1011);
    m_answersPos[2] = Vec2(1161, 1011);
    m_answersPos[3] = Vec2(1489, 1011);
    
    m_answersEndPos[0] = Vec2(1530,509);
    m_answersEndPos[1] = Vec2(300,800);
    m_answersEndPos[2] = Vec2(1495,615);
    m_answersEndPos[3] = Vec2(1651,647);
    
}

/*
 *  destructor
 */
Round046_02::~Round046_02()
{
    cleanUp();
}

//
Round046_02* Round046_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round046_02();
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
bool Round046_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "GC-GK10-2-DA%02ld.png",i+1);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswers[i]->setUserData((void*)i);
        m_pAnswers[i]->setPosition(ANSWER_BEGAN_POS);
        m_pAnswers[i]->setScale(0.0f);
        addChild(m_pAnswers[i],ANSWERS_ZORDER);
        if (i == 3) {
            m_pAnswers[i]->setRotation(90.0f);
        }
    }
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(buffer, "res/stage046/GC-GK10-2-DA%02d.png",i+1);
        m_pImages[i] = new Image();
        m_pImages[i]->initWithImageFile(buffer);
    }
    
    return true;
}

// 进入本回合
void Round046_02::onEnterRound()
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
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        for (unsigned int i = 0; i < 4; i++) {
            if (i != 1) {
                m_pAnswers[i]->setVisible(false);
            }
        }
        MyAudioEngine::getInstance()->playAudio(NPC_ENTER_BAG_MUSIC_FILE);
        processNpcCollect();
        std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
        MyAudioEngine::getInstance()->playAudio(effStr.c_str());
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
                                       {
                                           auto dispatcher = Director::getInstance()->getEventDispatcher();
                                           dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
                                           dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
                                       });
    this->schedule(CC_SCHEDULE_SELECTOR(Round046_02::upAnswersSchedule), 1.1f, -1, 0);
}

//上升答案
void Round046_02::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round046_02::upAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    if (m_InitAniIndex == 3) {
        m_pAnswers[m_InitAniIndex]->runAction(Sequence::create(Spawn::create(MoveTo::create(1.0f, m_answersPos[m_InitAniIndex]),RotateBy::create(1.0f, -90.0f),ScaleTo::create(1.0f, 1.0f), NULL),CallFunc::create([this]{
            m_InitAniIndex++;
        }), NULL));
    } else
    {
        m_pAnswers[m_InitAniIndex]->runAction(Sequence::create(Spawn::create(MoveTo::create(1.0f, m_answersPos[m_InitAniIndex]),ScaleTo::create(1.0f, 1.0f), NULL),CallFunc::create([this]{
            m_InitAniIndex++;
        }), NULL));
    }
}

//
void Round046_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    for (unsigned int i = 0; i < 4; i++) {
        CC_SAFE_DELETE(m_pImages[i]);
    }
}

/* 开启触摸 */
void Round046_02::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        for (int i = 0; i < 4; i++) {
            auto touchPoint = m_pAnswers[i]->convertTouchToNodeSpace(touch);
            Rect rect = Rect(0.0f, 0.0f, m_pAnswers[i]->getContentSize().width, m_pAnswers[i]->getContentSize().height);
            if (rect.containsPoint(touchPoint)) {
                unsigned char *data = m_pImages[i] ->getData();
                
                int pa = 4 * ((m_pImages[i]->getHeight() - (int)(touchPoint.y) - 1) * m_pImages[i]->getWidth() + (int)(touchPoint.x)) + 3;
                unsigned int ap = data[pa];
                if (ap > 0) {
                    if (index == i) {
                        m_pAnswers[i]->setScale(1.1f);
                        m_curSelIndex = i;
                        return true;
                    }
                } else {
                    continue;
                }
            }
        } /*for*/
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        m_pAnswers[m_curSelIndex]->setScale(1.0f);
        if (m_curSelIndex == 1) {   //答对
            m_pAnswers[m_curSelIndex]->setLocalZOrder(10);
            m_pAnswers[m_curSelIndex]->runAction(Sequence::create(Spawn::create(MoveTo::create(1.0f, m_answersEndPos[m_curSelIndex]),RotateBy::create(1.0f, 45.0f),ScaleTo::create(1.0f, 0.5f), NULL),Spawn::create(MoveBy::create(0.5f, Vec2(0.0f, -400.0f)),FadeOut::create(0.6f), NULL),CallFunc::create([this]{
            
                downAnswers();
            }), NULL));
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            celebrate(CELEBRATE_DELAY);
            
            for (unsigned int i = 0; i < 4; i++) {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswers[i]);
            } /*for*/
        } else {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswers[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswers[i]);
    } /*for*/
}

void Round046_02::downAnswers()
{
    for (unsigned int i = 0; i < 4; i++) {
        if (i != 1) {
            if (i == 3) {
                m_pAnswers[i]->runAction(Sequence::create(Spawn::create(MoveTo::create(0.5f, m_answersEndPos[i]),RotateBy::create(0.5f, 60.0),ScaleTo::create(0.5f, 0.2f), NULL), NULL));
            } else
            {
                if (i == 0) {
                    m_pAnswers[i]->setLocalZOrder(10);
                }
                m_pAnswers[i]->runAction(Sequence::create(Spawn::create(MoveTo::create(0.5f, m_answersEndPos[i]),ScaleTo::create(0.5f, 0.2f), NULL), NULL));
            }
        }
    }
}

/*
 *  庆祝
 */
void Round046_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round046_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round046_02::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(100, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pMSActor->setVisible(false);
    }), NULL);
    m_pMSActor->runAction(npcActions);
    
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
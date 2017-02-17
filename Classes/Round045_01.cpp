//
//  Round045_01.cpp
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#include "Round045_01.h"
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

#define BN_PLIST_FILE                  "res/stage045/GC-GK09-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage045/GC-GK09-IT.png"               // BN的 png 文件

#define NPC_GAF_CONFIG_FILE             "res/stage045/ms01.json"
#define NPC_GAF_FILE                    "res/stage045/GC-GK09-1-MS01-D+C1+C2/GC-GK09-1-MS01-D+C1+C2.gaf"    // NPC gaf
#define NPC_KEY                         "NPC-050"
#define NPC_COLLECT_DURATION            1.5f
#define NPC_ZORDER                      2
#define NPC_POS                         Vec2(1281,196)

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define NPC_ENTER_BAG_MUSIC_FILE        "res/audio/npccollect/NPCjinbeibao.mp3"
#define ANSWERS_ZORDER                  5
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGERONE                      "triggerone"                                // 触发动画
#define TRIGGERTWO                      "triggertwo"                                // 触发动画
#define TARGET_POS                      Vec2(1471,357)
#define SHAWDOW_PNG_FILE                "GC-GK09-1-DA01-ying.png"
#define SHAWDOW_POS                     Vec2(1269, 856)
#define SHAWDOW_ZORDER                  1

/*
 *  constructor
 */
Round045_01::Round045_01(){
    m_answersPos[0] = Vec2(1259, 856);
    m_answersPos[1] = Vec2(458, 756);
    m_answersPos[2] = Vec2(947, 855);
    m_answersPos[3] = Vec2(706, 893);
}

/*
 *  destructor
 */
Round045_01::~Round045_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
}

//
Round045_01* Round045_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round045_01();
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
bool Round045_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "GC-GK09-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setPosition(m_answersPos[i]);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    } /*for*/
    
    m_pShawdowSp = Sprite::createWithSpriteFrameName(SHAWDOW_PNG_FILE);
    m_pShawdowSp->setPosition(SHAWDOW_POS);
    this->addChild(m_pShawdowSp, SHAWDOW_ZORDER);
    
    m_pNpcActor = Actor::createWithGafConfigFile(NPC_GAF_CONFIG_FILE);
    m_pNpcActor->setPosition(NPC_POS);
    m_pNpcActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pNpcActor, NPC_ZORDER);
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }

    return true;
}

//
void Round045_01::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round045_01::changeScaleAnswersSchedule), 0.6f, -1, 0);
}

//提示答案
void Round045_01::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round045_01::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    if (m_InitAniIndex == 0) {
        m_pShawdowSp->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round045_01::openTouch()
{
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        unsigned long index = (unsigned long)target->getUserData();
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) /*答对*/ {
                changeActor();
                m_pShawdowSp->setVisible(false);
                target->runAction(Sequence::create(MoveBy::create(0.5f, Vec2(150, -250)),
                                                   MoveTo::create(0.5f, TARGET_POS),
                                                   CallFunc::create([this]{
                    changeActor();
                }),FadeOut::create(0.1f), NULL));
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
                    dispatcher->removeEventListenersForTarget(m_pAnswersSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round045_01::changeActor()
{
    m_pNpcActor->playWithSequenceAliasName(TRIGGERONE);
    m_pNpcActor->setAnimationFinishedPlayDelegate([this]{
        m_pNpcActor->playWithSequenceAliasName(TRIGGERTWO,true);
    });
}

//
void Round045_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(true);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round045_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round045_01::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(100, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
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
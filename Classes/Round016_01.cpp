//
//  Round016_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#include "Round016_01.h"
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

#define NPC_GAF_FILE                "res/stage016/XQ-GK04-1-MS01-D+C/XQ-GK04-1-MS01-D+C.gaf"    // NPC gaf
#define NPC_KEY                     "NPC-053"
#define NPC_COLLECT_DURATION        1.5f
#define NPC_GAF_POS                 Vec2(1038,275)
#define NPC_GAF_ZORDER              2
#define NPC_GAF_CONFIG_FILE         "config/stage016/robot.json"

#define CRANE_GAF_FILE              "res/stage016/XQ-GK04-1-TM01-D+C/XQ-GK04-1-TM01-D+C.gaf"
#define CRANE_GAF_CONFIG_FILE       "config/stage016/crane.json"
#define CRANE_GAF_POS               Vec2(1788,1562)
#define CRANE_GAF_ZORDER            1

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define NPC_ENTER_BAG_MUSIC_FILE   "res/audio/npccollect/NPCjinbeibao.mp3"
#define STANDBY                    "standby"                                   // 待机动画
#define TRIGGER                    "trigger"                                // 触发动画一
#define TRIGGERONE                 "triggerone"                                // 触发动画一
#define TRIGGERTWO                 "triggertwo"                                // 触发动画二
#define BN_PLIST_FILE              "res/stage016/XQ-GK04-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                "res/stage016/XQ-GK04-IT.png"               // BN的 png 文件
#define BN_ZORDER                  3                                           // BN的z值
#define FOODS_ZORDER               1
#define MOV_TIME                   0.2f                                        // 答错回原位置移动时间

/*
 *  constructor
 */
Round016_01::Round016_01(){
    
    m_foodsPos[0] = Vec2(294, 899);
    m_foodsPos[1] = Vec2(588, 899);
    m_foodsPos[2] = Vec2(882, 899);
    m_foodsPos[3] = Vec2(1176, 899);
}

/*
 *  destructor
 */
Round016_01::~Round016_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(CRANE_GAF_FILE);
}

//
Round016_01* Round016_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round016_01();
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
bool Round016_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pNpcActor = Actor::createWithGafConfigFile(NPC_GAF_CONFIG_FILE);
    m_pNpcActor->setPosition(NPC_GAF_POS);
    m_pNpcActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pNpcActor, NPC_GAF_ZORDER);
    
    m_pCraneActor = Actor::createWithGafConfigFile(CRANE_GAF_CONFIG_FILE);
    m_pCraneActor->playWithSequenceAliasName(STANDBY,true);
    m_pCraneActor->setPosition(CRANE_GAF_POS);
    this->addChild(m_pCraneActor, CRANE_GAF_ZORDER);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "XQ-GK04-1-DA01-%02ld.png",i+1);
        m_pFoodsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pFoodsSp[i]->setUserData((void*)i);
        m_pBN->addChild(m_pFoodsSp[i], FOODS_ZORDER);
        m_pFoodsSp[i]->setScale(0.0f);
        
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[4] = {false, false, false, false};
    unsigned int placed = 0;
    
    while (placed < 4) {
        auto randIndex = static_cast<unsigned int>(rand() % 4);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pFoodsSp[placed]->setPosition(m_foodsPos[randIndex]);
        m_foodsEndPos[placed] = m_foodsPos[randIndex];
        if (randIndex == 2) {
            m_curRandThirdPosFoodIndex = placed;
        }
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
void Round016_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round016_01::changeScaleFoodsSchedule), 0.5f, -1, 0);

    
}
//改变大小出现
void Round016_01::changeScaleFoodsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round016_01::changeScaleFoodsSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round016_01::changeScaleFoodsHintSchedule), 0.6f, -1, 0);
        return;
    }
    m_pFoodsSp[m_InitAniIndex]->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)),CallFunc::create([this]{
        m_pFoodsSp[m_InitAniIndex]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        m_InitAniIndex++;
    }), NULL));
}

//提示
void Round016_01::changeScaleFoodsHintSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round016_01::changeScaleFoodsHintSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pFoodsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round016_01::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f, m_pNpcActor->getContentSize().width, m_pNpcActor->getContentSize().height);
        unsigned long index = (unsigned long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(m_pNpcActor->convertTouchToNodeSpace(touch))) {
            if (index == m_curRandThirdPosFoodIndex) /*答对*/ {
                target->runAction(FadeOut::create(0.2f));
                //改变actor动作
                changeNpcActor();
                fadeOutFoods();
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
                    dispatcher->removeEventListenersForTarget(m_pFoodsSp[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_foodsEndPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_foodsEndPos[index]);
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_foodsEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_foodsEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pFoodsSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pFoodsSp[i]);
    } /*for*/
}

//答对后隐藏剩余食物
void Round016_01::fadeOutFoods()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_pFoodsSp[i]->runAction(FadeOut::create(0.5f));
    }
}

//答对后改变actor动作
void Round016_01::changeNpcActor()
{
    m_pNpcActor->playWithSequenceAliasName(TRIGGERONE);
    m_pNpcActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pNpcActor->playWithSequenceAliasName(TRIGGERTWO);
    });
}

//开启吊车门
void Round016_01::openDoor()
{
    m_pCraneActor->playWithSequenceAliasName(TRIGGER);
    m_pCraneActor->setAnimationFinishedPlayDelegate([this]
    {
        upMoveDoor();
    });
}

//上升移动门
void Round016_01::upMoveDoor()
{
    m_pCraneActor->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0.0f, 2000.0f)),CallFunc::create([this]
    {
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
}

//
void Round016_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
     // 播放过关庆祝动画
     m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round016_01::endRound()
{
    openDoor();         //打开门
}

/*  处理npc收集 */
void Round016_01::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(200, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
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

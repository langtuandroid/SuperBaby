//
//  Round013_03.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round013_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "AppConfig.h"

using namespace gaf;

// 大图
#define STAGE_IMAGE_FILE               "res/stage013/XQ-GK01-IT02.png"
#define STAGE_PLIST_FILE               "res/stage013/XQ-GK01-IT02.plist"

// 庆祝延时
#define CELEBRATE_DELAY                     5.0f
//
#define NPC_COLLECT_DURATION                1.5f

// npc
#define ROBOT_GAF_CONFIG                    "res/stage013/XQ-GK01-3-DA01-D+C.json"
#define ROBOT_GAF_FILE                      "res/stage013/XQ-GK01-3-DA01-D+C/XQ-GK01-3-DA01-D+C.gaf"

// box
#define BOX_PNG                             "res/stage013/XQ-GK01-3-IT01.png"

#define NPC_KEY                             "NPC-054"
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"

#define SCALE_ORIGIN                    0.36f
#define SCALE_DURATION                  0.3f
#define SCALE_FACTOR                    1.2f

/*
 *  constructor
 */
Round013_03::Round013_03() {}

/*
 *  destructor
 */
Round013_03::~Round013_03()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF_FILE);
    
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(STAGE_PLIST_FILE);
}

//
Round013_03* Round013_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round013_03();
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
bool Round013_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(BOX_PNG);
    appendAsyncLoadTexture(STAGE_IMAGE_FILE);
    appendRemovedTexture(BOX_PNG);
    
    appendAsyncLoadGaf(ROBOT_GAF_FILE);
    preLoadRes();
    return true;
}

//
void Round013_03::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // npc
    m_pNpcActor = Actor::createWithGafConfigFile(ROBOT_GAF_CONFIG);
    m_pNpcActor->setPosition(Vec2(728.0f, 608.0f));
    addChild(m_pNpcActor, 3);
    m_pNpcActor->playWithSequenceAliasName("standby", true);
    
    // box
    auto boxSp = Sprite::create(BOX_PNG);
    boxSp->setPosition(Vec2(1532.0f, 912.0f));
    addChild(boxSp, 1);
    
    //
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 2);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(STAGE_PLIST_FILE);
    char szBuffer[32];
    for (long i = 0; i < 4; i++)
    {
        sprintf(szBuffer, "XQ-GK01-3-TM%02ld-2.png", i + 1);
        m_robotSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_robotSp[i]->setScale(SCALE_ORIGIN);
        m_robotSp[i]->setUserData((void*)i);
        m_pSBN->addChild(m_robotSp[i], 1);
    } /*for*/
    
    float fx = 1326.0f, fy = 1080.0f, hoffset = 400.0f;
    m_robotPos[0] = Vec2(fx, fy);
    m_robotPos[1] = Vec2(fx + hoffset, fy);
    m_robotPos[2] = Vec2(fx, 746.0f);
    m_robotPos[3] = Vec2(fx + hoffset, 746.0f);
    
    // 随机
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4)
    {
        unsigned int index = static_cast<unsigned int>(rand() % 4);
        if (selected[index]) continue;
        selected[index] = true;
        m_robotSp[all]->setPosition(m_robotPos[index]);
        scaleIndex[index] = all;
        all++;
    } /*while*/
}

/*
 *  进入回合
 */
void Round013_03::onEnterRound()
{
    long robotIndex = scaleIndex[m_numberIndex];
    auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                     ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN),
                     ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                     ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN), CallFunc::create([this, robotIndex] {
        char szBuffer[64];
        sprintf(szBuffer, "XQ-GK01-3-TM%02ld.png", robotIndex + 1);
        m_robotSp[robotIndex]->setVisible(false);
        m_robotSp1[robotIndex] = Sprite::createWithSpriteFrameName(szBuffer);
        m_robotSp1[robotIndex]->setScale(SCALE_ORIGIN);
        m_robotSp1[robotIndex]->setPosition(m_robotPos[m_numberIndex]);
        m_robotSp1[robotIndex]->setUserData((void*)robotIndex);
        m_pSBN->addChild(m_robotSp1[robotIndex], 1);
        
        scaleCompleteCB();
    }),NULL);
    m_robotSp[robotIndex]->runAction(scaleSeq);
}

//
void Round013_03::scaleCompleteCB()
{
    if (++m_numberIndex == 4)
    {
        openTouch();
        auto dispatcher = Director::getInstance()->getEventDispatcher();
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
               if (m_completeCB)
               {
                   m_completeCB();
               }
           }
        );
    } else {
        long robotIndex = scaleIndex[m_numberIndex];
        auto cb = CallFunc::create([this, robotIndex]
        {
            char szBuffer[64];
            sprintf(szBuffer, "XQ-GK01-3-TM%02ld.png", robotIndex + 1);
            m_robotSp[robotIndex]->setVisible(false);
            m_robotSp1[robotIndex] = Sprite::createWithSpriteFrameName(szBuffer);
            m_robotSp1[robotIndex]->setScale(SCALE_ORIGIN);
            m_robotSp1[robotIndex]->setPosition(m_robotPos[m_numberIndex]);
            m_robotSp1[robotIndex]->setUserData((void*)robotIndex);
            m_pSBN->addChild(m_robotSp1[robotIndex], 1);
            scaleCompleteCB();
        });
        auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                                         ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN),
                                         ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                                         ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN), cb, NULL);
        m_robotSp[robotIndex]->runAction(scaleSeq);
    }
}

/* 开启触摸 */
void Round013_03::openTouch()
{
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            return true;
        }
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            long index = (long)target->getUserData();
            if (0 == index) /*答对*/ {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x, touch->getLocation().y));
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                celebrate(CELEBRATE_DELAY);
                
                auto eventDispatcher = Director::getInstance()->getEventDispatcher();
                for (int i = 0; i < 4; i++) {
                    eventDispatcher->removeEventListenersForTarget(m_robotSp[i]);
                } /*for*/
            } else {
                // 播放答错音效
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
    // 将本回合节点绑定到事件监听器
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_robotSp[0]);
    for (int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_robotSp[i]);
    } /*for*/
}

/*  处理npc收集 */
void Round013_03::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x - 118, npcBagPos.y - 111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(400, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),
                                       CallFunc::create([this]{m_pNpcActor->setVisible(false);
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
                 },
                 NPC_COLLECT_DURATION, "npcCollected");
}

/*
 *  庆祝
 */
void Round013_03::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB)
    {
        m_celebrateCB(1);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round013_03::endRound()
{
//    if (m_completeCB)
//    {
//        m_completeCB();
//    }
}

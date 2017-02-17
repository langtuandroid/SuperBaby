//
//  Round002_03.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round002_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "Actor.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"

#define ANT_GAF_CONFIG_FILE         "config/stage002/ant.json"          // 蚂蚁 gaf 配置文件
#define ANT_WG_GAF_CONFIG_FILE      "config/stage002/antWithGlass.json" // 带眼镜蚂蚁配置文件

#define ANT_WAIT                    "wait"      // 蚂蚁等待动画别名
#define ANT_WALK                    "walk"      // 蚂蚁行走动画别名

#define ANT_NODE_TAG                3000
#define NPC_COLLECT_DURATION        1.5f
#define NPC_KEY                     "NPC-026"
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"

#define NPC_GAF_FILE                "res/stage002/SL-GK02-3-GS01-D/SL-GK02-3-GS01-D.gaf"

using namespace gaf;

/*
 *  constructor
 */
Round002_03::Round002_03() {}

/*
 *  destructor
 */
Round002_03::~Round002_03()
{
    cleanUp();
}

//
Round002_03* Round002_03::createWithRoundIndex(const int roundIndex,StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round002_03();
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
bool Round002_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage002/SL-GK02-3-MS01-D+C/SL-GK02-3-MS01-D+C.gaf");
    appendAsyncLoadGaf("res/stage002/SL-GK02-3-MS02-D+C/SL-GK02-3-MS02-D+C.gaf");
    appendAsyncLoadGaf("res/stage002/SL-GK02-3-GS01-D/SL-GK02-3-GS01-D.gaf");
    preLoadRes();
    return true;
}

/*
 *  本回合结束后蚂蚁 move
 */
void Round002_03::antMoveOnComplete()
{
    // 蚂蚁切换为行走动画
    for (auto &it : m_pAntActors) {
       it->playWithSequenceAliasName(ANT_WALK, true);
    } /*for*/
    
    //
    m_pAntNode->runAction(MoveBy::create(5.0f, Vec2(-500.0f, 0.0f)));
}

/*
 *  清理
 */
void Round002_03::cleanUp()
{
    if (m_pAntNode != nullptr) {
        m_pStageScene->removeOBLeaderNode(ANT_NODE_TAG);
        m_pAntNode = nullptr;
    }
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(NPC_GAF_FILE);
}

void Round002_03::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    m_pAntNode = Node::create();
    m_pStageScene->addOverLeaderNode(m_pAntNode, this->getPosition(), ANT_NODE_TAG);
    
    m_pNpcActor = Actor::createWithGafFile(NPC_GAF_FILE);
    m_pNpcActor->setSize(Size(150, 150));
    m_pNpcActor->setPosition(Vec2(600, 750));
    m_pNpcActor->setPosAtNode(Vec2(-200, 350));
    addChild(m_pNpcActor,1);
    
    m_pArrow = Sprite::create("res/common/npcArrow.png");
    m_pArrow->setPosition(Vec2(75, 250));
    m_pNpcActor->addChild(m_pArrow);
    m_pArrow->setVisible(false);
    m_pArrow->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.0f)),
                                                               MoveBy::create(0.5f, Vec2(0.0f, 10.0f)),
                                                               MoveBy::create(0.5f, Vec2(0.0f, 10.0f)),
                                                               MoveBy::create(0.5f, Vec2(0.0f, -10.0f))
                                                               , NULL)));
    
    /* 加载蚂蚁 Actor */
    m_pAntActors[0] = Actor::createWithGafConfigFile(ANT_GAF_CONFIG_FILE);
    m_pAntActors[1] = Actor::createWithGafConfigFile(ANT_WG_GAF_CONFIG_FILE);
    m_pAntActors[2] = Actor::createWithActor(m_pAntActors[0]);
    m_pAntActors[3] = Actor::createWithActor(m_pAntActors[0]);
    for (unsigned long i = 0; i < 4; i++) {
        m_pAntActors[i]->setAutoReleaseAsset(true);
        m_pAntActors[i]->setUserData((void*)i);
        m_pAntActors[i]->playWithSequenceAliasName(ANT_WAIT, true);
        m_pAntNode->addChild(m_pAntActors[i], 3);
    } /*for*/
    
    srand(unsigned(time(0)));
    bool placed[4] = {false, false, false, false};
    int allPlaced = 0;
    float posX = 900.0f, posY = 600.0f, offsetX = 200.0f;
    unsigned int i = 0;
    while (allPlaced < 4)
    {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (placed[randIndex]) continue;
        placed[randIndex] = true;
        m_pAntActors[randIndex]->setPosition(Vec2(posX, posY));
        m_antPosIndex[randIndex] = i;
        posX += offsetX;
        allPlaced++;i++;
    } /*while*/
    
    // 蚂蚁矩形
    float antStartX = 1070.0f, antStartY = 240.0, antOffsetX = 180.0f, antOffsetY = 180.0f;
    for (unsigned int i = 0; i < 4; i++) {
        m_antRect[i] = Rect(antStartX + offsetX * i, antStartY, antOffsetX, antOffsetY);
    } /*for*/
}

/* 开启触摸 */
void Round002_03::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题正确监听器
    auto antListener = EventListenerTouchOneByOne::create();
    antListener->setSwallowTouches(true);
    antListener->onTouchBegan = [this](Touch* pTouch, Event* pEvent) -> bool
    {
        auto pAntActor = static_cast<Actor*>(pEvent->getCurrentTarget());
        auto touchPoint = this->convertToNodeSpace(pTouch->getLocation());
        unsigned long antIndex = (unsigned long)pAntActor->getUserData();
        
        if (m_antRect[m_antPosIndex[antIndex]].containsPoint(touchPoint))
        {
            if (m_correctNumber == antIndex ) /*选中了第二只蚂蚁，答对了*/ {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->playAnswerCorrectEffect(pTouch->getLocation());
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned long i = 0; i < 4; i++) {
                    dispatcher->removeEventListenersForTarget(m_pAntActors[i]);
                } /*for*/
                
                celebrate(3.0f);
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
        return false;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(antListener, m_pAntActors[0]);
    for (unsigned long i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(antListener->clone(), m_pAntActors[i]);
    } /*for*/
}

/*
 *  进入回合
 */
void Round002_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
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
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
}

//
void Round002_03::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round002_03::endRound()
{
    antMoveOnComplete();
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round002_03::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(400, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
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
    },
    NPC_COLLECT_DURATION, "npcCollected");
}

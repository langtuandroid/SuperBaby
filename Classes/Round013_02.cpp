//
//  Round013_02.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/09.
//
//

#include "Round013_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "GafAssetCache.h"
#include "NpcActor.h"
#include "CustomMessage.h"
#include "AppConfig.h"

// 大图
#define STAGE_IMAGE_FILE        "res/stage013/XQ-GK01-IT01.png"
#define STAGE_PLIST_FILE        "res/stage013/XQ-GK01-IT01.plist"

#define NPC_KEY                 "NPC-056"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

//
#define NPC_COLLECT_DURATION            1.5f

// npc 跳跃时间
#define NPC_JUMP_TIME                  1.0f
#define NPC_FadeOUT_TIME               1                              //npc淡出时间
#define MOV_TIME                       0.2f                                    // 答错回原位置移动时间

// 火箭配置文件
#define ROCKET_GAF_CONFIG       "res/stage013/XQ-GK01-2-MS01-D+C.json"
#define ROCKET_GAF              "res/stage013/XQ-GK01-2-MS01-D+C/XQ-GK01-2-MS01-D+C.gaf"

// NPC 机器人
#define ROBOT_GAF_CONFIG       "res/npc/NPC-056-D+C.json"

// 飞船
#define AIR_SHIP               "res/common/XQ-GK01-2-IT01.png"
#define AIR_SHIP_TAG           600

//
#define SCALE_DURATION                  0.3f
#define SCALE_FACTOR                    1.5f
#define LINE_SCALE_FACTOR               1.1f

#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"
#define PASS_GAF_FILE                   "res/common/yanshi02/yanshi02.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3

#define AISLE_PNG2            "res/stage013/XQ-GK01-1-QJ1.png"
#define AISLE_TAG       501

/*
 *  constructor
 */
Round013_02::Round013_02()
{
    m_linePos[0] = Vec2(1156.0f, 976.0f);
    m_linePos[1] = Vec2(1196.0f, 872.0f);
    m_linePos[2] = Vec2(1396.0f, 816.0f);
}

/*
 *  destructor
 */
Round013_02::~Round013_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);

    GafAssetCache* pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(ROCKET_GAF);
    
    // 删除飞船
    m_pStageScene->removeOBLeaderNode(AIR_SHIP_TAG);
    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->removeTextureForKey(AIR_SHIP);
    
    //
    CCSpriteFrameCache::getInstance()->removeSpriteFramesFromFile(STAGE_PLIST_FILE);
    
    // 删除过道图片
    m_pStageScene->removeOBLeaderNode(AISLE_TAG);
    textureCache->removeTextureForKey(AISLE_PNG2);
}

//
Round013_02* Round013_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round013_02();
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
bool Round013_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf(ROCKET_GAF);
    appendAsyncLoadTexture(AIR_SHIP);
    preLoadRes();
    return true;
}

//
void Round013_02::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // gaf
    m_rocketActor = Actor::createWithGafConfigFile(ROCKET_GAF_CONFIG);
    m_rocketActor->setPosition(Vec2(1522.0f, 832.0f));
    addChild(m_rocketActor, 1);
    m_rocketActor->playWithSequenceAliasName("standby", true);
    
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 2);
    CCSpriteFrameCache::getInstance()->addSpriteFramesWithFile(STAGE_PLIST_FILE);
    appendRemovedTexture(STAGE_IMAGE_FILE);
    
    // npc
    m_pNpcActor = NpcActor::createWithGafConfigFile(ROBOT_GAF_CONFIG);
    m_pNpcActor->setPosition(Vec2(616.0f, 406.0f));
    addChild(m_pNpcActor, 2);
    m_pNpcActor->standby(true);
    
    char szBuffer[64];
    for (int i = 0; i < 3; i++)
    {
        sprintf(szBuffer, "XQ-GK01-2-TM%02d-1.png", i + 1);
        m_LineSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_LineSp[i]->setPosition(m_linePos[i]);
        m_pSBN->addChild(m_LineSp[i], 1);
        
        sprintf(szBuffer, "XQ-GK01-2-TM%02d-2.png", i + 1);
        m_LineSpHL[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_LineSpHL[i]->setPosition(m_linePos[i]);
        m_LineSpHL[i]->setVisible(false);
        m_pSBN->addChild(m_LineSpHL[i], 2);
    } /*for*/
    // 飞船
    auto airSp = Sprite::create(AIR_SHIP);
    m_pStageScene->addBelowLeaderNode(airSp, Vec2(5422.0f, 300.0f), AIR_SHIP_TAG);
    
    // 数字
    for (long i = 0; i < 3; i++)
    {
        sprintf(szBuffer, "SZ1-%02ld.png", i + 1);
        m_numberSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_numberSp[i]->setUserData((void*)i);
        addChild(m_numberSp[i], 3);
    } /*for*/
    m_numberSp[0]->setPosition(Vec2(906.0f, 1076.0f));
    m_numberSp[1]->setPosition(Vec2(1288.0f, 952.0f));
    m_numberSp[2]->setPosition(Vec2(1612.0f, 704.0f));
}

//
void Round013_02::onEnterRound()
{
    auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f),
                                     ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f),
                                     NULL);
    m_numberSp[0]->runAction(scaleSeq);
    
    auto scaleLineSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, LINE_SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f),
                                     ScaleTo::create(SCALE_DURATION, LINE_SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f),
                                          CallFunc::create([this] {
        m_LineSp[0]->setVisible(true);
        m_LineSpHL[0]->setVisible(false);
        scaleCompleteCB();
    }), NULL);
    m_LineSp[0]->setVisible(false);
    m_LineSpHL[0]->setVisible(true);
    m_LineSpHL[0]->runAction(scaleLineSeq);
}

//
void Round013_02::scaleCompleteCB()
{
    if (++m_numberIndex == 3)
    {
        openTouch();
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->addCustomEventListener("endRound", [this] (EventCustom *)
        {
            auto dispatcher = Director::getInstance()->getEventDispatcher();
            dispatcher->removeCustomEventListeners("endRound");
            endRound();
        });
        dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *)
        {
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
            m_pStageScene->manualScroll(1360.0f, [this]
            {
                auto movedCB = [this]
                {
                    if (m_completeCB)
                    {
                        m_completeCB();
                    }
                };
                m_pStageScene->moveLeadingActorCB(3.0f, Vec2(740.0f, 0.0f), movedCB);
            });
        });
        if (!UserDefault::getInstance()->getBoolForKey("clicktwo", false))
        {
            playPassActor();        //播放引导
            UserDefault::getInstance()->setBoolForKey("clicktwo", true);
        }
    } else {
        auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
             ScaleTo::create(SCALE_DURATION, 1.0f),
             ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
             ScaleTo::create(SCALE_DURATION, 1.0f), CallFunc::create([this]
        {
            m_LineSp[m_numberIndex]->setVisible(true);
            m_LineSpHL[m_numberIndex]->setVisible(false);
            scaleCompleteCB();
        }), NULL);
        m_numberSp[m_numberIndex]->runAction(scaleSeq);
        
        auto scaleLineSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, LINE_SCALE_FACTOR),
                                              ScaleTo::create(SCALE_DURATION, 1.0f),
                                              ScaleTo::create(SCALE_DURATION, LINE_SCALE_FACTOR),
                                              ScaleTo::create(SCALE_DURATION, 1.0f), NULL);
        m_LineSp[m_numberIndex]->setVisible(false);
        m_LineSpHL[m_numberIndex]->setVisible(true);
        m_LineSpHL[m_numberIndex]->runAction(scaleLineSeq);
    }
}

/* 
 *  播放过关引导
 */
void Round013_02::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->addChild(colorLayer, PASS_GAF_ZORDER);
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, colorLayer);
    dispatcher->setPriority(listenerDrag, -130);
    Actor* passActor = Actor::createWithGafFile(PASS_GAF_FILE,false);
    passActor->setPosAtNode(Vec2(300, 500));
    passActor->setPosition(Vec2(350, 650));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this, passActor, colorLayer]
    {
        colorLayer->removeFromParent();
        passActor->setVisible(false);
    });
}

/* 
 *  开启触摸 
 */
void Round013_02::openTouch()
{
    ///////////////////
    // 事件派发
    auto listenerTouch = EventListenerTouchOneByOne::create();
    listenerTouch->setSwallowTouches(true);
    listenerTouch->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        auto rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerTouch->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        long selNumber = (long)target->getUserData();
        auto rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint))
        {
            if (selNumber == 1) /*答题正确*/
            {
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++)
                {
                    dispatcher->removeEventListenersForTarget(m_numberSp[i]);
                } /*for*/
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x,
                                                            touch->getLocation().y));
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1)
                {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                playLeadAnsCorEff();
                
                m_pStageScene->getRoundUINode()->stopReadEffect();  // 停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();    // 停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    listenerTouch->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
    };
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, m_numberSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch->clone(), m_numberSp[i]);
    } /*for*/
}

//
void Round013_02::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    //
    goOnRocket();
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/* 登上火箭 */
void Round013_02::goOnRocket()
{
    auto callfunc = CallFunc::create([this]
    {
        auto pos = m_pNpcActor->getPosition();
        pos.y += 600.0f;
        m_pNpcActor->setPosition(pos);
        m_pNpcActor->setScale(1.0f);
        rocketFly();
    });
    auto moveTargetPos = m_rocketActor->getPosition();
    moveTargetPos.y += 200.0f;
    auto spawnAction = Spawn::create(EaseExponentialIn::create(MoveTo::create(3.0f, moveTargetPos)),
        EaseExponentialIn::create(ScaleTo::create(3.0f, 0.3f)), NULL);
    m_pNpcActor->runAction( Sequence::create(spawnAction, callfunc, NULL));
}

//
void Round013_02::rocketFly()
{
    for (int i = 0; i < 3; i++)
    {
        m_numberSp[i]->removeFromParent();
        m_numberSp[i] = nullptr;
        m_LineSpHL[i]->removeFromParent();
        m_LineSpHL[i] = nullptr;
        m_LineSp[i]->removeFromParent();
        m_LineSp[i] = nullptr;
    } /*for*/
    m_rocketActor->playWithSequenceAliasName("trigger", false);
}

/*  回合结束回调 */
void Round013_02::endRound(){}

/*  处理npc收集 */
void Round013_02::processNpcCollect()
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


//
//  Round001_01.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//

#include "Round002_01.h"
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

#define SHAKE_ACTION_TAG            1001
#define CELEBRATE_GAF_BG_FILE       "res/common/guanghuanxiaoguo/guanghuanxiaoguo.gaf"
#define CELEBRATE_GAF_FILE          "res/common/wujiaoxing01/wujiaoxing01.gaf"
#define BN_PLIST_FILE                   "res/stage002/SL-GK02-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                     "res/stage002/SL-GK02-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值

#define NPC_GAF_FILE                "res/stage002/SL-GK02-1-GS01/SL-GK02-1-GS01.gaf"    // NPC gaf
#define NPC_POS                     Vec2(740.0f, 462.0f)
#define NPC_KEY                     "NPC-007"
#define NPC_COLLECT_DURATION        1.5f

#define FADE_OUT_TIME              0.5f                                             // 渐隐时间
#define INIT_UP_OFFSETY            300.0f                                           // 上升动画偏移值
#define INIT_UP_TIME               0.5f                                             // 上升动画时间

// 庆祝延时
#define CELEBRATE_DELAY            5.0f
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round002_01::Round002_01():m_correctNumber(2) {}

/*
 *  destructor
 */
Round002_01::~Round002_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);

}

//
Round002_01* Round002_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round002_01();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene, pos))
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
bool Round002_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(BN_PNG_FILE);
    appendAsyncLoadGaf(NPC_GAF_FILE);
    
    preLoadRes();
    return true;
}

//
void Round002_01::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char textureFile[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(textureFile, "SL-GK02-1-DA%02ld.png", i + 1);
        m_fruitSp[i] = Sprite::createWithSpriteFrameName(textureFile);
        m_fruitSp[i]->setUserData((void*)i);
        m_fruitSp[i]->setGlobalZOrder(1);
        m_pBN->addChild(m_fruitSp[i]);
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[4] = {false, false, false, false};
    unsigned int placed = 0;
    
    float posX = 750.0f, posY = -80.0f, offsetX = 280.0f;
    while (placed < 4)
    {
        auto randIndex = static_cast<unsigned int>(rand() % 4);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_fruitSp[randIndex]->setPosition(Vec2(posX, posY));
        m_fruitSpUp[placed] = m_fruitSp[randIndex];
        posX += offsetX;placed++;
    } /*while*/
    
    // 树上的水果
    m_pTreeFruits = ImageView::create("SL-GK02-1-TM01.png",ui::Widget::TextureResType::PLIST);
    addChild(m_pTreeFruits);
    m_pTreeFruits->setPosition(Vec2(1380.0f, 1030.0f));
    
    // npc
    m_pNpcActor = Actor::createWithGafFile(NPC_GAF_FILE, true, 0.7f, Vec2(-102.0f, 260.0f));
    m_pNpcActor->setPosition(NPC_POS);
    m_pNpcActor->setSize(Size(160.0f, 160.0f));
    addChild(m_pNpcActor);

    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
}

//
void Round002_01::onEnterRound()
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
    schedule(CC_SCHEDULE_SELECTOR(Round002_01::upFruitsSchedule), INIT_UP_TIME, -1, 0);
    changeFruit();
}

//回合开始，突出树上水果
void Round002_01::changeFruit()
{
    m_pTreeFruits->runAction(Sequence::create(DelayTime::create(2.0f),
                                              CallFunc::create([this]{
        m_pTreeFruits->loadTexture("SL-GK02-1-TM02.png",ui::Widget::TextureResType::PLIST);
                                              }),DelayTime::create(0.5f),
                                              CallFunc::create([this]{
        m_pTreeFruits->loadTexture("SL-GK02-1-TM01.png",ui::Widget::TextureResType::PLIST);
                                              }),DelayTime::create(0.5f),
                                              CallFunc::create([this]{
        m_pTreeFruits->loadTexture("SL-GK02-1-TM02.png",ui::Widget::TextureResType::PLIST);
                                              }),DelayTime::create(0.5f),
                                              CallFunc::create([this]{
        m_pTreeFruits->loadTexture("SL-GK02-1-TM01.png",ui::Widget::TextureResType::PLIST);
    }), NULL));
}

/* 开启触摸 */
void Round002_01::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Rect rectNode = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rectNode.containsPoint(target->convertToNodeSpace(touch->getLocation()))) {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        Rect rectNode = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rectNode.containsPoint(target->convertToNodeSpace(touch->getLocation())))
        {
            unsigned long index = (unsigned long)target->getUserData();
            if (index == m_correctNumber) /*答对*/
            {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1)
                {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                this->celebrate(CELEBRATE_DELAY);
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 4; i++) {
                    dispatcher->removeEventListenersForTarget(m_fruitSp[i]);
                } /*for*/
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                
                Vec2 originalPos = target->getPosition();
                Vec2 pointL = Vec2(target->getPosition().x - 24.0f, target->getPosition().y);
                Vec2 pointR = Vec2(target->getPosition().x + 24.0f, target->getPosition().y);
                
                MoveTo* moveLeft = MoveTo::create(0.2f, pointL);
                MoveTo* moveRight = MoveTo::create(0.2f, pointR);
                MoveTo* moveOrigina = MoveTo::create(0.2f, originalPos);
                FiniteTimeAction* action= Sequence::create(moveLeft, moveRight, moveOrigina, NULL);
                action->setTag(SHAKE_ACTION_TAG);
                target->runAction(action);
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_fruitSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_fruitSp[i]);
    } /*for*/
}

/*
 *  水果上升
 */
void Round002_01::upFruitsSchedule(float)
{
    if (4 == m_InitAniIndex) {
        unschedule(CC_SCHEDULE_SELECTOR(Round002_01::upFruitsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    } else {
        m_fruitSpUp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY))));
    }
}

//
void Round002_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

// 渐变消失水果答案
void Round002_01::fadeOutFruit()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_fruitSp[i]->runAction(FadeOut::create(FADE_OUT_TIME));
    } /*for*/
}


/*  回合结束回调 */
void Round002_01::endRound()
{
    fadeOutFruit();
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round002_01::processNpcCollect()
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

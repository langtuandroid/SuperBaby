//
//  Round029_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Round029_01.h"
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

#define BN_PLIST_FILE                  "res/stage029/HY-GK05-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage029/HY-GK05-IT.png"               // BN的 png 文件
#define NPC_GAF_FILE                    "res/stage029/HY-GK05-1-MS01-D/HY-GK05-1-MS01-D.gaf"    // NPC gaf
#define NPC_KEY                         "NPC-077"
#define NPC_COLLECT_DURATION            1.5f
#define NPC_ZORDER                      2
#define NPC_POS                         Vec2(1290,879)

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define NPC_ENTER_BAG_MUSIC_FILE        "res/audio/npccollect/NPCjinbeibao.mp3"
#define ANSWERS_ZORDER                  5
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(318,868)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(272,795)
#define WHITELIGHT_GAF_ZORDER           1
#define TOPIC_PNG_FILE                  "HY-GK05-1-TM01.png"
#define TOPIC_END_FILE                  "HY-GK05-1-TM02.png"
#define TOPIC_POS                       Vec2(1290,879)
#define TOPIC_BEGAN_ZORDER              3
#define TOPIC_END_ZORDER                1
#define HINT_PNG_FILE                   "HY-GK05-1-TMtishi.png"
#define HINT_POS                        Vect(969,891)
#define HINT_ZORDER                     4
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round029_01::Round029_01(){
    
    m_answersPos[0] = Vec2(870, -300);
    m_answersPos[1] = Vec2(1258, -300);
    m_answersPos[2] = Vec2(1646, -300);
}

/*
 *  destructor
 */
Round029_01::~Round029_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_GAF_FILE);
}

//
Round029_01* Round029_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round029_01();
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
bool Round029_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pPaopaoActor = Actor::createWithGafFile(PAOPAO_GAF_FILE);
    m_pPaopaoActor->setPosition(PAOPAO_GAF_POS);
    this->addChild(m_pPaopaoActor, PAOPAO_GAF_ZORDER);
    
    m_pWhiteLightActor = Actor::createWithGafFile(WHITELIGHT_GAF_FILE);
    m_pWhiteLightActor->setPosition(WHITELIGHT_GAF_POS);
    m_pWhiteLightActor->setSize(Size(300, 300));
    m_pWhiteLightActor->setPosAtNode(Vec2(-151, 747));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pBeganTopicSp = Sprite::createWithSpriteFrameName(TOPIC_PNG_FILE);
    m_pBeganTopicSp->setPosition(TOPIC_POS);
    this->addChild(m_pBeganTopicSp, TOPIC_BEGAN_ZORDER);
    
    m_pEndTopicSp = Sprite::createWithSpriteFrameName(TOPIC_END_FILE);
    m_pEndTopicSp->setPosition(TOPIC_POS);
    this->addChild(m_pEndTopicSp, TOPIC_END_ZORDER);
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    m_pHintSp->setOpacity(0);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK05-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->setScale(1.1f);
    } /*for*/
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int all = 0;
    
    while (all < 3) {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = Vec2(m_answersPos[randIndex].x,m_answersPos[randIndex].y+600);
        all++;
    } /*while*/
    
    m_pNpcActor = Actor::createWithGafFile(NPC_GAF_FILE);
    m_pNpcActor->setPosition(NPC_POS);
    this->addChild(m_pNpcActor, NPC_ZORDER);
    m_pNpcActor->setSize(Size(400, 400));
    m_pNpcActor->setPosAtNode(Vec2(-328,631));
    m_pNpcActor->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

//
void Round029_01::onEnterRound()
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
        m_pAnswersSp[1]->runAction(Sequence::create(Spawn::create(FadeOut::create(0.4f),CallFunc::create([this]{
            m_pBeganTopicSp->runAction(FadeOut::create(0.4f));
        }), NULL),DelayTime::create(0.5f),CallFunc::create([this]{
            m_pEndTopicSp->runAction(Sequence::create(FadeOut::create(0.4f),CallFunc::create([this]{
                processNpcCollect();
                std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
                MyAudioEngine::getInstance()->playAudio(effStr.c_str());
            }), NULL));
        }), NULL));
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round029_01::upAnswersSchedule), 0.4f, -1, 0);
}

//上升答案块
void Round029_01::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round029_01::upAnswersSchedule));
        fadeHintSp();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, 600))));
}

void Round029_01::fadeHintSp()
{
    m_pHintSp->runAction(Sequence::create(FadeIn::create(0.4f),
                                          FadeOut::create(0.4f),
                                          FadeIn::create(0.4f),
                                          FadeOut::create(0.4f),
                                          CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }),
                                          NULL));
}

/* 开启触摸 */
void Round029_01::openTouch()
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
            target->setLocalZOrder(10);
            target->setScale(1.2f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
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
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        unsigned long index = (unsigned long)target->getUserData();
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.1f);
        if (rect.containsPoint(touchPoint)) {
            
            if (index == 1) /*答对*/ {
                target->setPosition(m_pHintSp->getPosition());
                target->runAction(Sequence::create(ScaleTo::create(0.4f, 1.2f),ScaleTo::create(0.2f, 1.1f), NULL));
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
                fadeAnswer();
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round029_01::fadeAnswer()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswersSp[i]->runAction(FadeOut::create(0.4f));
        }
    }
}

//
void Round029_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(true);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round029_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round029_01::processNpcCollect()
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

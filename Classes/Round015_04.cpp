//
//  Round015_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/14.
//
//

#include "Round015_04.h"
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
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "ThemeSelScene.h"

#define BN_PNG_FILE                 "res/stage015/XQ-GK03-IT.png"               // BN的 png 文件
#define BN_ZORDER                   2                                           // BN的z值
#define NPC_COLLECT_DURATION        1.5f
#define CELEBRATE_DELAY             5.0f
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"
#define STANDBY                     "standby"                                // 待机动画
#define TRIGGER                     "trigger"                                // 触发动画
#define NPC_KEY                     "NPC-072"
#define NPC_GAF_CONFIG_FILE         "res/npc/NPC-072-D+C.json"
#define NPC_GAF_POS                 Vec2(1342.0f, 390.0f)

/*
 *  constructor
 */
Round015_04::Round015_04()
{
    m_topicsPos[0] = Vec2(861, 927);
    m_topicsPos[1] = Vec2(861, 782);
    m_topicsPos[2] = Vec2(861, 644);
    
    m_answersPos[0] = Vec2(774, 150);
    m_answersPos[1] = Vec2(1024, 150);
    m_answersPos[2] = Vec2(1274, 150);
}

/*
 *  destructor
 */
Round015_04::~Round015_04()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
}

//
Round015_04* Round015_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round015_04();
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
bool Round015_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    m_pNpcActor = Actor::createWithGafConfigFile(NPC_GAF_CONFIG_FILE);
    m_pNpcActor->playWithSequenceAliasName(STANDBY,true);
    m_pNpcActor->setPosition(NPC_GAF_POS);
    this->addChild(m_pNpcActor);
    
    char buffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK03-4-TM%02d.png",i+1);
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_topicsPos[i]);
        m_pBN->addChild(m_pTopicsSp[i]);
        
    } /*for*/
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ6-%02ld.png",(i+1)*2);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i]);
        m_pAnswersSp[i]->setScale(0.0f);
    }
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    
    while (placed < 3)
    {
        auto randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pAnswersSp[placed]->setPosition(m_answersPos[randIndex]);
        placed++;
    } /*while*/
    
    return true;
}

//
void Round015_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round015_04::answersAppearActionSchedule), 0.6f, -1, 0);
}

//答案出现时动作
void Round015_04::answersAppearActionSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round015_04::answersAppearActionSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round015_04::scaleChangeSchedule), 0.6f, -1, 0);
        return;
    }
    m_pAnswersSp[m_InitAniIndex]->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)),CallFunc::create([this]{
        m_pAnswersSp[m_InitAniIndex]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        m_InitAniIndex++;
    }), NULL));
}

void Round015_04::scaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 3)
    {
        unschedule(CC_SCHEDULE_SELECTOR(Round015_04::scaleChangeSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    } else {
        m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
    }
}

/* 开启触摸 */
void Round015_04::openTouch()
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            unsigned long index = (unsigned long)target->getUserData();
            if (index == 1) /*答对*/ {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                this->celebrate(CELEBRATE_DELAY);
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    dispatcher->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
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
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round015_04::celebrate(const float delay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
     // 播放过关庆祝动画
     m_pStageScene->getRoundUINode()->playPassActor(3);
    
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round015_04::chooseTrueAnswersActionSchedule), 0.85f, -1, 0);
}

/*  回合结束回调 */
void Round015_04::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

//答对后答案下降系列动作
void Round015_04::chooseTrueAnswersActionSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round015_04::chooseTrueAnswersActionSchedule));
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(MoveBy::create(0.3f, Vec2(0.0f, 100.0f)),MoveBy::create(0.4f, Vec2(0.0f, -400.0f)), NULL));
}

/*  处理npc收集 */
void Round015_04::processNpcCollect()
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


//回合结束分享调用
void Round015_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    processNpcCollect();    //npc收集
    std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
    MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}

//分享回调
void Round015_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round015_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round015_04::roundEndShareCallBack, this,true));
}

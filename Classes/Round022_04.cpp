//
//  Round022_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#include "Round022_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "AppConfig.h"

#define CELEBRATE_DELAY                 5.0f
#define MOVE_ACTOR_TIME                 3.0f                              //分享界面ok过后主角移动时间
#define DINO_GAF_FILE                   "res/stage022/XQ-GK10-4-NPC01-D/XQ-GK10-4-NPC01-D.gaf"
#define DINO_GAF_POS                    Vec2(1229,817)
#define DINO_ZORDER                     1
#define EGG_GAF_CONFIG_FILE             "config/stage022/egg.json"
#define EGG_GAF_FILE                    "res/stage022/XQ-GK10-4-TM01-D+C/XQ-GK10-4-TM01-D+C.gaf"
#define EGG_GAF_POS                     Vec2(1158,738)
#define EGG_GAF_ZORDER                  2
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define COLORLAYER_TAG                  300
#define NPC_KEY                         "NPC-091"
#define NPC_COLLECT_DURATION            1.5f
#define NPC_ENTER_BAG_MUSIC_FILE        "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round022_04::Round022_04()
:m_InitAniIndex(0)
{
    m_answersRect[0] = Rect(786, 621, 300, 300);
    m_answersRect[1] = Rect(1118, 467, 300, 300);
    m_answersRect[2] = Rect(1434, 558, 300, 300);
}

/*
 *  destructor
 */
Round022_04::~Round022_04()
{
    cleanUp();
}

//
void Round022_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
}

//
Round022_04* Round022_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round022_04();
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
bool Round022_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pDinoActor = Actor::createWithGafFile(DINO_GAF_FILE);
    m_pDinoActor->setPosition(DINO_GAF_POS);
    m_pDinoActor->setPosAtNode(Vec2(-400, 400));
    this->addChild(m_pDinoActor, DINO_ZORDER);
    
    m_pEggActor = Actor::createWithGafConfigFile(EGG_GAF_CONFIG_FILE);
    m_pEggActor->playWithSequenceAliasName(STANDBY,true);
    m_pEggActor->setPosition(EGG_GAF_POS);
    this->addChild(m_pEggActor, EGG_GAF_ZORDER);
    
    return true;
}

// 进入本回合
void Round022_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
       dispatcher->removeCustomEventListeners("openTouch");
       openTouch();
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
       moveActor();
       scheduleOnce(CC_SCHEDULE_SELECTOR(Round022_04::fadeOutLayer), 1.0f);
    });
}

/* 开启触摸 */
void Round022_04::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        for (unsigned int i = 0; i < 3; i++) {
            if (m_answersRect[i].containsPoint(this->convertTouchToNodeSpace(touch))) {
                return true;
            }
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        for (unsigned int i = 0; i < 3; i++) {
            if (m_answersRect[i].containsPoint(this->convertTouchToNodeSpace(touch))) {
                if (i == 1) {
                    changeActor();
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    celebrate(CELEBRATE_DELAY);
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
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, this);
}

//答对后改变actor动作
void Round022_04::changeActor()
{
    m_pEggActor->playWithSequenceAliasName(TRIGGER);
}

/*
 *  庆祝
 */
void Round022_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round022_04::endRound()
{
    
}

/*
 *  分享界面点击ok后移动主角
 */

void Round022_04::moveActor()
{
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2100, 0), true);
}

//渐变变黑退出
void Round022_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//回合结束分享调用
void Round022_04::roundEndShareCallBack(bool isShare)
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
void Round022_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round022_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round022_04::roundEndShareCallBack, this,true));
}


/*  处理npc收集 */
void Round022_04::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(200, 500), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pDinoActor->setVisible(false);
    }), NULL);
    m_pDinoActor->runAction(npcActions);
    
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
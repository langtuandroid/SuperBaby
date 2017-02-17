//
//  Round045_02.cpp
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#include "Round045_02.h"
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
#define ANSWERS_ZORDER                  4
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define MS_GAF_CONFIG_FILE              "res/stage045/ms02.json"
#define MS_GAF_FILE                     "res/stage045/GC-GK09-2-NPC45-D+C/GC-GK09-2-NPC45-D+C.gaf"
#define MS_POS                          Vec2(1272,219)
#define MS_ZORDER                       1
#define NPC_KEY                         "NPC-045"
#define NPC_COLLECT_DURATION            1.5f
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define HINT_PNG_FILE                   "GC-GK09-2-TM01.png"
#define HINT_LIGHT_PNG_FILE             "GC-GK09-2-TM02.png"
#define HINT_POS                        Vec2(1605,173)
#define HINT_ZORDER                     2
#define NPC_ENTER_BAG_MUSIC_FILE        "res/audio/npccollect/NPCjinbeibao.mp3"
#define SHAWDOW_PNG_FILE                "GC-GK09-2-DA01-ying.png"
#define SHAWDOW_POS                     Vec2(572, 291)
#define SHAWDOW_END_POS                 Vec2(1605,173)
#define SHAWDOW_ZORDER                  1

/*
 *  constructor
 */
Round045_02::Round045_02()
{
    m_answersPos[0] = Vec2(572, 291);
    m_answersPos[1] = Vec2(812, 304);
    m_answersPos[2] = Vec2(1122, 307);
    
}

/*
 *  destructor
 */
Round045_02::~Round045_02()
{
    cleanUp();
}

//
void Round045_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

//
Round045_02* Round045_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round045_02();
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
bool Round045_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;

    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    m_pHintImageView = ImageView::create(HINT_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pHintImageView->setPosition(HINT_POS);
    m_pHintImageView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    this->addChild(m_pHintImageView,HINT_ZORDER);
    
    m_pShawdowSp = Sprite::createWithSpriteFrameName(SHAWDOW_PNG_FILE);
    m_pShawdowSp->setPosition(SHAWDOW_POS);
    this->addChild(m_pShawdowSp, SHAWDOW_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK09-2-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setPosition(m_answersPos[i]);
        m_pAnswersSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
    }

    return true;
}

// 进入本回合
void Round045_02::onEnterRound()
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
        m_pAnswersSp[0]->runAction(FadeOut::create(0.3f));
        m_pShawdowSp->setVisible(false);
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round045_02::changeScaleAnswersSchedule), 0.6f, -1, 0);   //
    
}

//上升答案以显示
void Round045_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round045_02::changeScaleAnswersSchedule));
        changeScaleHint();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

void Round045_02::changeScaleHint()
{
    m_pHintImageView->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),
                                                 CallFunc::create([this]{
        m_pHintImageView->loadTexture(HINT_LIGHT_PNG_FILE,ui::Widget::TextureResType::PLIST);
    }),
                                                 ScaleTo::create(0.4f, 1.0f),
                                                 CallFunc::create([this]{
        m_pHintImageView->loadTexture(HINT_PNG_FILE,ui::Widget::TextureResType::PLIST);
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round045_02::openTouch()
{
    ///////////////////
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
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        if (index == 0) {
            m_pShawdowSp->setVisible(false);
        }
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintImageView->getContentSize().width,
                         m_pHintImageView->getContentSize().height);
        if (rect.containsPoint(m_pHintImageView->convertTouchToNodeSpace(touch)))
        {
            if (index == 0) {   //答对
                target->setPosition(m_pHintImageView->getPosition());
                target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.15f, 1.0f), NULL));
                target->runAction(Sequence::create(RotateBy::create(0.1f, -15),RotateBy::create(0.1f, 15.0f),RotateBy::create(0.1f, -10.0f),RotateBy::create(0.1f, 10.0f),RotateBy::create(0.1f, -5.0f),RotateBy::create(0.1f, 5.0f), NULL));
                if (index == 0) {
                    m_pShawdowSp->setVisible(true);
                    m_pShawdowSp->setPosition(SHAWDOW_END_POS);
                }
                
                m_pHintImageView->setVisible(false);
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersPos[index])),CallFunc::create([this,target,index]{
                    if (index == 0) {
                        m_pShawdowSp->setVisible(true);
                    }
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersPos[index]);
                }), NULL));
            }
        } else
        {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersPos[index])),CallFunc::create([this,target,index]{
                if (index == 0) {
                    m_pShawdowSp->setVisible(true);
                }
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_answersPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round045_02::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER,true);
}

/*
 *  庆祝
 */
void Round045_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round045_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
/*  处理npc收集 */
void Round045_02::processNpcCollect()
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

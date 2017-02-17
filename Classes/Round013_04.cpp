//
//  Round013_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/3/27.
//
//

#include "Round013_04.h"
#include "StageScene.h"
#include "ThemeSelScene.h"
#include "StageDataManager.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SelStageScene.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "AppConfig.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define MOVE_ACTOR_TIME                 5.0f                                // 分享界面ok过后主角移动时间
#define MOV_TIME                        0.2f                                // 答错回原位置移动时间

#define NPC_KEY                         "NPC-069"

#define ROBOT_CONFIG1                   "res/stage013/XQ-GK01-4-DA01-D+C.json"
#define ROBOT_CONFIG2                   "res/stage013/XQ-GK01-4-DA02-D+C.json"
#define ROBOT_CONFIG3                   "res/stage013/XQ-GK01-4-DA03-D+C.json"
#define ROBOT_CONFIG4                   "res/stage013/XQ-GK01-4-DA04-D+C.json"
#define ROBOT_CONFIG5                   "res/stage013/XQ-GK01-4-DA05-D+C.json"
#define ROBOT_GAF1      "res/stage013/XQ-GK01-4-DA01-D+C/XQ-GK01-4-DA01-D+C.gaf"
#define ROBOT_GAF2      "res/stage013/XQ-GK01-4-DA02-D+C/XQ-GK01-4-DA02-D+C.gaf"
#define ROBOT_GAF3      "res/stage013/XQ-GK01-4-DA03-D+C/XQ-GK01-4-DA03-D+C.gaf"
#define ROBOT_GAF4      "res/stage013/XQ-GK01-4-DA04-D+C/XQ-GK01-4-DA04-D+C.gaf"
#define ROBOT_GAF5      "res/stage013/XQ-GK01-4-DA05-D+C/XQ-GK01-4-DA05-D+C.gaf"

//
#define ROBOT_TARGET      "res/stage013/XQ-GK01-4-TM01.png"
#define ROBOT_TARGETH     "res/stage013/XQ-GK01-4-TM01-2.png"

#define SCALE_ORIGIN                0.34
#define SCALE_FACTOR                1.2
#define SCALE_DURATION              0.3f

// 门
#define DOOR_FILE                       "res/stage013/XQ-GK01-4-IT01.png"

#define OVER_FILE                       "res/stage013/XQ-GK01-BJ3_002-zhegai.png"
#define OVER_TAG                        501

//
#define NPC_COLLECT_DURATION                1.5f

/*
 *  constructor
 */
Round013_04::Round013_04() {}

/*
 *  destructor
 */
Round013_04::~Round013_04()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF1);
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF2);
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF3);
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF4);
    pGafAssetCache->removeGafAssetForKey(ROBOT_GAF5);
}

//
Round013_04* Round013_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round013_04();
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
bool Round013_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf(ROBOT_GAF1);
    appendAsyncLoadGaf(ROBOT_GAF2);
    appendAsyncLoadGaf(ROBOT_GAF3);
    appendAsyncLoadGaf(ROBOT_GAF4);
    appendAsyncLoadGaf(ROBOT_GAF5);
    
    appendAsyncLoadTexture(ROBOT_TARGET);
    appendAsyncLoadTexture(ROBOT_TARGETH);
    appendAsyncLoadTexture(DOOR_FILE);
    appendAsyncLoadTexture(OVER_FILE);
    
    appendRemovedTexture(DOOR_FILE);
    appendRemovedTexture(ROBOT_TARGET);
    appendRemovedTexture(ROBOT_TARGETH);
    appendRemovedTexture(OVER_FILE);
    
    preLoadRes();
    return true;
}

void Round013_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    m_pRobotActor[0] = Actor::createWithGafConfigFile(ROBOT_CONFIG1);
    m_pRobotActor[1] = Actor::createWithGafConfigFile(ROBOT_CONFIG2);
    m_pRobotActor[2] = Actor::createWithGafConfigFile(ROBOT_CONFIG3);
    m_pRobotActor[3] = Actor::createWithGafConfigFile(ROBOT_CONFIG4);
    m_pRobotActor[4] = Actor::createWithGafConfigFile(ROBOT_CONFIG5);
    
    Vec2 robotPos[5];
    float fx = 660.0f, fy = 550.0f, offsetx = 260.0f;
    for (long i = 0; i < 5; i++) {
        m_pRobotActor[i]->setUserData((void*)i);
        m_pRobotActor[i]->playWithSequenceAliasName("standby", true);
        robotPos[i] = Vec2(fx + offsetx * i, fy);
        addChild(m_pRobotActor[i], 2);
    } /*for*/
    
    // 随机
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5)
    {
        unsigned int index = static_cast<unsigned int>(rand() % 5);
        if (selected[index]) continue;
        selected[index] = true;
        m_pRobotActor[all]->setPosition(robotPos[index]);
        m_originalPoints[all] = robotPos[index];
        all++;
    } /*while*/
    
    fx = 1000.0f, fy = 430.0f, offsetx = 320.0f;
    for (int i = 0; i < 3; i++)
    {
        m_targetSp[i] = Sprite::create(ROBOT_TARGET);
        m_targetSp[i]->setPosition(fx + i * offsetx, fy);
        m_targetSp[i]->setScale(SCALE_ORIGIN);
        addChild(m_targetSp[i], 1);
        
        auto hsprite = Sprite::create(ROBOT_TARGETH);
        hsprite->setPosition(fx + i * offsetx, fy);
        hsprite->setScale(SCALE_ORIGIN);
        hsprite->setVisible(false);
        addChild(hsprite, 2);
        m_targetSp[i]->setUserObject(hsprite);
    } /*for*/
    
    // 门
    m_doorSp = Sprite::create(DOOR_FILE);
    addChild(m_doorSp, 10);
    m_doorSp->setPosition(Vec2(2349.0f, 802.0f));
    
    // 加挡板
    m_OverSp = Sprite::create(OVER_FILE);
    m_OverSp->setAnchorPoint(Vec2::ZERO);
    
    m_pStageScene->addOverLeaderNode(m_OverSp, Vec2(13588.0f - m_OverSp->getContentSize().width, 203.0f), OVER_TAG);
}

/*
 *  进入回合
 */
void Round013_04::onEnterRound()
{
    auto cb = CallFunc::create([this] {
        auto hsprite = (Sprite*)m_targetSp[m_numberIndex]->getUserObject();
        hsprite->setVisible(false);
        m_targetSp[m_numberIndex]->setVisible(true);
        scaleCompleteCB();
    });
    auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN),
                                     ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN), cb, NULL);
    m_targetSp[m_numberIndex]->setVisible(false);
    auto hsprite = (Sprite*)m_targetSp[m_numberIndex]->getUserObject();
    hsprite->setVisible(true);
    hsprite->runAction(scaleSeq);
}

//
void Round013_04::scaleCompleteCB()
{
    if (++m_numberIndex == 3)
    {
        openTouch();
        auto dispatcher = Director::getInstance()->getEventDispatcher();
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
           
           float moveOffset = 620.0f;
           auto moveCB = std::bind(&Round013_04::moveCB, this);
           m_pStageScene->manualScroll(moveOffset, moveCB);
       }
       );
    } else {
        auto cb = CallFunc::create([this] {
            auto hsprite = (Sprite*)m_targetSp[m_numberIndex]->getUserObject();
            hsprite->setVisible(false);
            m_targetSp[m_numberIndex]->setVisible(true);
            scaleCompleteCB();
        });
        auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                                         ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN),
                                         ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN * SCALE_FACTOR),
                                         ScaleTo::create(SCALE_DURATION, SCALE_ORIGIN), cb, NULL);
        m_targetSp[m_numberIndex]->setVisible(false);
        auto hsprite = (Sprite*)m_targetSp[m_numberIndex]->getUserObject();
        hsprite->setVisible(true);
        hsprite->runAction(scaleSeq);
    }
}

//
void Round013_04::moveCB()
{
    // 门右移动
    auto doorMoveCompletedCB = CallFunc::create([this] {
        const float moveOffset = 2000.0f;
        m_pStageScene->getBeibeiActor()->walk(); m_pStageScene->getAimeeActor()->walk();
        m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(moveOffset, 0));
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round013_04::fadeOutLayer), MOVE_ACTOR_TIME);    //退出
    });
    m_doorSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_doorSp->getContentSize().width + 16.0f, 0.0f)), Hide::create(), doorMoveCompletedCB, NULL));
}

/*
 *  开启触摸
 */
void Round013_04::openTouch()
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
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            reorderChild(target, 3);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos = Vec2::ZERO;
        
        long index = (long)target->getUserData();
        m_movePos.x = m_originalPoints[index].x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoints[index].y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        target->setScale(1.0f);
        
        long robotNumber = (long)target->getUserData();
        bool isTouchIn = false;
        int touchInTargetIndex = -1;
        
        for (int i = 0; i < 3; i++)
        {
            if (m_targetSp[i] == nullptr) {
                continue;
            }
            auto targetSp = m_targetSp[i];
            auto touchPoint = targetSp->convertTouchToNodeSpace(touch);
            Rect rect = Rect(0.0f, 0.0f,
                             targetSp->getContentSize().width, targetSp->getContentSize().height);
            if (rect.containsPoint(touchPoint)) {
                isTouchIn = true;
                touchInTargetIndex = i;
                break;
            }
        } /*for*/
        
        if (isTouchIn && (robotNumber + 1) % 2 == 1 /*答对了*/)
        {
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x, touch->getLocation().y));    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            auto newPos = m_targetSp[touchInTargetIndex]->getPosition();
            newPos.x -= m_targetSp[touchInTargetIndex]->getContentSize().width / 2.0f * SCALE_ORIGIN;
            newPos.y -= m_targetSp[touchInTargetIndex]->getContentSize().height / 2.0f * SCALE_ORIGIN;
            target->setPosition(newPos);
            target->playWithSequenceAliasName("trigger", true);
            m_targetSp[touchInTargetIndex]->removeFromParent();
            m_targetSp[touchInTargetIndex] = nullptr;
            
            if (++m_correctNumber == 3) /*全部答对*/ {
                answerCorrectCB();
            } else {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) { // 沮丧
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[robotNumber])),CallFunc::create([this,target,robotNumber] {
                this->reorderChild(target, 2);
                target->setPosition(m_originalPoints[robotNumber]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pRobotActor[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(),  m_pRobotActor[i]);
    }
}

/* 
 *  全部答对回调
 */
void Round013_04::answerCorrectCB()
{
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    for (unsigned int i = 0; i < 5; i++) {
        eventDispatcher->removeEventListenersForTarget(m_pRobotActor[i]);
    } /*for*/
    
    m_pStageScene->getRoundUINode()->stopReadEffect();          // 停止题目音效
    m_pStageScene->getRoundUINode()->endReadTopic();            // 停止读题目改变动
    
    float moveOffset = 2340.0f;
    m_pRobotActor[0]->runAction(MoveBy::create(MOVE_ACTOR_TIME, Vec2(moveOffset, 0.0f)));
    m_pRobotActor[2]->runAction(MoveBy::create(MOVE_ACTOR_TIME, Vec2(moveOffset, 0.0f)));
    m_pRobotActor[4]->runAction(MoveBy::create(MOVE_ACTOR_TIME, Vec2(moveOffset, 0.0f)));
    
    celebrate(CELEBRATE_DELAY); //庆祝
}

/*
 *  庆祝
 */
void Round013_04::celebrate(const float celebrateDelay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round013_04::endRound() {}

//回合结束调用
void Round013_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
        
    } else {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    
    processNpcCollect();    //npc收集
    std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
    MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}

/*  处理npc收集 */
void Round013_04::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x - 118, npcBagPos.y - 111);
    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(200, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),
                                       CallFunc::create([this]{m_pRobotActor[1]->setVisible(false);
    }), NULL);
    m_pRobotActor[1]->runAction(npcActions);
    
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
 *  渐变变黑退出
 */
void Round013_04::fadeOutLayer(float dt)
{
    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width + 620.0f, size.height);
    addChild(m_LayerColor, 110);
    schedule([this](float dt) {
        if (m_opacity == 155) {
            unschedule("add");
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            } else {
                // 过关
                m_pStageScene->stageClear();
                //试玩时停止播放背景音乐
                if (m_pStageScene->isTrial())
                {
                    MyAudioEngine::getInstance()->stopAudio(0,true);
                }
            }
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.005f, "add");
}

//分享回调
void Round013_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round013_04::roundEndShareCallBack, this, true));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round013_04::roundEndShareCallBack, this, false));
}

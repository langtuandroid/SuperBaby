//
//  Round023_03.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round023_03.h"
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
#define STAGE_IMAGE_FILE        "res/stage023/XQ-GK11-IT.png"
#define STAGE_PLIST_FILE        "res/stage023/XQ-GK11-IT.plist"
// 庆祝延时
#define CELEBRATE_DELAY                     5.0f
// npc
#define NPC_GAF_FILE                      "res/stage023/XQ-GK11-3-SM-D/XQ-GK11-3-SM-D.gaf"
#define ROTATE_ACTION_TAG       501

/*
 *  constructor
 */
Round023_03::Round023_03()
{
    srand(unsigned(time(0)));
}

/*
 *  destructor
 */
Round023_03::~Round023_03()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(NPC_GAF_FILE);
}

//
Round023_03* Round023_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round023_03();
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
bool Round023_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf(NPC_GAF_FILE);
    preLoadRes();
    return true;
}

//
void Round023_03::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // npc
    m_pNpcActor = Actor::createWithGafFile(NPC_GAF_FILE, true, 1.0f, Vec2(-100.0f, 428.0f));
    m_pNpcActor->setSize(Size(312.0f, 286.0f));
    m_pNpcActor->setPosition(Vec2(1580.0f, 1042.0f));
    addChild(m_pNpcActor, 1);
    
    // 大图
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 1);
    
    m_grapePos[0] = Vec2(388.0f, 1286.0f);
    m_grapePos[1] = Vec2(822.0f, 1328.0f);
    m_grapePos[2] = Vec2(1016.0f, 1152.0f);
    m_grapePos[3] = Vec2(1252.0f, 1336.0f);
    m_grapePos[4] = Vec2(1470.0f, 1008.0f);
    char szBuffer[64];
    for (int long i = 0; i < 5; i++)
    {
        sprintf(szBuffer, "XQ-GK11-3-DA%02ld.png", i + 1);
        m_pGrapeSps[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pGrapeSps[i]->setUserData((void*)(i + 5));
        m_pGrapeSps[i]->setAnchorPoint(Vec2(0.5f, 1.0f));
        m_pGrapeSps[i]->setPosition(m_grapePos[i]);
        rotateGrapeSp(i);
        m_pSBN->addChild(m_pGrapeSps[i], 2);
    } /*for*/
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    while (all < 5)
    {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randomIndex]) continue;
        selected[randomIndex] = true;
        m_pGrapeSps[all]->setPosition(m_grapePos[randomIndex]);
        m_originalPoints[all] = m_grapePos[randomIndex];
        all++;
    } /*while*/
    
    m_basketSp = Sprite::createWithSpriteFrameName("XQ-GK11-3-DA06.png");
    m_basketSp->setPosition(Vec2(1042.0f, 448.0f));
    m_pSBN->addChild(m_basketSp, 1);
    
    auto pPlatformSp = Sprite::createWithSpriteFrameName("XQ-GK11-4-IT01.png");
    pPlatformSp->setPosition(Vec2(3836.0f, 182.0f));
    addChild(pPlatformSp, 1);
}

//
void Round023_03::rotateGrapeSp(const long index)
{
    const float rot = 3.0f;
    int ridx = rand() % 5;
    Sequence* sq = nullptr;
    if (ridx % 2 == 0) {
        sq = Sequence::create(RotateTo::create(1.0, rot), RotateTo::create(1.0, 0.0f),
                                   RotateTo::create(1.0, -rot), RotateTo::create(1.0, 0.0f), NULL);
    } else {
        sq = Sequence::create(RotateTo::create(1.0, -rot), RotateTo::create(1.0, 0.0f),
                              RotateTo::create(1.0, rot), RotateTo::create(1.0, 0.0f), NULL);
    }
    auto repeatForever = RepeatForever::create(sq);
    repeatForever->setTag(ROTATE_ACTION_TAG);
    m_pGrapeSps[index]->runAction(repeatForever);
}

/*
 *  进入回合
 */
void Round023_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("endRound", [this, dispatcher] (EventCustom *)
    {
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
       dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
}

/* 
 *  开启触摸
 */
void Round023_03::openTouch()
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
            m_touchBeganPoint = touch->getLocation();
            target->stopActionByTag(ROTATE_ACTION_TAG);
            target->setScale(1.2f);
            m_pSBN->reorderChild(target, 3);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        long index = (long)target->getUserData() - 5;
        m_movePos.x = m_originalPoints[index].x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoints[index].y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long number = (long)target->getUserData();
        long index = number - 5;
        
        target->setScale(1.0f);
        m_pSBN->reorderChild(target, 2);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f,
                         m_basketSp->getContentSize().width,
                         m_basketSp->getContentSize().height);
        if (rect.containsPoint(m_basketSp->convertTouchToNodeSpace(touch)) )
        {
            if (number % 2 == 1)
            {
                isCorrect = true;
            }
        }
        if (isCorrect) /* 答题正确 */
        {
            m_numberOfCorrect++;
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            //
            placeGrapeSpInBasket(m_numberOfCorrect, target);
            if (3 == m_numberOfCorrect) /*全部答对*/
            {
                for (int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pGrapeSps[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round023_03::fadeBasketAndGrapes), 3.0f);
            } else {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
                if (m_celebrateCB)
                {
                    m_celebrateCB(1);
                }
            }
        } else {
            if (m_anwerErrorCB)
            {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答错时主角说话
                playLeadAnsErrEff();
                m_anwerErrorCB();
            }
            auto retCB = CallFunc::create([this, target, index]
            {
                target->setPosition(m_originalPoints[index]);
                rotateGrapeSp(index);
            });
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(0.2f, m_originalPoints[index])), retCB, NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_originalPoints[index]);
        target->setScale(1.0f);
        m_pSBN->reorderChild(target, 2);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pGrapeSps[0]);
    for (int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pGrapeSps[i]);
    }
}

//
void Round023_03::placeGrapeSpInBasket(const int numberOfPlaced, Sprite* pGrapeSp)
{
    CCASSERT(pGrapeSp != nullptr, "invalid pGrapeSp");
    
    auto basketPos = m_basketSp->getPosition();
    float ang = 0.0f;
    Vec2 pos = Vec2::ZERO;
    
    if (1 == numberOfPlaced) {
        ang = -45.0f;
        pos.x = basketPos.x - 268.0f;
        pos.y = basketPos.y + 194.0f;
    } else if (2 == numberOfPlaced) {
        ang = 0.0f;
        pos.x = basketPos.x;
        pos.y = basketPos.y + 294.0f;
    } else {
        ang = 45.0f;
        pos.x = basketPos.x + 268.0f;
        pos.y = basketPos.y + 194.0f;
    }
    pGrapeSp->setRotation(ang);
    pGrapeSp->setPosition(pos);
    pGrapeSp->stopAllActions();
}

void Round023_03::fadeBasketAndGrapes(float)
{
    m_basketSp->runAction(FadeOut::create(0.5f));
    m_pGrapeSps[0]->runAction(FadeOut::create(0.5f));
    m_pGrapeSps[2]->runAction(FadeOut::create(0.5f));
    m_pGrapeSps[4]->runAction(FadeOut::create(0.5f));
}

/*
 *  庆祝
 */
void Round023_03::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB)
    {
        m_celebrateCB(1);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round023_03::endRound()
{
    m_pStageScene->manualScroll(2924.0f, [this]
    {
        auto movedCB = [this]
        {
            if (m_completeCB)
            {
                m_completeCB();
            }
        };
        m_pStageScene->moveLeadingActorCB(1.88f, Vec2(480.0f, 0.0f), movedCB);
    });
}

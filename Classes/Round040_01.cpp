//
//  Round026_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#include "Round040_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage040/GC-GK04-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage040/GC-GK04-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define GOUXUAN_GAF_FILE                "res/common/SL-goxuan/SL-goxuan.gaf"
#define GOUXUAN_ZORDER                  2
#define SCULPTURES_ZORDER               1

/*
 *  constructor
 */
Round040_01::Round040_01()
{
    m_sculpturesPos[0] = Vec2(670, 316);
    m_sculpturesPos[1] = Vec2(1355, 318);
    
    m_leftHouseGouxuanPos[0] = Vec2(331, 1238);
    m_leftHouseGouxuanPos[1] = Vec2(456, 881);
    
    m_rightHouseGouxuanPos[0] = Vec2(999, 1238);
    m_rightHouseGouxuanPos[1] = Vec2(1124, 881);
    
    m_touchLeftRects[0] = Rect(505,904,200,200);
    m_touchLeftRects[1] = Rect(609,517,200,200);
    
    m_touchRightRects[0] = Rect(1196,904,200,200);
    m_touchRightRects[1] = Rect(1300,517,200,200);
    
    m_touchArea = Rect(458.0f, 336.0f, 1320.0f, 800.0f);
    
    m_difsPos[0] = Vec2(395,639);
    m_difsPos[1] = Vec2(504,305);
    
    for (auto &it : m_answered) {
        it = false;
    } /*for*/
}

/*
 *  destructor
 */
Round040_01::~Round040_01()
{
    cleanUp();
}

//
void Round040_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(GOUXUAN_GAF_FILE);
}

//
Round040_01* Round040_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round040_01();
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
bool Round040_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "GC-GK04-1-TM02.png");
        m_pSculpturesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pSculpturesSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(m_pSculpturesSp[i],SCULPTURES_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "GC-GK04-1-TM%02d.png",i+3);
        m_pDifSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pDifSp[i]->setPosition(m_difsPos[i]);
        m_pSculpturesSp[1]->addChild(m_pDifSp[i]);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pSculpturesSp[all]->setPosition(m_sculpturesPos[randIndex]);
        all++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }

    return true;
}

// 进入本回合
void Round040_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round040_01::changeScaleSculpturesSchedule), 0.6f, -1, 0);   //
    
}

//依次放大城堡以提示
void Round040_01::changeScaleSculpturesSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round040_01::changeScaleSculpturesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pSculpturesSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round040_01::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        if (m_touchArea.containsPoint(touchPoint))
        {
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        
        bool hasAnswerRight = false;
        for (unsigned int i = 0; i < 2; i++) {
            if (m_answered[i]) continue;
            if (m_touchLeftRects[i].containsPoint(touchPoint) || m_touchRightRects[i].containsPoint(touchPoint)) {
                hasAnswerRight = true;
                if (m_pGouActor != nullptr) {
                    auto actor1 = Actor::createWithActor(m_pGouActor, false);
                    actor1->setPosition(m_leftHouseGouxuanPos[i]);
                    this->addChild(actor1, GOUXUAN_ZORDER);
                    m_pGouxuanActors.push_back(actor1);
                } else {
                    m_pGouActor = Actor::createWithGafFile(GOUXUAN_GAF_FILE, false);
                    m_pGouActor->setPosition(m_leftHouseGouxuanPos[i]);
                    this->addChild(m_pGouActor, GOUXUAN_ZORDER);
                    m_pGouxuanActors.push_back(m_pGouActor);
                }
                auto actor2 = Actor::createWithActor(m_pGouActor, false);
                actor2->setPosition(m_rightHouseGouxuanPos[i]);
                this->addChild(actor2, GOUXUAN_ZORDER);
                m_answered[i] = true;
                m_pGouxuanActors.push_back(actor2);
            }
        }
        
        if (hasAnswerRight) {
            m_pStageScene->playAnswerCorrectEffect(touchPoint);    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            bool isAll = true;
            for (auto &it : m_answered) {
                if (!it) {
                    isAll = false;
                    break;
                }
            } /*for*/
            if (isAll) /*全部答对*/ {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(1.5f);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
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
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, this);
}

/*
 *  庆祝
 */
void Round040_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round040_01::endRound()
{
    for (auto &it : m_pGouxuanActors) {
        it->removeFromParent();
    } /*for*/
    m_pGouxuanActors.clear();
    if (m_completeCB) {
        m_completeCB();
    }
}

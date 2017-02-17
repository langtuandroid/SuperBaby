//
//  Round036_01.cpp
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#include "Round036_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage036/HY-GK12-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage036/HY-GK12-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(1118,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define GOUXUAN_GAF_FILE                "res/common/SL-goxuan/SL-goxuan.gaf"
#define GOUXUAN_ZORDER                  5
#define FISHES_ZORDER                   3

/*
 *  constructor
 */
Round036_01::Round036_01()
:m_InitAniIndex(0)
{
    m_fishesPos[0] = Vec2(746, 710);
    m_fishesPos[1] = Vec2(1404, 710);
    
    m_leftHouseGouxuanPos[0] = Vec2(271, 1138);
    m_leftHouseGouxuanPos[1] = Vec2(576, 1021);
    
    m_rightHouseGouxuanPos[0] = Vec2(1349, 1109);
    m_rightHouseGouxuanPos[1] = Vec2(1002, 1023);
    
    m_touchLeftRects[0] = Rect(473, 800, 150, 150);
    m_touchLeftRects[1] = Rect(799, 693, 100, 100);
    
    m_touchRightRects[0] = Rect(1529, 774, 150, 150);
    m_touchRightRects[1] = Rect(1239, 692, 100, 100);
    
    m_touchArea = Rect(415.0f, 700.0f, 1320.0f, 400.0f);
    for (auto &it : m_answered) {
        it = false;
    } /*for*/
}

/*
 *  destructor
 */
Round036_01::~Round036_01()
{
    cleanUp();
}

//
void Round036_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(GOUXUAN_GAF_FILE);
}

//
Round036_01* Round036_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round036_01();
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
bool Round036_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK12-1-TM%02ld.png",i+1);
        m_pFishesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pFishesSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        m_pFishesSp[i]->setPosition(m_fishesPos[i]);
        this->addChild(m_pFishesSp[i],FISHES_ZORDER);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
//    drawtest(Rect(415.0f, 700.0f, 1320.0f, 400.0f));
    return true;
}

void Round036_01::drawtest(const Rect& rect)
{
    DrawNode* drawNode = DrawNode::create();
    this->addChild(drawNode,50);
    Vec2 point[4];
    point[0] = Vec2(rect.origin.x, rect.origin.y);
    point[1] = Vec2(rect.origin.x, rect.origin.y + rect.size.height);
    point[2] = Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
    point[3] = Vec2(rect.origin.x + rect.size.width, rect.origin.y);
    drawNode->drawPolygon(point, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 1, 0, 1));
}

// 进入本回合
void Round036_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round036_01::changeScaleFishesSchedule), 0.6f, -1, 0);   //
    
}

//依次放大鱼以提示
void Round036_01::changeScaleFishesSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_01::changeScaleFishesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pFishesSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round036_01::openTouch()
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

//答对后移动鱼
void Round036_01::moveFishes()
{
    m_pFishesSp[0]->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(1800.0f, 0.0f)),CallFunc::create([this]{
        m_pFishesSp[0]->setVisible(false);
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
    m_pFishesSp[1]->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(-1800.0f, 0.0f)),CallFunc::create([this]{
        m_pFishesSp[1]->setVisible(false);
    }), NULL));
}

/*
 *  庆祝
 */
void Round036_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round036_01::endRound()
{
    for (auto &it : m_pGouxuanActors) {
        it->removeFromParent();
    } /*for*/
    m_pGouxuanActors.clear();
    
    moveFishes();
}

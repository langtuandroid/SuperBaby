//
//  Round044_03.cpp
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#include "Round044_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define GOUXUAN_GAF_FILE                "res/common/SL-goxuan/SL-goxuan.gaf"
#define GOUXUAN_ZORDER                  2
#define PICTURE_ZORDER                  1
#define GOU_GAF_LEFT_TAG                300
#define GOU_GAF_RIGHT_TAG               301

/*
 *  constructor
 */
Round044_03::Round044_03()
{
    m_picturesPos[0] = Vec2(570, 1242);
    m_picturesPos[1] = Vec2(1461, 1242);
    
    m_leftHouseGouxuanPos = Vec2(137, 1251);
    
    m_rightHouseGouxuanPos = Vec2(1026, 1251);
    
    m_touchLeftRect = Rect(327,882,200,200);
    
    m_touchRightRect = Rect(1220,882,200,200);
    
    m_touchArea = Rect(236.0f, 469.0f, 1720.0f, 800.0f);

}

/*
 *  destructor
 */
Round044_03::~Round044_03()
{
    cleanUp();
}

//
void Round044_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(GOUXUAN_GAF_FILE);
}

//
Round044_03* Round044_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round044_03();
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
bool Round044_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "GC-GK08-3-TM%02ld.png",i+1);
        m_pPicturesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pPicturesSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        this->addChild(m_pPicturesSp[i],PICTURE_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pPicturesSp[all]->setPosition(m_picturesPos[randIndex]);
        all++;
    } /*while*/
    
    return true;
}

void Round044_03::drawtest(const Rect& rect)
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
void Round044_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round044_03::changeScalePicturesSchedule), 0.6f, -1, 0);   //
    
}

//依次放大城堡以提示
void Round044_03::changeScalePicturesSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round044_03::changeScalePicturesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pPicturesSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round044_03::openTouch()
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
        if (m_touchLeftRect.containsPoint(touchPoint) || m_touchRightRect.containsPoint(touchPoint)) {
            hasAnswerRight = true;
            if (m_pGouActor != nullptr) {
                auto actor1 = Actor::createWithActor(m_pGouActor, false);
                actor1->setPosition(m_leftHouseGouxuanPos);
                this->addChild(actor1, GOUXUAN_ZORDER, GOU_GAF_LEFT_TAG);
            } else {
                m_pGouActor = Actor::createWithGafFile(GOUXUAN_GAF_FILE, false);
                m_pGouActor->setPosition(m_leftHouseGouxuanPos);
                this->addChild(m_pGouActor, GOUXUAN_ZORDER, GOU_GAF_LEFT_TAG);
            }
            auto actor2 = Actor::createWithActor(m_pGouActor, false);
            actor2->setPosition(m_rightHouseGouxuanPos);
            this->addChild(actor2, GOUXUAN_ZORDER, GOU_GAF_RIGHT_TAG);
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
            
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            celebrate(1.5f);
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
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
void Round044_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round044_03::endRound()
{
    this->getChildByTag(GOU_GAF_LEFT_TAG)->removeFromParent();
    this->getChildByTag(GOU_GAF_RIGHT_TAG)->removeFromParent();
    if (m_completeCB) {
        m_completeCB();
    }
}

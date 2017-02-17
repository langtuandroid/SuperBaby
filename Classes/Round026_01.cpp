//
//  Round026_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#include "Round026_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage026/HY-GK02-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage026/HY-GK02-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define GOUXUAN_GAF_FILE                "res/common/SL-goxuan/SL-goxuan.gaf"
#define GOUXUAN_ZORDER                  2
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(1024,1026)
#define BOGUANG_GAF_ZORDER              1
#define HOUSE_ZORDER                    2

/*
 *  constructor
 */
Round026_01::Round026_01()
:m_InitAniIndex(0)
{
    m_housesPos[0] = Vec2(913, 357);
    m_housesPos[1] = Vec2(1550, 357);
    
    m_leftHouseGouxuanPos[0] = Vec2(661, 1188);
    m_leftHouseGouxuanPos[1] = Vec2(656, 691);
    m_leftHouseGouxuanPos[2] = Vec2(873, 903);
    
    m_rightHouseGouxuanPos[0] = Vec2(1299, 1189);
    m_rightHouseGouxuanPos[1] = Vec2(1292, 693);
    m_rightHouseGouxuanPos[2] = Vec2(1509, 903);
    
    m_touchLeftRects[0] = Rect(820,862,200,200);
    m_touchLeftRects[1] = Rect(820,374,200,100);
    m_touchLeftRects[2] = Rect(1065,503,150,200);
    
    m_touchRightRects[0] = Rect(1457,862,200,200);
    m_touchRightRects[1] = Rect(1457,374,200,100);
    m_touchRightRects[2] = Rect(1702,503,150,200);
    
    m_touchArea = Rect(458.0f, 336.0f, 1320.0f, 800.0f);
    for (auto &it : m_answered) {
        it = false;
    } /*for*/
}

/*
 *  destructor
 */
Round026_01::~Round026_01()
{
    cleanUp();
}

//
void Round026_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(GOUXUAN_GAF_FILE);
}

//
Round026_01* Round026_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round026_01();
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
bool Round026_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK02-1-TM%02ld.png",i+1);
        m_pHousesSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pHousesSp[i]->setAnchorPoint(Vec2(0.5f,0.0f));
        this->addChild(m_pHousesSp[i],HOUSE_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pHousesSp[all]->setPosition(m_housesPos[randIndex]);
        all++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

void Round026_01::drawtest(const Rect& rect)
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
void Round026_01::onEnterRound()
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

    this->schedule(CC_SCHEDULE_SELECTOR(Round026_01::changeScaleHousesSchedule), 0.6f, -1, 0);   //

}

//依次放大城堡以提示
void Round026_01::changeScaleHousesSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round026_01::changeScaleHousesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pHousesSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round026_01::openTouch()
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
        for (unsigned int i = 0; i < 3; i++) {
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
void Round026_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round026_01::endRound()
{
    for (auto &it : m_pGouxuanActors) {
        it->removeFromParent();
    } /*for*/
    m_pGouxuanActors.clear();
    if (m_completeCB) {
        m_completeCB();
    }
}

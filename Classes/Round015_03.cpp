//
//  Round015_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#include "Round015_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PNG_FILE                     "res/stage015/XQ-GK03-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define CELEBRATE_DELAY                 5.0f
#define HINT_ZORDER                     2
#define FLOWER_BOARD_ZORDER             2
#define MAGIC_POOL_POS                  Vec2(1028,314)
#define MAGIC_POOL_PNG_FILE             "XQ-GK03-3-IT01.png"
#define MAGIC_POOL_ZORDER               1
#define UFO_GAF_CONFIG_FILE             "config/stage015/ufo.json"
#define STANDBY                         "standby"                                // 待机动画
#define TRIGGER                         "trigger"                                // 触发动画
#define UFO_GAF_POS                     Vec2(1500,1010)
#define UFO_GAF_ZORDER                  3
#define MAGIC_GAF_FILE                  "res/stage015/XQ-GK03-3-IT01/XQ-GK03-3-IT01.gaf"
#define MAGIC_GAF_POS                   Vec2(1008,314)
#define MAGIC_GAF_ZORDER                1

/*
 *  constructor
 */
Round015_03::Round015_03()
:m_InitAniIndex(0)
{
    
    m_sunflowersPos[0] = Vec2(668, 1004);
    m_sunflowersPos[1] = Vec2(884, 720);
    m_sunflowersPos[2] = Vec2(1140, 1084);
    m_sunflowersPos[3] = Vec2(1277, 726);
    
    m_sunflowerMovePos[0] = Vec2(1280, 900);
    m_sunflowerMovePos[1] = Vec2(1320, 815);
    m_sunflowerMovePos[2] = Vec2(1420, 940);
    m_sunflowerMovePos[3] = Vec2(1450, 820);
    
    m_sunflowerMoveRelativePos[0] = Vec2(20, 20);
    m_sunflowerMoveRelativePos[1] = Vec2(20, 30);
    m_sunflowerMoveRelativePos[2] = Vec2(10, 20);
    m_sunflowerMoveRelativePos[3] = Vec2(10, 30);
    
}

/*
 *  destructor
 */
Round015_03::~Round015_03()
{
    cleanUp();
}

//
void Round015_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage015/XQ-GK05-2-MS02-D+C/XQ-GK05-2-MS02-D+C.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey(MAGIC_GAF_FILE);
}

//
Round015_03* Round015_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round015_03();
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
bool Round015_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    srand((unsigned)time(0));
    m_curSameBallRandNum = rand()%2;
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        if (i == 0) {
            if (m_curCorrectNumber == 0) {
                sprintf(buffer, "XQ-GK03-3-DA01.png");
            }else
            {
                sprintf(buffer, "XQ-GK03-3-DA02.png");
            }
        }else
        {
            if (m_curCorrectNumber == 0) {
                sprintf(buffer, "XQ-GK03-3-DA02.png");
            }else
            {
                sprintf(buffer, "XQ-GK03-3-DA01.png");
            }
        }
        m_pSunflowers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pSunflowers[i]->setPosition(Vec2(MAGIC_POOL_POS.x+20, MAGIC_POOL_POS.y));
        m_pSunflowers[i]->setUserData((void*)i);
        m_pBN->addChild(m_pSunflowers[i], FLOWER_BOARD_ZORDER);
        m_pSunflowers[i]->setScale(0.5f);
        m_pSunflowers[i]->setOpacity(0);
    }
    
    m_pMagicPool = Sprite::createWithSpriteFrameName(MAGIC_POOL_PNG_FILE);
    m_pMagicPool->setPosition(MAGIC_POOL_POS);
    this->addChild(m_pMagicPool, MAGIC_POOL_ZORDER);
    
    m_pFlyShipActor = Actor::createWithGafConfigFile(UFO_GAF_CONFIG_FILE);
    m_pFlyShipActor->playWithSequenceAliasName(STANDBY,true);
    m_pFlyShipActor->setPosition(UFO_GAF_POS);
    m_pFlyShipActor->setSize(Size(200, 200));
    this->addChild(m_pFlyShipActor, UFO_GAF_ZORDER);
    
    m_pMagicActor = Actor::createWithGafFile(MAGIC_GAF_FILE);
    m_pMagicActor->setPosition(MAGIC_GAF_POS);
    m_pMagicActor->setPosAtNode(Vec2(-334, 548));
    this->addChild(m_pMagicActor, MAGIC_GAF_ZORDER);
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int flowerIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[flowerIndex]) continue;
        selected[flowerIndex] = true;
        m_sunflowersEndPos[all] =  m_sunflowersPos[flowerIndex];
        m_sunflowerMoveEndPos[all] = m_sunflowerMovePos[flowerIndex];
        m_sunflowerMoveRelativeEndPos[all] = m_sunflowerMoveRelativePos[flowerIndex];
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round015_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round015_03::appearSunflowersSchedule), 1.1f, -1, 0);
    
}

//出现花图片
void Round015_03::appearSunflowersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round015_03::appearSunflowersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    m_pSunflowers[m_InitAniIndex]->runAction(Sequence::create(Spawn::create(ScaleTo::create(1.0f, 1.0f),FadeIn::create(0.6f),RotateBy::create(1.0f, -360),MoveTo::create(1.0f, m_sunflowersEndPos[m_InitAniIndex]), NULL),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
}


/* 开启触摸 */
void Round015_03::openTouch()
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
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) {
                m_curCorrectNumber++;
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pSunflowers[i]);
                } /*for*/
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
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pSunflowers[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pSunflowers[i]);
    } /*for*/
    
}

//答对后改变飞船动作
void Round015_03::changeActor()
{
    m_pFlyShipActor->playWithSequenceAliasName(TRIGGER);
    m_pFlyShipActor->setAnimationFinishedPlayDelegate([this]
    {
        if (m_completeCB) {
            m_completeCB();
        }
    });
}

//渐隐魔法池
void Round015_03::fadeOutMagicPool()
{
    m_pMagicActor->setVisible(false);
    m_pMagicPool->runAction(FadeOut::create(0.3f));
}

//答对后，花图片动作
void Round015_03::chooseTrueSunflowersActions()
{
    for (unsigned int i = 0; i < 4; i++) {
        if (i == 3) {
            changeActor();              //改变飞船动作
            m_pSunflowers[i]->runAction(Sequence::create(Spawn::create(ScaleTo::create(1.0f, 0.5f),MoveTo::create(1.0f, m_sunflowerMoveEndPos[i]), NULL),CallFunc::create([this]{
                fadeOutMagicPool();         //渐隐魔法池
            }),MoveBy::create(0.5f, m_sunflowerMoveRelativeEndPos[i]),Spawn::create(ScaleTo::create(0.2f, 0.4f),MoveTo::create(0.2f, Vec2(1600, 1090)), NULL),CallFunc::create([this,i]{
                for (unsigned int i = 0; i < 4; i++) {
                    m_pSunflowers[i]->setVisible(false);
                }
            }), NULL));
        }else
        {
            m_pSunflowers[i]->runAction(Sequence::create(Spawn::create(ScaleTo::create(1.0f, 0.5f),MoveTo::create(1.0f, m_sunflowerMoveEndPos[i]), NULL),MoveBy::create(0.5f, m_sunflowerMoveRelativeEndPos[i]),Spawn::create(ScaleTo::create(0.2f, 0.4f),MoveTo::create(0.2f, Vec2(1600, 1090)), NULL), NULL));
        }
    }
}

/*
 *  庆祝
 */
void Round015_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round015_03::endRound()
{
    chooseTrueSunflowersActions();

}
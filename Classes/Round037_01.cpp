//
//  Round037_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#include "Round037_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage037/GC-GK01-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage037/GC-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define TOWER_ZORDER                    1

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FADE_TIME                       1.0f                                       // 渐变时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define PASS_GAF_FILE                   "res/common/yanshi02/yanshi02.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3
#define BEFORE_BACKGROUND_FILE          "res/stage037/GC-GK01-bjzhe.png"        //前景图
#define BEFORE_BACKGROUND_ZORDER        3
#define BACKGROUND_MUSIC_FILE           "res/selStage/04/audio/GC-BGM.mp3"

/*
 *  constructor
 */
Round037_01::Round037_01()
:m_InitAniIndex(0)
{
    m_mountsPos[0] = Vec2(460, 429);
    m_mountsPos[1] = Vec2(976, 429);
    m_mountsPos[2] = Vec2(1463, 429);
    
}

/*
 *  destructor
 */
Round037_01::~Round037_01()
{
    cleanUp();
}

//
Round037_01* Round037_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round037_01();
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
bool Round037_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //试玩时播放背景音乐
    if (m_pStageScene->isTrial())
    {
        MyAudioEngine::getInstance()->playAudio(BACKGROUND_MUSIC_FILE,true,true,0.4f);
    }
    
    m_pBeforeBackGroundSp = Sprite::create(BEFORE_BACKGROUND_FILE);
    m_pBeforeBackGroundSp->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pBeforeBackGroundSp->setPosition(Vec2(145.0f, 232.0f));
    this->addChild(m_pBeforeBackGroundSp, BEFORE_BACKGROUND_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK01-1-DA%02ld.png",i+1);
        m_pMounts[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pMounts[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        m_pMounts[i]->setPosition(m_mountsPos[i]);
        m_pBN->addChild(m_pMounts[i], TOWER_ZORDER);
        m_pMounts[i]->setUserData((void*)i);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

// 进入本回合
void Round037_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    if (!UserDefault::getInstance()->getBoolForKey("delaytimefour", false)) {
        UserDefault::getInstance()->setBoolForKey("delaytimefour", true);
        scheduleOnce([this](float){
            playPassActor();        //播放引导
        }, 7.0f, "delaytime");
    }

    this->schedule(CC_SCHEDULE_SELECTOR(Round037_01::scaleChangeMountSchedule), 0.4f, -1, 0);
}

//改变山的大小，以提示
void Round037_01::scaleChangeMountSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round037_01::scaleChangeMountSchedule));
        return;
    }
    
    m_pMounts[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 播放过关引导 */
void Round037_01::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->getRoundUINode()->addChild(colorLayer,PASS_GAF_ZORDER);
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, colorLayer);
    dispatcher->setPriority(listenerDrag, -130);
    Actor* passActor = Actor::createWithGafFile(PASS_GAF_FILE,false);
    passActor->setPosAtNode(Vec2(300, 500));
    passActor->setPosition(Vec2(350, 650));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]{
        colorLayer->removeFromParent();
        passActor->setVisible(false);
    });
}

//
void Round037_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(PASS_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(BEFORE_BACKGROUND_FILE);
}

/* 开启触摸 */
void Round037_01::openTouch()
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
        long index = (long)target->getUserData();
        
        target->setScale(1.0f);
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 2) {   //答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
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
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pMounts[i]);
                } /*for*/
                
            }else
            {
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
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pMounts[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pMounts[i]);
    } /*for*/
    
}

/*
 *  庆祝
 */
void Round037_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round037_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
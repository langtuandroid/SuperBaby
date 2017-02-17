//
//  Round025_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#include "Round025_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage025/HY-GK01-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage025/HY-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define TOWER_ZORDER                    1
#define SHIP_GAF_FILE                   "res/stage025/HY-GK01-1-chuan-D/HY-GK01-1-chuan-D.gaf"    //船的gaf文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FADE_TIME                       1.0f                                       // 灯塔渐变时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define PASS_GAF_FILE                   "res/common/yanshi02/yanshi02.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3
#define BACKGROUND_MUSIC_FILE           "res/selStage/03/audio/HY-BGM.mp3"
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(1024,1026)
#define BOGUANG_GAF_ZORDER              1

/*
 *  constructor
 */
Round025_01::Round025_01()
:m_InitAniIndex(0)
{
    m_towersPos[0] = Vec2(1038, 589);
    m_towersPos[1] = Vec2(1338, 589);
    m_towersPos[2] = Vec2(1638, 589);

}

/*
 *  destructor
 */
Round025_01::~Round025_01()
{
    cleanUp();
}

//
Round025_01* Round025_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round025_01();
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
bool Round025_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK01-1-TM%02ld.png",i+1);
        m_pTowers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTowers[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        m_pBN->addChild(m_pTowers[i], TOWER_ZORDER);
        m_pTowers[i]->setOpacity(0);
        m_pTowers[i]->setUserData((void*)i);
    }
    
    m_pShipActor = Actor::createWithGafFile(SHIP_GAF_FILE);
    m_pShipActor->setPosition(Vec2(100, 1300));
    this->addChild(m_pShipActor);
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int towerIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[towerIndex]) continue;
        selected[towerIndex] = true;
        m_pTowers[all]->setPosition(m_towersPos[towerIndex]);
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
void Round025_01::onEnterRound()
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
    if (!UserDefault::getInstance()->getBoolForKey("delaytimethree", false)) {
        UserDefault::getInstance()->setBoolForKey("delaytimethree", true);
        scheduleOnce([this](float){
            this->schedule(CC_SCHEDULE_SELECTOR(Round025_01::fadeInTowersSchedule), FADE_TIME - 0.5f, -1, 0);   //渐隐显示灯塔
        }, 7.0f, "delaytime");
    }else
    {
        this->schedule(CC_SCHEDULE_SELECTOR(Round025_01::fadeInTowersSchedule), FADE_TIME - 0.5f, -1, 0);   //渐隐显示灯塔
    }
    
}

//渐隐出现灯塔
void Round025_01::fadeInTowersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_01::fadeInTowersSchedule));
        if (!UserDefault::getInstance()->getBoolForKey("clickthree", false)) {
            playPassActor();        //播放引导
            UserDefault::getInstance()->setBoolForKey("clickthree", true);
        }
        return;
    }
    m_pTowers[m_InitAniIndex++]->runAction(FadeIn::create(FADE_TIME));
}

/* 播放过关引导 */
void Round025_01::playPassActor()
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
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]
    {
        colorLayer->removeFromParent();
        passActor->setVisible(false);
    });
}

//
void Round025_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(SHIP_GAF_FILE);
    
    GafAssetCache::getInstance()->removeGafAssetForKey(PASS_GAF_FILE);
}

/* 开启触摸 */
void Round025_01::openTouch()
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
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pTowers[i]);
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
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pTowers[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pTowers[i]);
    } /*for*/

}

//渐隐消失灯塔
void Round025_01::fadeOutTowersSchedule(float)
{
    if (m_InitAniIndex < 0) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_01::fadeOutTowersSchedule));
        return;
    }
    m_pTowers[--m_InitAniIndex]->runAction(FadeOut::create(FADE_TIME));
}

/*
 *  庆祝
 */
void Round025_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round025_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
    this->schedule(CC_SCHEDULE_SELECTOR(Round025_01::fadeOutTowersSchedule), FADE_TIME-0.5f, -1, 0);        //灯塔渐隐消失
}

//
//  Round037_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#include "Round037_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define NUMBER_MOVE_TIME                0.5f                                     // 数字上升时间
#define NUMBER_MOVE_OFFSETY             400                                     //数字移动距离
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define SEABIRD_MOVE_TIME               2.0f                                    //海鸟移动时间
#define SEABIRD_MOVE_OFFSETX            -2000                                   //海鸟移动距离
#define SEABIRD_GAF_FILE                "res/stage025/HY-GK01-2-TM01-D/HY-GK01-2-TM01-D.gaf"        //海鸟gaf文件
#define BN_PNG_FILE                    "res/stage037/GC-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define EFFECT_GAF_FILE                 "res/stage037/GC-GK01-2-TX/GC-GK01-2-TX.gaf"
#define EFFECT_GAF_ZORDER               3

/*
 *  constructor
 */
Round037_02::Round037_02()
:m_InitAniIndex(0)
{
    m_flowersPos[0] = Vec2(920, 1100);
    m_flowersPos[1] = Vec2(909, 639);
    m_flowersPos[2] = Vec2(1241, 729);
    m_flowersPos[3] = Vec2(1417, 1001);
}

/*
 *  destructor
 */
Round037_02::~Round037_02()
{
    cleanUp();
}

//
Round037_02* Round037_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round037_02();
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
bool Round037_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "GC-GK01-2-DA%02ld.png",i+1);
        m_pFlowers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pFlowers[i]->setUserData((void*)i);
        m_pFlowers[i]->setScale(0.0f);
        m_pBN->addChild(m_pFlowers[i]);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int flowerIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[flowerIndex]) continue;
        selected[flowerIndex] = true;
        m_pFlowers[all]->setPosition(m_flowersPos[flowerIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round037_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round037_02::scaleChangeFlowersSchedule), 0.3f, -1, 0);
}

//改变花的大小
void Round037_02::scaleChangeFlowersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round037_02::scaleChangeFlowersSchedule));
        return;
    }
    m_pFlowers[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}

//
void Round037_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

/* 开启触摸 */
void Round037_02::openTouch()
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
            target->setScale(1.2f);
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
            if (index == 1) {   //答对
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
                chooseTrueComplete(target);
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pFlowers[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pFlowers[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pFlowers[i]);
    } /*for*/
}

//答对后转动花，并播放特效
void Round037_02::chooseTrueComplete(Node* target)
{
    target->runAction(RepeatForever::create(RotateBy::create(0.5f, 180)));
    
    auto targetPos = target->getPosition();
    m_pEffectActor = Actor::createWithGafFile(EFFECT_GAF_FILE);
    m_pEffectActor->setPosAtNode(Vec2(-1024, 768));
    m_pEffectActor->setPosition(targetPos);
    addChild(m_pEffectActor, EFFECT_GAF_ZORDER);
}

/*
 *  庆祝
 */
void Round037_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

/*  回合结束回调 */
void Round037_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
    
}
//
//  Round026_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#include "Round026_02.h"
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
#define SHIPONE_GAF_FILE                "res/stage026/HY-GK02-2-IT01/HY-GK02-2-IT01.gaf"        //船gaf文件
#define SHIPTWO_GAF_FILE                "res/stage026/HY-GK02-2-IT02/HY-GK02-2-IT02.gaf"        //船gaf文件
#define SHIPTHREE_GAF_FILE              "res/stage026/HY-GK02-2-IT03/HY-GK02-2-IT03.gaf"        //船gaf文件
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(924,1026)

/*
 *  constructor
 */
Round026_02::Round026_02()
:m_InitAniIndex(0)
{
    
    m_shipsPos[0] = Vec2(702, 837);
    m_shipsPos[1] = Vec2(1647, 837);
    m_shipsPos[2] = Vec2(1154, 625);

}

/*
 *  destructor
 */
Round026_02::~Round026_02()
{
    cleanUp();
}

//
void Round026_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round026_02* Round026_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round026_02();
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
bool Round026_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor);
    
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 0) {
            m_pShipsActor[i] = Actor::createWithGafFile(SHIPONE_GAF_FILE);
            m_pShipsActor[i]->setPosAtNode(Vec2(-113, 337));
            m_pShipsActor[i]->setSize(Size(300, 300));
        }else if (i == 1)
        {
            m_pShipsActor[i] = Actor::createWithGafFile(SHIPTHREE_GAF_FILE);
            m_pShipsActor[i]->setPosAtNode(Vec2(-154, 300));
            m_pShipsActor[i]->setSize(Size(250, 250));
        }else
        {
            m_pShipsActor[i] = Actor::createWithGafFile(SHIPTWO_GAF_FILE);
            m_pShipsActor[i]->setPosAtNode(Vec2(-87, 531));
            m_pShipsActor[i]->setSize(Size(400, 400));
            m_pShipsActor[i]->setPosition(m_shipsPos[i]);
        }
        m_pShipsActor[i]->setUserData((void*)i);
        m_pShipsActor[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        this->addChild(m_pShipsActor[i]);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pShipsActor[all]->setPosition(m_shipsPos[randIndex]);
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round026_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round026_02::changeScaleShipsSchedule), 0.6f, -1, 0);   //
}

//依次放大船以提示
void Round026_02::changeScaleShipsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round026_02::changeScaleShipsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pShipsActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round026_02::openTouch()
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (2 == index) {   //答对
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pShipsActor[i]);
                } /*for*/
            } else {
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
        }else
        {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pShipsActor[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pShipsActor[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round026_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round026_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

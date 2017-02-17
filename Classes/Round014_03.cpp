//
//  Round014_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/12.
//
//

#include "Round014_03.h"
#include "ui/CocosGUI.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

USING_NS_CC;

#define BEGFORE_TAG                     200
#define ROAD_TAG                        201
#define WALL_TAG                        202
#define SHIP_ZORDER                     2
#define GLASS_ZORDER                    1
#define REDLIGHT_GAF_CONFIG_FILE        "config/stage014/light.json"
#define UP_LIGHT_ACTOR_POS              Vec2(300.0f, 300.0f)
#define DOWN_LIGHT_ACTOR_POS            Vec2(300.0f, 100.0f)
#define STANDBY                        "standby"                                // 待机动画
#define TRIGGER                        "trigger"                                // 触发动画
#define DOWN_LIGHT_ACTOR_TAG            205
#define CELEBRATE_DELAY                 5.0f

/*
 *  constructor
 */
Round014_03::Round014_03()
{
    m_airShipsPos[0] = Vec2(924, 767);
    m_airShipsPos[1] = Vec2(1200, 1131);
    m_airShipsPos[2] = Vec2(1489, 767);
    m_airShipsPos[3] = Vec2(1664, 1159);
}

/*
 *  destructor
 */
Round014_03::~Round014_03()
{
    m_pStageScene->removeOBLeaderNode(BEGFORE_TAG);
    m_pStageScene->removeOBLeaderNode(WALL_TAG);
    m_pStageScene->removeOBLeaderNode(ROAD_TAG);
    m_pStageScene->removeOBLeaderNode(DOWN_LIGHT_ACTOR_TAG);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

Round014_03* Round014_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round014_03();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene,pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

bool Round014_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    srand((unsigned)time(0));
    m_curSameBallRandNum = rand()%3;
    
    char buffer[64];
    unsigned int number = 0;
    for (unsigned long i = 0; i < 4; i++)
    {
        if (m_curSameBallRandNum == i || m_curSameBallRandNum == i - 1) {
            sprintf(buffer, "XQ-GK02-3-DA%02d.png",m_curSameBallRandNum+1);
            m_pAirShips[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
            if (m_curSameBallRandNum == i-1) {
                number++;
            }
        } else {
            sprintf(buffer, "XQ-GK02-3-DA%02d.png",number+1);
            m_pAirShips[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
            number++;
        }
        m_pAirShips[i]->setUserData((void*)i);
        this->addChild(m_pAirShips[i], SHIP_ZORDER);
        m_pAirShips[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4)
    {
        unsigned int shipIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[shipIndex]) continue;
        selected[shipIndex] = true;
        m_pAirShips[all]->setPosition(m_airShipsPos[shipIndex]);
        all++;
    } /*while*/
    
    m_pRedLightsActorUp = Actor::createWithGafConfigFile(REDLIGHT_GAF_CONFIG_FILE);
    m_pRedLightsActorUp->setAutoReleaseAsset(true);
    m_pRedLightsActorUp->setPosition(Vec2(451, 594));
    this->addChild(m_pRedLightsActorUp);
    m_pRedLightsActorUp->playWithSequenceAliasName(STANDBY,true);
    
    m_pRedLightsActorDown = Actor::createWithGafConfigFile(REDLIGHT_GAF_CONFIG_FILE);
    m_pStageScene->addOverLeaderNode(m_pRedLightsActorDown, Vec2(7319, 271), DOWN_LIGHT_ACTOR_TAG);
    m_pRedLightsActorDown->playWithSequenceAliasName(STANDBY,true);
    
    m_pGlassBoard = Sprite::createWithSpriteFrameName("XQ-GK02-3-IT02.png");
    m_pGlassBoard->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pGlassBoard->setPosition(Vec2(-826, 140));
    this->addChild(m_pGlassBoard);
    
    return true;
}

void Round014_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round014_03::scaleChangeSchedule), 1.1f, -1, 0);
}

// 依次改变飞船大小
void Round014_03::scaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 4)
    {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_03::scaleChangeSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAirShips[m_InitAniIndex]->runAction(Sequence::create(CallFunc::create([this]{
        char buffer[64];
        if (m_curSameBallRandNum == m_InitAniIndex || m_curSameBallRandNum == m_InitAniIndex - 1) {
            sprintf(buffer, "XQ-GK02-3-DA%02d-2.png",m_curSameBallRandNum+1);
            m_pAirShips[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        } else {
            sprintf(buffer, "XQ-GK02-3-DA%02d-2.png",m_number+1);
            m_pAirShips[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        }
    }),DelayTime::create(0.4f),CallFunc::create([this]{
        char buffer[64];
        if (m_curSameBallRandNum == m_InitAniIndex || m_curSameBallRandNum == m_InitAniIndex - 1) {
            sprintf(buffer, "XQ-GK02-3-DA%02d.png",m_curSameBallRandNum+1);
            m_pAirShips[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
            if (m_curSameBallRandNum == m_InitAniIndex-1) {
                m_number++;
            }
        } else {
            sprintf(buffer, "XQ-GK02-3-DA%02d.png",m_number+1);
            m_pAirShips[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
            m_number++;
        }
        m_InitAniIndex++;
    }), NULL));
}

/* 开启触摸 */
void Round014_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        if (rect.containsPoint(touchPoint))
        {
            char buffer[64];
            if (index == m_curSameBallRandNum || index == m_curSameBallRandNum+1) {
                sprintf(buffer, "XQ-GK02-3-DA%02d-2.png",m_curSameBallRandNum+1);
                target->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
            } else {
                if (index > m_curSameBallRandNum) {
                    sprintf(buffer, "XQ-GK02-3-DA%02d-2.png",(int)index);
                } else {
                    sprintf(buffer, "XQ-GK02-3-DA%02d-2.png",(int)index + 1);
                }
                target->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
            }
            
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);

        long index = (long)target->getUserData();
        
        char buffer[64];
        if (index != m_curSameBallRandNum && index != m_curSameBallRandNum+1) {
            if (index > m_curSameBallRandNum) {
                sprintf(buffer, "XQ-GK02-3-DA%02d.png",(int)index);
            } else {
                sprintf(buffer, "XQ-GK02-3-DA%02d.png",(int)index+1);
            }
            target->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        }
        if (rect.containsPoint(touchPoint)) {
            if (index == m_curSameBallRandNum || index == m_curSameBallRandNum+1)
            {
                m_curCorrectNumber++;
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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
        
        if (m_curCorrectNumber == 2) {
            for (unsigned int i = 0; i < 4; i++) {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAirShips[i]);
            } /*for*/
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            celebrate(CELEBRATE_DELAY);
            changeActor();                                              //设置红灯闪动
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAirShips[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAirShips[i]);
    } /*for*/

}

void Round014_03::moveGlassBoard()
{
    m_pGlassBoard->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(1300, 0.0f)),CallFunc::create([this]{
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
}

void Round014_03::changeActor()
{
    m_pRedLightsActorUp->playWithSequenceAliasName(TRIGGER,true);
    m_pRedLightsActorDown->playWithSequenceAliasName(TRIGGER,true);
}

void Round014_03::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

void Round014_03::endRound()
{
    moveGlassBoard();
}

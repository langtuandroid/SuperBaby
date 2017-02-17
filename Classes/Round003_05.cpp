//
//  Round003_05.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round003_05.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define SL_MAP_BGPNG_FILE           "res/stage003/SL-MAP-BG.png"

#define SONGSHU_CONFIG_FILE         "res/stage003/SL-GK03-3-MS01-D+C.json"

#define CLOTHER1_FRAMENAME          "SL-GK03-3-DA01.png"
#define CLOTHER2_FRAMENAME          "SL-GK03-3-DA02.png"
#define CLOTHER3_FRAMENAME          "SL-GK03-3-DA03.png"

/*
 *  constructor
 */
Round003_05::Round003_05() {}

/*
 *  destructor
 */
Round003_05::~Round003_05()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round003_05* Round003_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round003_05();
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
bool Round003_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(SL_MAP_BGPNG_FILE);
    appendAsyncLoadGaf("res/stage003/SL-GK03-3-MS01-D+C/SL-GK03-3-MS01-D+C.gaf");
    preLoadRes();
    return true;
}

void Round003_05::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    auto sbn = SpriteBatchNode::create(SL_MAP_BGPNG_FILE);
    addChild(sbn, 2);
    
    float posx = 0.0f, posy = 0.0f, offsetx = 0.0f;
    char szBuffer[32];
    posx = 620.0f;posy = 850.0f;offsetx = 320.0f;
    for (unsigned long i = 0; i < 3; i++)
    {
        sprintf(szBuffer, "SL-GK03-3-DA%02ld.png", i + 1);
        m_pClotheSps[i] = Sprite::createWithSpriteFrameName(szBuffer);
        sbn->addChild(m_pClotheSps[i], 1);
        if (2 == i)
        m_pClotheSps[i]->setPosition(Vec2(posx + 50.0f, posy));
        else
        m_pClotheSps[i]->setPosition(Vec2(posx, posy));
        m_pClotheSps[i]->setUserData((void*)i);
        posx += offsetx;
    } /*for*/
    
    // 松鼠
    m_pActor = Actor::createWithGafConfigFile(SONGSHU_CONFIG_FILE);
    m_pActor->playWithSequenceAliasName("standby", true);
    m_pActor->setPosition(Vec2(1300.0f, 960.0f));
    addChild(m_pActor, 1);
}

void Round003_05::openTouch()
{
    //
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
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
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            unsigned long selIndex = (unsigned long)target->getUserData();
            if (2 == selIndex) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                
                celebrate(3.0f);
                auto eventDispatch = Director::getInstance()->getEventDispatcher();
                eventDispatch->removeEventListenersForTarget(m_pClotheSps[0]);
                eventDispatch->removeEventListenersForTarget(m_pClotheSps[1]);
                eventDispatch->removeEventListenersForTarget(m_pClotheSps[2]);
            } else {
                // 播放答错音效
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
    auto eventDispatch = Director::getInstance()->getEventDispatcher();
    eventDispatch->addEventListenerWithSceneGraphPriority(listener, m_pClotheSps[0]);
    eventDispatch->addEventListenerWithSceneGraphPriority(listener->clone(), m_pClotheSps[1]);
    eventDispatch->addEventListenerWithSceneGraphPriority(listener->clone(), m_pClotheSps[2]);
}

//
void Round003_05::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round003_05::changeScaleSchedule), 0.6f, -1, 0);
}

//
void Round003_05::changeScaleSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round003_05::changeScaleSchedule));
        return;
    }
    m_pClotheSps[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//
void Round003_05::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/*  回合结束回调 */
void Round003_05::endRound()
{
    m_pActor->playWithSequenceAliasName("trigger", false);
    if (m_completeCB) {
        m_completeCB();
    }
}

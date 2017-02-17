//
//  Round009_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round009_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "SocialShareNode.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_GAF_BG_FILE           "res/common/guanghuanxiaoguo/guanghuanxiaoguo.gaf"
#define CELEBRATE_GAF_FILE              "res/common/wujiaoxing01/wujiaoxing01.gaf"
#define CELEBRATE_DELAY                 5.0f                                    // 庆祝延时
#define MOVE_ACTOR_TIME                 2.0f                                    // 分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                                    // 返回关卡选择场景延时时间

#define CROW_PNG_FILE                   "res/stage009/SL-GK09-4-MS01-D+C/SL-GK09-4-MS01-D+C.png"
#define CROW_GAF_CONFIG_FILE            "res/stage009/SL-GK09-4-MS01-D+C.json"

#define GK_09_PNG                       "res/stage009/SL-GK09-IT.png"
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round009_04::Round009_04():
m_correctNumber(0),
m_pCrowGaf(nullptr),
m_pBSN(nullptr)
{}

/*
 *  destructor
 */
Round009_04::~Round009_04() {
    cleanUp();
}

/*
 *  清理
 */
void Round009_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round009_04* Round009_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round009_04();
    if (pRound && pRound->initWithRoundIndex(roundIndex,pStageScene,pos))
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
bool Round009_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(CROW_PNG_FILE);
    appendAsyncLoadGaf("res/stage009/SL-GK09-4-MS01-D+C/SL-GK09-4-MS01-D+C.gaf");
    preLoadRes();
    
    m_pBSN = SpriteBatchNode::create(GK_09_PNG);
    addChild(m_pBSN, 2);
    char szBuffer[32];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK09-4-DA%02ld.png", i + 1);
        m_pSugarSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pSugarSp[i]->setUserData((void*)i);
        m_pBSN->addChild(m_pSugarSp[i], 1);
    } /*for*/
    
    float posx = 760.0f, posy = 300.0f, offsetx = 240.0f;
    bool placed[3] = {false, false, false};
    unsigned int allPlaced = 0;
    srand(unsigned(time(0)));
    while (allPlaced < 3)
    {
        auto randPlacePos = static_cast<unsigned int>(rand() % 3);
        if (placed[randPlacePos]) continue;
        placed[randPlacePos] = true;
        m_pSugarSp[randPlacePos]->setPosition(Vec2(posx, posy));
        posx += offsetx;
        allPlaced++;
    } /*while*/
    
    return true;
}

// 开始回合
void Round009_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round009_04::changeScaleSchedule), 0.6f, -1, 0);
}

//依次放大
void Round009_04::changeScaleSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round009_04::changeScaleSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pSugarSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//
void Round009_04::onPreLoadedRes()
{
    /////////
    RoundBase::onPreLoadedRes();
    
    // 乌鸦 npc
    m_pCrowGaf = Actor::createWithGafConfigFile(CROW_GAF_CONFIG_FILE);
    m_pCrowGaf->setPosition(Vec2(1320.0f, 950.0f));
    m_pCrowGaf->playWithSequenceAliasName("standby", true);
    addChild(m_pCrowGaf, 1);
}

// 庆祝
void Round009_04::celebrate(const float celebrateDelay) {
    
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
    
}

//回合结束分享调用
void Round009_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    moveActor();                        //移动主角
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round009_04::fadeOutLayer), BACK_SCENE_TIME-1.5f);
}

//
void Round009_04::addListener()
{
    auto listenerTouch = EventListenerTouchOneByOne::create();
    listenerTouch->setSwallowTouches(true);
    listenerTouch->onTouchBegan = [this](Touch* touch, Event* event) -> bool
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
    listenerTouch->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(target->convertTouchToNodeSpace(touch)))
        {
            unsigned long touchIndex = (unsigned long)target->getUserData();
            if (touchIndex == m_correctNumber) /*答对*/ {
                auto eventDisptacher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    eventDisptacher->removeEventListenersForTarget(m_pSugarSp[i]);
                } /*for*/
                m_pCrowGaf->playWithSequenceAliasName("trigger");
                m_pCrowGaf->setAnimationFinishedPlayDelegate([this]
                {
                    m_pCrowGaf->playWithSequenceAliasName("standby", true);
                });
                for (unsigned int i = 0; i < 3; i++) {
                    m_pSugarSp[i]->runAction(FadeOut::create(CELEBRATE_DELAY));
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                this->celebrate(CELEBRATE_DELAY);
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
    auto eventDisptacher = Director::getInstance()->getEventDispatcher();
    eventDisptacher->addEventListenerWithSceneGraphPriority(listenerTouch, m_pSugarSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        eventDisptacher->addEventListenerWithSceneGraphPriority(listenerTouch->clone(), m_pSugarSp[i]);
    } /*for*/
}

// 移动主角退出回合
void Round009_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000.0f, 0.0f));
}

/* 开启触摸 */
void Round009_04::openTouch()
{
    addListener();
}

/*  回合结束回调 */
void Round009_04::endRound() {}

//渐变变黑退出
void Round009_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor,COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);   //停止分享音效
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round009_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round009_04::roundEndShareCallBack, this, false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round009_04::roundEndShareCallBack, this, true));
}

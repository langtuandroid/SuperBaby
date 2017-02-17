//
//  Round021_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/21.
//
//

#include "Round021_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"

#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  5
#define MESSAGEBOX_ZORDER               4
#define MOVE_ACTOR_TIME                 3.0f                              //分享界面ok过后主角移动时间
#define COLORLAYER_TAG                  300
#define DOORFRAME_ZORDER                2
#define DOOR_ZORDER                     3
#define UPWALL_ZORDER                   1
#define RIGHTWALL_TAG                   201
#define UPWALL_POS                      Vec2(1812, 1605)
#define RIGHTWALL_POS                   Vec2(13011, 1031)
#define DOOR_PNG_FILE                   "XQ-GK09-4-IT02.png"
#define DOORFRAME_PNG_FILE              "res/stage021/XQ-GK09-04-IT01.png"
#define UPWALL_PNG_FILE                 "res/stage021/XQ-GK09-04-IT02.png"
#define RIGHTWALL_PNG_FILE              "res/stage021/XQ-GK09-04-IT03.png"
#define MESSAGEBOX_PNG_FILE             "XQ-GK09-4-TM01.png"
#define MESSAGEBOX_POS                  Vec2(1773,839)
#define DOORFRAME_POS                   Vec2(1652,822)
#define DOOR_POS                        Vec2(1782,852)

/*
 *  constructor
 */
Round021_04::Round021_04()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(427, 372);
    m_answersPos[1] = Vec2(427, 188);
}

/*
 *  destructor
 */
Round021_04::~Round021_04()
{
    cleanUp();
}

//
void Round021_04::cleanUp()
{
    m_pStageScene->removeOBLeaderNode(RIGHTWALL_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOORFRAME_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(UPWALL_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(RIGHTWALL_PNG_FILE);
}

//
Round021_04* Round021_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round021_04();
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
bool Round021_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pDoorFrameSp = Sprite::create(DOORFRAME_PNG_FILE);
    m_pDoorFrameSp->setPosition(DOORFRAME_POS);
    this->addChild(m_pDoorFrameSp, DOORFRAME_ZORDER);
    
    m_pDoorSp = Sprite::createWithSpriteFrameName(DOOR_PNG_FILE);
    m_pDoorSp->setPosition(DOOR_POS);
    this->addChild(m_pDoorSp, DOOR_ZORDER);
    
    m_pUpWallSp = Sprite::create(UPWALL_PNG_FILE);
    m_pUpWallSp->setPosition(UPWALL_POS);
    this->addChild(m_pUpWallSp, UPWALL_ZORDER);
    
    m_pRightWallSp = Sprite::create(RIGHTWALL_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pRightWallSp, RIGHTWALL_POS, RIGHTWALL_TAG);
    
    m_pMessageBoxSp = Sprite::createWithSpriteFrameName(MESSAGEBOX_PNG_FILE);
    m_pMessageBoxSp->setPosition(MESSAGEBOX_POS);
    this->addChild(m_pMessageBoxSp, MESSAGEBOX_ZORDER);
    m_pMessageBoxSp->setAnchorPoint(Vec2(1.0f, 0.5f));
    m_pMessageBoxSp->setScale(0.0f);
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "XQ-GK09-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setPosition(m_answersPos[i]);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pMessageBoxSp->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    return true;
}

// 进入本回合
void Round021_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    
    changeScaleMessageBox();
}

//放大对话框
void Round021_04::changeScaleMessageBox()
{
    m_pMessageBoxSp->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)),CallFunc::create([this]{
        this->schedule(CC_SCHEDULE_SELECTOR(Round021_04::scaleChangeAnswersSchedule), 0.6f, -1, 0);
    }), NULL));
}

//改变尺寸以提示
void Round021_04::scaleChangeAnswersSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round021_04::scaleChangeAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round021_04::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        for (unsigned int i = 0; i < 2; i++) {
            Rect rect = Rect(0, 0, m_pAnswersSp[i]->getContentSize().width, m_pAnswersSp[i]->getContentSize().height);
            if (rect.containsPoint(m_pAnswersSp[i]->convertTouchToNodeSpace(touch))) {
                m_pAnswersSp[i]->setScale(1.1f);
                return true;
            }
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        for (unsigned int i = 0; i < 2; i++) {
            Rect rect = Rect(0, 0, m_pAnswersSp[i]->getContentSize().width, m_pAnswersSp[i]->getContentSize().height);
            if (rect.containsPoint(m_pAnswersSp[i]->convertTouchToNodeSpace(touch))) {
                m_pAnswersSp[i]->setScale(1.0f);
                if (i == 0) {
                    scheduleOnce(CC_SCHEDULE_SELECTOR(Round021_04::fadeMessageBox), 0.5f);
                    // 播放答对特效
                    m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                    // 播放答对音效
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
                    for (unsigned int i = 0; i < 2; i++) {
                        Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                    } /*for*/
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    celebrate(CELEBRATE_DELAY);

                }else
                {
                    // 播放答错音效
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
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[1]);
}


//打开门
void Round021_04::openDoor()
{
    m_pDoorSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_pDoorSp->getContentSize().width + 16.0f, 0.0f)), Hide::create(), CallFunc::create([this]{
        moveActor();
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round021_04::fadeOutLayer), 1.0f);
    }), NULL));
}

//答对后消失对话框
void Round021_04::fadeMessageBox(float)
{
    m_pMessageBoxSp->runAction(Sequence::create(Spawn::create(ScaleTo::create(0.5f, 0.0f),FadeOut::create(0.5f), NULL), NULL));
}

/*
 *  庆祝
 */
void Round021_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round021_04::endRound()
{
    
}

/*
 *  分享界面点击ok后移动主角
 */

void Round021_04::moveActor()
{
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0), true);
}

//渐变变黑退出
void Round021_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//回合结束分享调用
void Round021_04::roundEndShareCallBack(bool isShare)
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
    openDoor();
}

//分享回调
void Round021_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round021_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round021_04::roundEndShareCallBack, this,true));
}
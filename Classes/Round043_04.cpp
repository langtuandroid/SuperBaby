//
//  Round043_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#include "Round043_04.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"
#include "RoundUINode.h"
#include "MyAudioEngine.h"
#include "SocialShareNode.h"
#include "ThemeSelScene.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "StageData.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define SCREW_PNG_FILE                  "res/stage043/GC-GC07-4-IT05.png"
#define SCREW_POS                       Vec2(1449,803)
#define SCREW_ZORDER                    2
#define DOOR_BIG_PNG_FILE               "res/stage043/GC-GC07-4-IT03.png"
#define DOOR_BIG_POS                    Vec2(1297,381)
#define DOOR_ZORDER                     3
#define DOOR_SMALL_PNG_FILE             "res/stage043/GC-GC07-4-IT01.png"
#define DOOR_SMALL_POS                  Vec2(977,415)
#define SHAOW_PNG_FILE                  "res/stage043/GC-GC07-4-IT00.png"
#define SHAOW_POS                       Vec2(1154,387)
#define SHAOW_ZORDER                    1
#define OVER_PNG_FILE                   "res/stage043/GC-GC07-4-IT04.png"
#define OVER_POS                        Vec2(12379,803)
#define OVER_TAG                        203
#define HINT_BIG_PNG_FILE               "res/stage043/GC-GC07-4-DA02.png"
#define HINT_BIG_POS                    Vec2(1297,381)
#define HINT_SMALL_PNG_FILE             "res/stage043/GC-GC07-4-DA01.png"
#define HINT_SMALL_POS                  Vec2(977,415)
#define HINT_ZORDER                     4
#define MS_GAF_CONFIG_FILE              "res/stage039/ms.json"
#define MS_GAF_FILE                     "res/stage039/GC-GK03-4-MS/GC-GK03-4-MS.gaf"
#define MS_POS                          Vec2(643,256)
#define MS_ZORDER                       4
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define COLORLAYER_TAG                  300


/*
 *  constructor
 */
Round043_04::Round043_04(){}

/*
 *  destructor
 */
Round043_04::~Round043_04(){
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    Director::getInstance()->getTextureCache()->removeTextureForKey(SCREW_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOOR_BIG_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOOR_SMALL_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(SHAOW_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

/*
 *  初始化
 */
Round043_04* Round043_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round043_04();
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
bool Round043_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pScrewSp = Sprite::create(SCREW_PNG_FILE);
    m_pScrewSp->setPosition(SCREW_POS);
    this->addChild(m_pScrewSp, SCREW_ZORDER);
    
    m_pDoorSp[0] = Sprite::create(DOOR_BIG_PNG_FILE);
    m_pDoorSp[0]->setPosition(DOOR_BIG_POS);
    this->addChild(m_pDoorSp[0], DOOR_ZORDER);
    
    m_pDoorSp[1] = Sprite::create(DOOR_SMALL_PNG_FILE);
    m_pDoorSp[1]->setPosition(DOOR_SMALL_POS);
    this->addChild(m_pDoorSp[1], DOOR_ZORDER);
    
    m_pShadowSp = Sprite::create(SHAOW_PNG_FILE);
    m_pShadowSp->setPosition(SHAOW_POS);
    this->addChild(m_pShadowSp, SHAOW_ZORDER);
    
    m_pOverSp = Sprite::create(OVER_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    m_pHintSp[0] = Sprite::create(HINT_BIG_PNG_FILE);
    m_pHintSp[0]->setPosition(HINT_BIG_POS);
    m_pHintSp[0]->setUserData((void*)0);
    m_pHintSp[0]->setOpacity(0);
    this->addChild(m_pHintSp[0], HINT_ZORDER);
    
    m_pHintSp[1] = Sprite::create(HINT_SMALL_PNG_FILE);
    m_pHintSp[1]->setPosition(HINT_SMALL_POS);
    m_pHintSp[1]->setUserData((void*)1);
    m_pHintSp[1]->setOpacity(0);
    this->addChild(m_pHintSp[1], HINT_ZORDER);
    
    m_pMSActor =Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);

    return true;
}

//
void Round043_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round043_04::showHintsSchedule), 1.2f, -1, 0);
    
}

//改变答案的大小
void Round043_04::showHintsSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round043_04::showHintsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pHintSp[m_InitAniIndex++]->runAction(Sequence::create(FadeIn::create(0.3f),
                                                            FadeOut::create(0.3f),
                                                            FadeIn::create(0.3f),
                                                            FadeOut::create(0.3f), NULL));
}

/* 开启触摸 */
void Round043_04::openTouch()
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
            if (index == 0) {   //答对
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
                changeActor();
                for (unsigned int i = 0; i < 2; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pHintSp[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pHintSp[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pHintSp[1]);
}

void Round043_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        openDoor();
    });
}

/*
 *  庆祝
 */
void Round043_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round043_04::endRound() {}

//回合结束分享调用
void Round043_04::roundEndShareCallBack(bool isShare)
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
    moveLeadActor();
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round043_04::fadeOutLayer), 3.0f);
}

void Round043_04::openDoor()
{
    m_pDoorSp[0]->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_pDoorSp[0]->getContentSize().width + 16.0f, 0.0f)), Hide::create(), NULL));
}

void Round043_04::moveLeadActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, -50.0f),true);
}

//渐变变黑退出
void Round043_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            }else
            {
                // 过关
                m_pStageScene->stageClear();
                //试玩时停止播放背景音乐
                if (m_pStageScene->isTrial())
                {
                    MyAudioEngine::getInstance()->stopAudio(0,true);
                }
            }
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round043_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round043_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round043_04::roundEndShareCallBack, this,true));
}
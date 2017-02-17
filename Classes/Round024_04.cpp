//
//  Round024_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/3/27.
//
//

#include "Round024_04.h"
#include "StageScene.h"
#include "ThemeSelScene.h"
#include "StageDataManager.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SelStageScene.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "AppConfig.h"
#include "GafAssetCache.h"

// 大图
#define STAGE_IMAGE_FILE        "res/stage024/XQ-GK12-IT.png"
#define STAGE_PLIST_FILE        "res/stage013/XQ-GK12-IT.plist"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define MOVE_ACTOR_TIME                 5.0f                                // 分享界面ok过后主角移动时间
#define MOV_TIME                        0.2f                                // 答错回原位置移动时间

#define SCALE_FACTOR                    1.2
#define SCALE_DURATION                  0.3f

//
#define NPC_COLLECT_DURATION                1.5f

// 火箭
#define ROCKET_GAF_FILE                 "res/stage024/XQ-GK12-4-TM-D+C1+C2/XQ-GK12-4-TM-D+C1+C2.gaf"
#define ROCKET_GAF_CONFIG               "res/stage024/XQ-GK12-4-TM-D+C1+C2.json"

//BS
#define BS_GAF_CONFIG_FILE              "res/stage024/XQ-GK12-BS.json"
#define BS_GAF_FILE                     "res/stage024/XQ-GK12-BS/XQ-GK12-BS.gaf"
#define BS_GAF_POS                      Vec2(800,600)
#define BS_GAF_ZORDER                   4
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGERONE                      "triggerone"                                // 触发动画
#define TRIGGERTWO                      "triggertwo"                                //
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round024_04::Round024_04()
{
    m_targerRect[0] = Rect(994.0f,  816.0f, 140.0f, 186.0f);
    m_targerRect[1] = Rect(1172.0f, 816.0f, 140.0f, 186.0f);
    m_targerRect[2] = Rect(994.0f, 623.0f, 140.0f, 186.0f);
    m_targerRect[3] = Rect(1172.0f, 623.0f, 140.0f, 186.0f);
}

/*
 *  destructor
 */
Round024_04::~Round024_04()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    dispatcher->removeCustomEventListeners("openTouch");
}

//
Round024_04* Round024_04::createWithRoundIndex(const int roundIndex,
                                               StageScene* pStageScene, const Vec2& nodePos, const Vec2& pos)
{
    auto pRound = new Round024_04();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene, nodePos,pos))
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
bool Round024_04::initWithRoundIndex(const int roundIndex,
                                     StageScene* pStageScene, const Vec2& nodePos, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    this->setPosition(nodePos);
    
    // 大图
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 2);
    
    appendAsyncLoadGaf(ROCKET_GAF_FILE);
    appendAsyncLoadGaf(BS_GAF_FILE);
    preLoadRes();
    
//    for (int i = 0; i < 4; i++)
//    {
//        DrawNode* drawNode = DrawNode::create();
//        this->addChild(drawNode, 10);
//        auto rect = m_targerRect[i];
//        Vec2 point[4];
//        point[0] = Vec2(rect.origin.x, rect.origin.y);
//        point[1] = Vec2(rect.origin.x, rect.origin.y + rect.size.height);
//        point[2] = Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
//        point[3] = Vec2(rect.origin.x + rect.size.width, rect.origin.y);
//        drawNode->drawPolygon(point, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 1, 0, 1));
//    }
    
    return true;
}

void Round024_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    const float nodeOffsetX = this->getPosition().x;
    
    m_pRocketActor = Actor::createWithGafConfigFile(ROCKET_GAF_CONFIG);
    m_pRocketActor->setAutoReleaseAsset(true);
    
    m_pRocketActor->playWithSequenceAliasName("standby", true);
    m_pStageScene->addOverLeaderNode(m_pRocketActor, Vec2(nodeOffsetX + 1174.0f, 746.0f), 4001);
    
    for (long i = 0; i < 5; i++)
    {
        if (4 == i)
            m_pBatterySp[i] = Sprite::createWithSpriteFrameName("XQ-GK12-4-DA02.png");
        else
            m_pBatterySp[i] = Sprite::createWithSpriteFrameName("XQ-GK12-4-DA01.png");
        m_pBatterySp[i]->setUserData((void*)i);
        m_pStageScene->addOverLeaderNode(m_pBatterySp[i], Vec2::ZERO, 4002 + (int)i, 1);
    } /*for*/
    
    // 随机位置
    float posx = 640.0f, posy = 260.0f, offsetx = 200.0f;
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5)
    {
        unsigned int rdx = static_cast<unsigned int>(rand() % 5);
        if (selected[rdx]) continue;
        selected[rdx] = true;
        m_pBatterySp[all]->setPosition(Vec2(nodeOffsetX + posx + rdx * offsetx, posy));
        m_originalPoints[all] = m_pBatterySp[all]->getPosition();
        m_pjBatterySp[rdx] = m_pBatterySp[all];
        all++;
    } /*while*/
}

/*
 *  进入回合
 */
void Round024_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
        dispatcher->removeCustomEventListeners("openTouch");
        m_opentouched = true;
        if (m_scaled)
        {
            openTouch();
        }
    });
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this, dispatcher] (EventCustom *)
    {
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    
    auto cb = CallFunc::create([this]{scaleCompleteCB();});
    auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f),
                                     ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f), cb, NULL);
    m_pjBatterySp[m_nScaleIndex]->runAction(scaleSeq);
}

//
void Round024_04::scaleCompleteCB()
{
    if (++m_nScaleIndex == 5)
    {
        m_scaled = true;
        if (m_opentouched)
        {
            openTouch();
        }
    } else {
        auto cb = CallFunc::create([this]{scaleCompleteCB();});
        auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                         ScaleTo::create(SCALE_DURATION, 1.0f),
                                         ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                         ScaleTo::create(SCALE_DURATION, 1.0f), cb, NULL);
        m_pjBatterySp[m_nScaleIndex]->runAction(scaleSeq);
    }
}

/*
 *  开启触摸
 */
void Round024_04::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect touchRect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (touchRect.containsPoint(touchPoint))
        {
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            auto parentNode = target->getParent();
            auto zorder = target->getLocalZOrder();
            parentNode->reorderChild(target, zorder + 1);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos = Vec2::ZERO;
        
        long index = (long)target->getUserData();
        m_movePos.x = m_originalPoints[index].x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoints[index].y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        
        auto parentNode = target->getParent();
        auto zorder = target->getLocalZOrder();
        parentNode->reorderChild(target, zorder - 1);
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        
        long battleIndex = (long)target->getUserData();
        bool isPlaced = false;
        
        for (int i = 0; i < 4; i++)
        {
            if (m_targerRect[i].containsPoint(touchPoint))
            {
                if (m_placed[i] || !isPowerFullBattery(battleIndex))
                {
                    break;
                }
                auto nodeOffsetX = this->getPosition().x;
                float tx = m_targerRect[i].origin.x + m_targerRect[i].size.width / 2.0f + nodeOffsetX;
                float ty = m_targerRect[i].origin.y + m_targerRect[i].size.height / 2.0f;
                target->setPosition(Vec2(tx, ty));
                m_placed[i] = true;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
                isPlaced = true;
            }
        } /*for*/
        
        if (isPlaced)
        {
            m_numberOfPlaced++;
            m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x, touch->getLocation().y));    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            if (4 == m_numberOfPlaced) {
                answerCorrectCB();
            } else {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) { // 沮丧
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[battleIndex])),CallFunc::create([this,target,battleIndex]
            {
                target->setPosition(m_originalPoints[battleIndex]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pBatterySp[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pBatterySp[i]);
    } /*for*/
}

//答对后隐藏剩余电池
void Round024_04::fadeBattery(float)
{
    m_pBatterySp[4]->runAction(FadeOut::create(0.5f));
}

//收集主题BS
void Round024_04::addBSActor()
{
    m_pBSActor = Actor::createWithGafConfigFile(BS_GAF_CONFIG_FILE);
    m_pStageScene->addOverLeaderNode(m_pBSActor, Vec2(11148.0f, 1066.0f), 4002, 2);
    m_pBSActor->playWithSequenceAliasName(TRIGGERONE);
    m_pBSActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pBSActor->playWithSequenceAliasName(STANDBY,true);
        scheduleOnce([this](float){
            m_pBSActor->playWithSequenceAliasName(TRIGGERTWO);
            m_pBSActor->setAnimationFinishedPlayDelegate([this]
            {
                m_pStageScene->manualScroll(556.0f, std::bind(&Round024_04::getOnRocket, this));
            });
        }, 1.0f, "delaytime");
    });
}

/*
 *  全部答对回调
 */
void Round024_04::answerCorrectCB()
{
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    for (unsigned int i = 0; i < 5; i++) {
        eventDispatcher->removeEventListenersForTarget(m_pBatterySp[i]);
    } /*for*/
    m_pStageScene->getRoundUINode()->stopReadEffect();          // 停止题目音效
    m_pStageScene->getRoundUINode()->endReadTopic();            // 停止读题目改变动
    
    celebrate(CELEBRATE_DELAY); //庆祝
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round024_04::fadeBattery), 1.0f);
}

/*
 *  庆祝
 */
void Round024_04::celebrate(const float celebrateDelay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round024_04::endRound() {}

//回合结束调用
void Round024_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
        
    } else {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    // 隐藏电池
    for (int i = 0; i < 5; i++)
    {
        if (i != 4) {
            m_pBatterySp[i]->setPosition(Vec2( 0.9f, -10000.0f));
        }
    } /*for*/
    m_pRocketActor->playWithSequenceAliasName(TRIGGERONE);
    m_pRocketActor->setAnimationFinishedPlayDelegate([this]
    {
        addBSActor();
    });
}

//
void Round024_04::moveActorComplete(float dt)
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);    // 停止分享音效
}

/* 登上火箭 */
void Round024_04::getOnRocket()
{
    auto cb = [this]
    {
        // 隐藏主角
        m_pStageScene->hideLeaderActor();
        m_pRocketActor->playWithSequenceAliasName(TRIGGERTWO, false);
        m_pRocketActor->setAnimationFinishedPlayDelegate(std::bind(&Round024_04::rocketUped, this));
    };
    auto speed = AppConfig::getInstance()->getBgScrollSpeed();
    speed = 256.0f;
    m_pStageScene->moveLeadingActorCB(1160.0f / speed, Vec2(1160.0f, 0.0f), cb);
}

/* 
 *  火箭上升后
 */
void Round024_04::rocketUped()
{
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round024_04::fadeOutLayer), 0.5f);
}

/*
 *  渐变变黑退出
 */
void Round024_04::fadeOutLayer(float dt)
{
//    m_pStageScene->removeOBLeaderNode(OVER_TAG);
    
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width + 620.0f, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    schedule([this](float dt) {
        if (m_opacity == 155) {
            unschedule("add");
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            } else {
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
    }, 0.005f, "add");
}

//分享回调
void Round024_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round024_04::roundEndShareCallBack, this, true));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round024_04::roundEndShareCallBack, this, false));
}

/* 
 *  是否满格的电源
 */
bool Round024_04::isPowerFullBattery(const long batteryIndex)
{
    return batteryIndex != 4;
}

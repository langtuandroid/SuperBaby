//
//  Round018_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round018_04.h"
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

#define STANDBY                         "standby"                                // 待机动画
#define TRIGGER                         "trigger"                             // 触发动画
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define MS_GAF_CONFIG_FILE              "config/stage018/crane.json"
#define MS_GAF_FILE                     "res/stage018/XQ-GK06-4-MS-D+C/XQ-GK06-4-MS-D+C.gaf"
#define MS_GAF_POS                      Vec2(750,1594)
#define MS_GAF_ZORDER                   2
#define MOVE_ACTOR_TIME                 4.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define BRIDGE_PNG_FILE                 "res/stage018/XQ-GK06-4-TM02.png"
#define BRIDGE_POS                      Vec2(1099,1542)
#define BRIDGE_ZORDER                   1
#define MOV_TIME                        0.2f                                        // 答错回原位置移动时间
#define TOPIC_ZORDER                    3
#define HINT_GAF_FILE                   "res/stage018/XQ-GK06-4-TM01-D/XQ-GK06-4-TM01-D.gaf"
#define HINT_GAF_POS                    Vec2(1306,984)
#define HINT_GAF_ZORDER                 3

/*
 *  constructor
 */
Round018_04::Round018_04()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(837, 602);
    m_answersPos[1] = Vec2(1002, 602);
    m_answersPos[2] = Vec2(1167, 602);
}

/*
 *  destructor
 */
Round018_04::~Round018_04()
{
    cleanUp();
}

//
void Round018_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(HINT_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(BRIDGE_PNG_FILE);
}

//
Round018_04* Round018_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round018_04();
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
bool Round018_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    m_pMSActor->playWithSequenceAliasName(STANDBY, true);
    
    m_pHintActor = Actor::createWithGafFile(HINT_GAF_FILE, true, 1.0f, Vec2(-24.0f, 175.0f));
    m_pHintActor->setPosition(HINT_GAF_POS);
    addChild(m_pHintActor, HINT_GAF_ZORDER);
    m_pHintActor->setSize(Size(150.0f, 150.0f));
    
    m_pBridgeSp = Sprite::create(BRIDGE_PNG_FILE);
    m_pBridgeSp->setPosition(BRIDGE_POS);
    addChild(m_pBridgeSp, BRIDGE_ZORDER);
    
    char buffer[64];
    float topicX = 687.0f, offSetX = 115.0f, topicY = 1050.0f;
    for (unsigned int i = 0; i < 8; i++)
    {
        sprintf(buffer, "XQ-GK06-4-DA%02d.png", i % 3 + 1);
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(Vec2(topicX + offSetX * i, topicY));
        addChild(m_pTopicsSp[i], TOPIC_ZORDER);
        m_pTopicsSp[i]->setScale(0.8f);
    } /*for*/
    m_pTopicsSp[6]->setVisible(false);
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK06-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/

    
    return true;
}

// 进入本回合
void Round018_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round018_04::scaleChangeGoodsSchedule), 0.6f, -1, 0);
}

//改变尺寸以提示
void Round018_04::scaleChangeGoodsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round018_04::scaleChangeGoodsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round018_04::openTouch()
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pHintActor->getContentSize().width, m_pHintActor->getContentSize().height);
        long index = (long)target->getUserData();
        bool isCorrect = false;
        
        target->setScale(1.0f);
        if (rect.containsPoint(m_pHintActor->convertTouchToNodeSpace(touch)))
        {
            if (index == 0) {   //答对
                isCorrect = true;
                target->setVisible(false);
                m_pTopicsSp[6]->setVisible(true);
                m_pHintActor->setVisible(false);
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1)
                {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }
        }
        if (!isCorrect) {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//隐藏剩余答案
void Round018_04::fadeOutAnswers()
{
    m_pAnswersSp[1]->runAction(FadeOut::create(0.4f));
    m_pAnswersSp[2]->runAction(FadeOut::create(0.4f));
}

// 答对后依次消失题目
void Round018_04::changeScaleTopicsSchedule(float)
{
    m_pTopicsSp[m_InitAniIndex++]->runAction(EaseExponentialOut::create(ScaleTo::create(0.2f, 0.0f)));
    if (m_InitAniIndex == 8)
    {
        unschedule(CC_SCHEDULE_SELECTOR(Round018_04::changeScaleTopicsSchedule));
        changeActor();          //改变ms动作
    }
}

//改变actor
void Round018_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]
    {
        fadeOutAnswers();
        upMsActor();
        downBridge();
    });
}

//上移actor
void Round018_04::upMsActor()
{
    m_pMSActor->runAction(MoveBy::create(1.5f, Vec2(0.0f, 1500)));
}

//下移桥
void Round018_04::downBridge()
{
    m_pBridgeSp->runAction(Sequence::create(MoveBy::create(1.5f, Vec2(0.0f, -675.0f)),CallFunc::create([this]{
        moveActor();        //主角移动
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round018_04::fadeOutLayer), 3.0f);
    }), NULL));
}

/*
 *  庆祝
 */
void Round018_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round018_04::endRound() {}

/*
 *  分享界面点击ok后移动主角
 */
void Round018_04::moveActor()
{
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0), true);
}

//渐变变黑退出
void Round018_04::fadeOutLayer(float)
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
void Round018_04::roundEndShareCallBack(bool isShare)
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
    
    m_InitAniIndex = 0;
    schedule(CC_SCHEDULE_SELECTOR(Round018_04::changeScaleTopicsSchedule), 0.2f, -1, 0.5f);
}

//分享回调
void Round018_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round018_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round018_04::roundEndShareCallBack, this,true));
}

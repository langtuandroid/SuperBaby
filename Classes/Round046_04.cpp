//
//  Round046_04.cpp
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#include "Round046_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "ThemeSelScene.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define MOVE_ACTOR_TIME                 3.0f                              //分享界面ok过后主角移动时间
#define COLORLAYER_TAG                  300
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define ANSWERS_ZORDER                  2
#define MS_ZORDER                   1

/*
 *  constructor
 */
Round046_04::Round046_04()
{
    m_answersPos[0] = Vec2(657, 1097);
    m_answersPos[1] = Vec2(1121, 1097);
    m_answersPos[2] = Vec2(1545, 1097);
    
    m_msesPos[0] = Vec2(762,392);
    m_msesPos[1] = Vec2(1193,392);
    m_msesPos[2] = Vec2(1624,392);
    
    m_answersCorPos[0] = Vec2(769, 588);
    m_answersCorPos[1] = Vec2(1204, 583);
    m_answersCorPos[2] = Vec2(1622, 591);
}

/*
 *  destructor
 */
Round046_04::~Round046_04()
{
    cleanUp();
}

//
Round046_04* Round046_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round046_04();
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
bool Round046_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK10-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
//        m_pAnswersSp[i]->setScale(0.65f);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
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
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "res/stage046/ms04_%02ld.json",i+1);
        m_pMSActor[i] = Actor::createWithGafConfigFile(buffer);
        m_pMSActor[i]->playWithSequenceAliasName(STANDBY,true);
        m_pMSActor[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        m_pMSActor[i]->setPosition(m_msesPos[i]);
        this->addChild(m_pMSActor[i], MS_ZORDER);
    }

    return true;
}

void Round046_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round046_04::onEnterRound()
{
    log("THIS pos X== %f",this->getPositionX());
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round046_04::changeScaleAnswersSchedule), 0.4f, -1, 0);   //
}

//改变答案大小
void Round046_04::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round046_04::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

//
void Round046_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage046/GC-GK10-4-MS03-D+C/GC-GK10-4-MS03-D+C.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage046/GC-GK10-4-MS02-D+C/GC-GK10-4-MS02-D+C.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage046/GC-GK10-4-MS01-D+C/GC-GK10-4-MS01-D+C.gaf");
}

/* 开启触摸 */
void Round046_04::openTouch()
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
            target->setLocalZOrder(5);
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
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
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        int bubbleIndex = 0;
        Rect rect = Rect(0.0f, 0.0f, m_pMSActor[0]->getContentSize().width, m_pMSActor[0]->getContentSize().height);
        if (rect.containsPoint(m_pMSActor[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pMSActor[1]->getContentSize().width, m_pMSActor[1]->getContentSize().height);
        if (rect.containsPoint(m_pMSActor[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pMSActor[2]->getContentSize().width, m_pMSActor[2]->getContentSize().height);
        if (rect.containsPoint(m_pMSActor[2]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
                bubbleIndex = 2;
            }
        }
        if (isCorrect) {
            m_curCorrectNum++;
            target->runAction(Sequence::create(Spawn::create(ScaleTo::create(0.5f, 0.65f),MoveTo::create(0.5f, m_answersCorPos[bubbleIndex]), NULL),Spawn::create(FadeOut::create(0.3f),CallFunc::create([this,bubbleIndex]{
                changeActor(bubbleIndex);
                if (m_curCorrectNum == 3) {
                    moveMSActors();
                }
            }), NULL), NULL));
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 3) {
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round046_04::changeActor(const unsigned int index)
{
    m_pMSActor[index]->playWithSequenceAliasName(TRIGGER,true);
}

void Round046_04::moveMSActors()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pMSActor[i]->runAction(Spawn::create(ScaleTo::create(0.5f, 0.5f),MoveBy::create(0.5f, Vec2(0.0f, 200.0f)), NULL));
    }
}

/*
 *  庆祝
 */
void Round046_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round046_04::endRound() {}

//回合结束分享调用
void Round046_04::roundEndShareCallBack(bool isShare)
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
    moveActor();
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round046_04::fadeOutLayer), 3.0f);
}

/*
 *  分享界面点击ok后移动主角
 */
void Round046_04::moveActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, 50.0f),true);
}

//渐变变黑退出
void Round046_04::fadeOutLayer(float)
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
void Round046_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round046_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round046_04::roundEndShareCallBack, this,true));
}
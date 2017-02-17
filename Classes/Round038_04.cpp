//
//  Round038_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/21.
//
//

#include "Round038_04.h"
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
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define ANSWERS_ZORDER                  4
#define WOOD_PNG_FILE                   "GC-GK04-TM01.png"
#define WOOD_POS                        Vec2(1062,919)
#define WOOD_ZORDER                     1
#define TOPICS_ZORDER                   2
#define MS_GAF_CONFIG_FILE              "res/stage038/ms.json"
#define MS_GAF_FILE                     "res/stage038/GC-GK02-4-MS01-D+C/GC-GK02-4-MS01-D+C.gaf"
#define MS_POS                          Vec2(1735,382)
#define MS_ZORDER                       1
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画

/*
 *  constructor
 */
Round038_04::Round038_04()
{
    m_answersPos[0] = Vec2(833, -100);
    m_answersPos[1] = Vec2(1062, -100);
    m_answersPos[2] = Vec2(1291, -100);
    
    m_topicsPos[0] = Vec2(757, 700);
    m_topicsPos[1] = Vec2(1062, 700);
    m_topicsPos[2] = Vec2(1378, 700);

}

/*
 *  destructor
 */
Round038_04::~Round038_04()
{
    cleanUp();
}

//
void Round038_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    
}

//
Round038_04* Round038_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round038_04();
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
bool Round038_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pWoodSp = Sprite::createWithSpriteFrameName(WOOD_PNG_FILE);
    m_pWoodSp->setPosition(WOOD_POS);
    this->addChild(m_pWoodSp, WOOD_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK04-TM%02ld.png",i+2);
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicsSp[i], TOPICS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK04-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y+250);
        all++;
    } /*while*/
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    return true;
}

void Round038_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round038_04::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round038_04::upAnswersSchedule), 0.5f, -1, 0);   //
}

//上升答案
void Round038_04::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_04::upAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round038_04::changeScaleTopicsSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, 250.0f))));
}

void Round038_04::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round038_04::changeScaleTopicsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),NULL));
}

/* 开启触摸 */
void Round038_04::openTouch()
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
            target->setLocalZOrder(10);
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        
        bool isCorrect = false;
        unsigned int topicNum = 0;
        Rect rect = Rect(0.0f, 0.0f,
                         m_pTopicsSp[0]->getContentSize().width,
                         m_pTopicsSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[0]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 0)
            {
                topicNum = 0;
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pTopicsSp[1]->getContentSize().width,
                    m_pTopicsSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 1)
            {
                topicNum = 1;
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pTopicsSp[2]->getContentSize().width,
                    m_pTopicsSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[2]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 2)
            {
                topicNum = 2;
                isCorrect = true;
            }
        }
        
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pTopicsSp[topicNum]->getPosition());
            m_pTopicsSp[topicNum]->setVisible(false);
            target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.15f, 1.0f), NULL));
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (m_curCorrectNum == 3) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }else
            {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }
        }else
        {
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

void Round038_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        m_pWoodSp->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500.0f)));
        for (unsigned int i = 0; i < 3; i++) {
            m_pAnswersSp[i]->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500.0f)));
        }
        moveLeadActor();
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round038_04::fadeOutLayer), 2.0f);
    });
}

/*
 *  庆祝
 */
void Round038_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round038_04::endRound() {}

//回合结束分享调用
void Round038_04::roundEndShareCallBack(bool isShare)
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
    changeActor();
}

void Round038_04::moveLeadActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, 0.0f),true);
}

//渐变变黑退出
void Round038_04::fadeOutLayer(float)
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
void Round038_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round038_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round038_04::roundEndShareCallBack, this,true));
}
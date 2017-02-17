//
//  Round036_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/7.
//
//

#include "Round036_04.h"
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

using namespace ui;

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

#define MOVE_ACTOR_TIME                 3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define ANSWERS_ZORDER                  4
#define TOPIC_ZORDER                    2
#define MS_GAF_CONFIG_FILE              "res/stage036/exit.json"
#define MS_GAF_FILE                     "res/stage036/HY-GK12-4-IT-D1+C1+C2/HY-GK12-4-IT-D1+C1+C2.gaf"
#define MS_POS                          Vec2(872,656)
#define MS_ZORDER                       1
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(818,868)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGERONE                      "triggerone"                                // 触发动画
#define TRIGGERTWO                      "triggertwo"                                // 触发动画

/*
 *  constructor
 */
Round036_04::Round036_04()
:m_InitAniIndex(0)
{
    m_topicsPos[0] = Vec2(1052, 986);
    m_topicsPos[2] = Vec2(1296, 694);
    m_topicsPos[3] = Vec2(1414, 1207);
    m_topicsPos[1] = Vec2(1667, 862);
    
    m_answersPos[0] = Vec2(752, 340);
    m_answersPos[1] = Vec2(1078, 340);
    m_answersPos[2] = Vec2(1404, 340);
    m_answersPos[3] = Vec2(1730, 340);
}

/*
 *  destructor
 */
Round036_04::~Round036_04()
{
    cleanUp();
}

//
Round036_04* Round036_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round036_04();
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
bool Round036_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pPaopaoActor = Actor::createWithGafFile(PAOPAO_GAF_FILE);
    m_pPaopaoActor->setPosition(PAOPAO_GAF_POS);
    this->addChild(m_pPaopaoActor, PAOPAO_GAF_ZORDER);
    
    m_pWhiteLightActor = Actor::createWithGafFile(WHITELIGHT_GAF_FILE);
    m_pWhiteLightActor->setPosition(WHITELIGHT_GAF_POS);
    m_pWhiteLightActor->setSize(Size(300, 300));
    m_pWhiteLightActor->setPosAtNode(Vec2(-300, 400));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "HY-GK12-4-TM-D-%02ld.png",i+1);
        m_pTopicImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pTopicImageView[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicImageView[i], TOPIC_ZORDER);
    }
    
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "HY-GK12-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->setScale(0.0f);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[all]);
        m_answersEndPos[all] = m_answersPos[all];
        all++;
    } /*while*/

    return true;
}

void Round036_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round036_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round036_04::changeScaleAnswersSchedule), 0.3f, -1, 0);
}

//答案变大出现
void Round036_04::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_04::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round036_04::changeScaleHintSchedule), 0.9f, -1, 0);
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)));
}

//放大题目以提示
void Round036_04::changeScaleHintSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round036_04::changeScaleHintSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicImageView[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),CallFunc::create([this]{
        char buffer[64];
        sprintf(buffer, "HY-GK12-4-TM-D-%02dB.png",m_InitAniIndex+1);
        m_pTopicImageView[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
    }),DelayTime::create(0.2f),CallFunc::create([this]{
        char buffer[64];
        sprintf(buffer, "HY-GK12-4-TM-D-%02d.png",m_InitAniIndex+1);
        m_pTopicImageView[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
    }),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
}

//
void Round036_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(PAOPAO_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(WHITELIGHT_GAF_FILE);
}

/* 开启触摸 */
void Round036_04::openTouch()
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
            target->setLocalZOrder(10);
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
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        int bubbleIndex = 0;
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, m_pTopicImageView[0]->getContentSize().width, m_pTopicImageView[0]->getContentSize().height);
        if (rect.containsPoint(m_pTopicImageView[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicImageView[1]->getContentSize().width, m_pTopicImageView[1]->getContentSize().height);
        if (rect.containsPoint(m_pTopicImageView[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicImageView[2]->getContentSize().width, m_pTopicImageView[2]->getContentSize().height);
        if (rect.containsPoint(m_pTopicImageView[2]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
                bubbleIndex = 2;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicImageView[3]->getContentSize().width, m_pTopicImageView[3]->getContentSize().height);
        if (rect.containsPoint(m_pTopicImageView[3]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 3) {
                isCorrect = true;
                bubbleIndex = 3;
            }
        }

        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pTopicImageView[bubbleIndex]->getPosition());
            m_pTopicImageView[bubbleIndex]->setVisible(false);
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 4) {
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
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//隐藏答案
void Round036_04::fadeAnswers()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_pAnswersSp[i]->runAction(FadeOut::create(0.1f));
    }
}

//答对改变ms
void Round036_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGERONE);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        m_pMSActor->playWithSequenceAliasName(TRIGGERTWO);
        m_pStageScene->getAimeeActor()->setVisible(false);
        m_pStageScene->getBeibeiActor()->setVisible(false);
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round036_04::fadeOutLayer), 2.0f);
    });
}

/*
 *  庆祝
 */
void Round036_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round036_04::endRound() {}

//回合结束分享调用
void Round036_04::roundEndShareCallBack(bool isShare)
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
    fadeAnswers();
    changeActor();
}

//渐变变黑退出
void Round036_04::fadeOutLayer(float)
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
void Round036_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round036_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round036_04::roundEndShareCallBack, this,true));
}
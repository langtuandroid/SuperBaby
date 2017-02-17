//
//  Round039_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#include "Round039_04.h"
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
#define TOPIC_PNG_FILE                  "GC-GK03-4-TM-D.png"
#define TOPIC_UP_PNG_FILE               "GC-GK03-4-TM-C-1.png"
#define TOPIC_DOWN_PNG_FILE             "GC-GK03-4-TM-C-2.png"
#define TOPIC_END_PNG_FILE              "GC-GK03-4-TM-C.png"
#define TOPIC_POS                       Vec2(1218,871)
#define TOPIC_ZORDER                    1
#define TOPICS_ZORDER                   2
#define MS_GAF_CONFIG_FILE              "res/stage039/ms.json"
#define MS_GAF_FILE                     "res/stage039/GC-GK03-4-MS/GC-GK03-4-MS.gaf"
#define MS_POS                          Vec2(966,345)
#define MS_ZORDER                       1
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define SCREW_PNG_FILE                  "res/stage039/GC-GK03-4-MEN-3.png"
#define SCREW_POS                       Vec2(2006,816)
#define SCREW_ZORDER                    2
#define DOOR_PNG_FILE                   "GC-GK03-4-MEN-2.png"
#define DOOR_POS                        Vec2(1992,650)
#define DOOR_ZORDER                     3
#define OVER_PNG_FILE                   "res/stage039/GC-GK03-4-MEN-1.png"
#define OVER_POS                        Vec2(13957,816)
#define OVER_TAG                        203
//#define HINT_PNG_FILE                   "GC-GK03-4-TM02.png"
#define HINT_LIGHT_PNG_FILE             "GC-GK03-4-TM02B.png"

/*
 *  constructor
 */
Round039_04::Round039_04()
{
    m_answersPos[0] = Vec2(980, -100);
    m_answersPos[1] = Vec2(1214, -100);
    m_answersPos[2] = Vec2(1448, -100);
    
    m_hintsPos[0] = Vec2(1221, 981);
    m_hintsPos[1] = Vec2(1221, 763);
}

/*
 *  destructor
 */
Round039_04::~Round039_04()
{
    cleanUp();
}

//
void Round039_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    Director::getInstance()->getTextureCache()->removeTextureForKey(SCREW_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOOR_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
}

//
Round039_04* Round039_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round039_04();
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
bool Round039_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pTopicImageView = ImageView::create(TOPIC_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pTopicImageView->setPosition(TOPIC_POS);
    this->addChild(m_pTopicImageView, TOPIC_ZORDER);
    
    char buffer[64];
    for (unsigned int i = 0; i < 2; i++) {
        m_pHintsImageView[i] = ImageView::create(HINT_LIGHT_PNG_FILE,ui::Widget::TextureResType::PLIST);
        m_pHintsImageView[i]->setPosition(m_hintsPos[i]);
        m_pHintsImageView[i]->setScale(0.8f);
        m_pHintsImageView[i]->setVisible(false);
        this->addChild(m_pHintsImageView[i], ANSWERS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK03-4-DA%02ld.png",i+1);
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
    
    m_pScrewSp = Sprite::create(SCREW_PNG_FILE);
    m_pScrewSp->setPosition(SCREW_POS);
    this->addChild(m_pScrewSp, SCREW_ZORDER);
    
    m_pDoorSp = Sprite::createWithSpriteFrameName(DOOR_PNG_FILE);
    m_pDoorSp->setPosition(DOOR_POS);
    this->addChild(m_pDoorSp, DOOR_ZORDER);
    
    m_pOverSp = Sprite::create(OVER_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    return true;
}

void Round039_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round039_04::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round039_04::upAnswersSchedule), 0.5f, -1, 0);   //
}

//上升答案
void Round039_04::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_04::upAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round039_04::changeScaleHintsSchedule), 1.0f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, 250.0f))));
}
//改变提示问号大小
void Round039_04::changeScaleHintsSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_04::changeScaleHintsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pHintsImageView[m_InitAniIndex]->runAction(Sequence::create(CallFunc::create([this]{
        m_pHintsImageView[m_InitAniIndex]->setVisible(true);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHintsImageView[m_InitAniIndex]->setVisible(false);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHintsImageView[m_InitAniIndex]->setVisible(true);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHintsImageView[m_InitAniIndex]->setVisible(false);
        m_InitAniIndex++;
    }), NULL));
}

/* 开启触摸 */
void Round039_04::openTouch()
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
                         m_pHintsImageView[0]->getContentSize().width,
                         m_pHintsImageView[0]->getContentSize().height);
        if (rect.containsPoint(m_pHintsImageView[0]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 1)
            {
                topicNum = 0;
                isCorrect = true;
                if (m_curCorrectNum == 0) {
                    m_firstNum = 1;
                }
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pHintsImageView[1]->getContentSize().width,
                    m_pHintsImageView[1]->getContentSize().height);
        if (rect.containsPoint(m_pHintsImageView[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 2)
            {
                topicNum = 1;
                isCorrect = true;
                if (m_curCorrectNum == 0) {
                    m_firstNum = 2;
                }
            }
        }
        
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pHintsImageView[topicNum]->getPosition());
            m_pHintsImageView[topicNum]->setVisible(false);
            target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.15f, 1.0f),CallFunc::create([this,target]{
                if (m_firstNum == 1 && m_curCorrectNum == 1) {
                    m_pTopicImageView->loadTexture(TOPIC_UP_PNG_FILE,ui::Widget::TextureResType::PLIST);
                    target->setVisible(false);
                } else if (m_firstNum == 2 && m_curCorrectNum == 1)
                {
                    m_pTopicImageView->loadTexture(TOPIC_DOWN_PNG_FILE,ui::Widget::TextureResType::PLIST);
                    target->setVisible(false);
                } else if (m_curCorrectNum == 2)
                {
                    m_pTopicImageView->loadTexture(TOPIC_END_PNG_FILE,ui::Widget::TextureResType::PLIST);
                    target->setVisible(false);
                }
                
            }), NULL));
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (m_curCorrectNum == 2) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                fadeOutAnswers();
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

void Round039_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        m_pDoorSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_pDoorSp->getContentSize().width + 16.0f, 50.0f)), Hide::create(), CallFunc::create([this]{
            moveLeadActor();
            scheduleOnce(CC_SCHEDULE_SELECTOR(Round039_04::fadeOutLayer), 2.0f);
        }), NULL));
    });
}

void Round039_04::fadeOutAnswers()
{
    m_pAnswersSp[0]->runAction(FadeOut::create(0.3f));
}

/*
 *  庆祝
 */
void Round039_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round039_04::endRound() {}

//回合结束分享调用
void Round039_04::roundEndShareCallBack(bool isShare)
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

void Round039_04::moveLeadActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, 100.0f),true);
}

//渐变变黑退出
void Round039_04::fadeOutLayer(float)
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
void Round039_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round039_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round039_04::roundEndShareCallBack, this,true));
}
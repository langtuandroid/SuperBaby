//
//  Round042_05.cpp
//  superBaby
//
//  Created by Administrator on 15/7/30.
//
//

#include "Round042_05.h"
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
#define COW_GAF_CONFIG_FILE             "res/stage042/ms.json"
#define COW_GAF_FILE                    "res/stage042/GK-GK06-4_MS01-D+C/GK-GK06-4_MS01-D+C.gaf"
#define COW_POS                         Vec2(844,565)
#define COW_ZORDER                      2
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define TARGET_POS                      Vec2(906,626)
/*
 *  constructor
 */
Round042_05::Round042_05()
{
    m_answersPos[0] = Vec2(771, -200);
    m_answersPos[1] = Vec2(1081, -200);
    m_answersPos[2] = Vec2(1353, -200);
    
}

/*
 *  destructor
 */
Round042_05::~Round042_05()
{
    cleanUp();
}

//
void Round042_05::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(COW_GAF_FILE);
}

//
Round042_05* Round042_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round042_05();
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
bool Round042_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pCowActor = Actor::createWithGafConfigFile(COW_GAF_CONFIG_FILE);
    m_pCowActor->playWithSequenceAliasName(STANDBY,true);
    m_pCowActor->setPosition(COW_POS);
    this->addChild(m_pCowActor, COW_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK06-4-DA%02ld.png",i+1);
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
        m_answersEndPos[all] = Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y+450);
        all++;
    } /*while*/

    return true;
}

void Round042_05::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round042_05::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round042_05::upAnswersSchedule), 0.6f, -1, 0);   //
}

//上升答案
void Round042_05::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round042_05::upAnswersSchedule));
        changeScaleActor();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.4f, Vec2(0.0f, 450))));
}

void Round042_05::changeScaleActor()
{
    m_pCowActor->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round042_05::openTouch()
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
        auto index = (long)target->getUserData();
        auto touchPoint = m_pCowActor->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pCowActor->getContentSize().width, m_pCowActor->getContentSize().height);
        target->setScale(1.0f);
        
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) {
                target->setPosition(TARGET_POS);
                target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),Spawn::create(ScaleTo::create(0.15f, 0.4f),FadeOut::create(0.2f), NULL),CallFunc::create([this]{
                    changeActor();
                }), NULL));
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
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
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(1.5f);
                
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
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

void Round042_05::changeActor()
{
    m_pCowActor->playWithSequenceAliasName(TRIGGER,true);
}

//下降答案
void Round042_05::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round042_05::downAnswersSchedule));
        moveLeadActor();
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round042_05::fadeOutLayer), 0.0f);
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(MoveBy::create(0.4f, Vec2(0.0f, 50)),MoveBy::create(0.4f, Vec2(0.0f, -500.0f)),NULL));
}

/*
 *  庆祝
 */
void Round042_05::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round042_05::endRound() {}

//回合结束分享调用
void Round042_05::roundEndShareCallBack(bool isShare)
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
    m_InitAniIndex = 1;
    this->schedule(CC_SCHEDULE_SELECTOR(Round042_05::downAnswersSchedule), 0.8f, -1, 0);   //
}

void Round042_05::moveLeadActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, 0.0f),true);
}

//渐变变黑退出
void Round042_05::fadeOutLayer(float)
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
void Round042_05::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round042_05::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round042_05::roundEndShareCallBack, this,true));
}
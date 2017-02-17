//
//  Round014_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/12.
//
//

#include "Round014_04.h"
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

#define STANDBY                        "standby"                                // 待机动画
#define TRIGGER                        "trigger"                                // 触发动画
#define CELEBRATE_DELAY                 5.0f
#define NUMBER_ZORDER                   3
#define MARK_ZORDER                     2
#define REEL_ACTOR_ZORDER               1
#define REEL_GAF_CONFIG_FILE            "config/stage014/reel.json"
#define REEL_GAF_POS                    Vec2(600,700)
#define MARK_POS                        Vect(1033,691)
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define MOVE_ACTOR_TIME                 2.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
// 飞船
#define AIR_SHIP                        "res/common/XQ-GK01-2-IT01.png"
#define AIR_SHIP_TAG                    600
#define AIR_SHIP_POS                    Vec2(13115,1915)
/*
 *  constructor
 */
Round014_04::Round014_04()
:m_InitAniIndex(0)
{
    
    m_answerNumbersPos[0] = Vec2(880, -100);
    m_answerNumbersPos[1] = Vec2(1165, -100);
    m_answerNumbersPos[2] = Vec2(1441, -100);
}

/*
 *  destructor
 */
Round014_04::~Round014_04()
{
    cleanUp();
}

//
void Round014_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    // 删除飞船
    m_pStageScene->removeOBLeaderNode(AIR_SHIP_TAG);
    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->removeTextureForKey(AIR_SHIP);
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage014/XQ-GK02-4-TM-D+C/XQ-GK02-4-TM-D+C.gaf");
}

//
Round014_04* Round014_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round014_04();
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
bool Round014_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ6-%02ld.png",2*i+3);
        m_pAnswersNumber[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pAnswersNumber[i], NUMBER_ZORDER);
        m_pAnswersNumber[i]->setUserData((void*)i);
    }
    
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int numberIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[numberIndex]) continue;
        selected[numberIndex] = true;
        m_pAnswersNumber[all]->setPosition(m_answerNumbersPos[numberIndex]);
        m_answerNumbersEndPos[all] = Vec2(m_answerNumbersPos[numberIndex].x, m_answerNumbersPos[numberIndex].y+250);
        all++;
    } /*while*/
    
    
    m_pReelActor = Actor::createWithGafConfigFile(REEL_GAF_CONFIG_FILE);
    m_pReelActor->setPosition(REEL_GAF_POS);
    this->addChild(m_pReelActor, REEL_ACTOR_ZORDER);
    m_pReelActor->playWithSequenceAliasName(STANDBY,true);
    
    m_pTargetMarkSp = Sprite::createWithSpriteFrameName("XQ-GK02-4-TM01-5.png");
    m_pTargetMarkSp->setPosition(MARK_POS);
    this->addChild(m_pTargetMarkSp, MARK_ZORDER);
    
    // 飞船
    m_pAirSp = Sprite::create(AIR_SHIP);
    m_pAirSp->setScale(0.8f);
    m_pStageScene->addBelowLeaderNode(m_pAirSp, AIR_SHIP_POS, AIR_SHIP_TAG);
    
    return true;
}

// 进入本回合
void Round014_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round014_04::upAnswersNumberSchedule), 0.6f, -1, 0);

}


//答案上升
void Round014_04::upAnswersNumberSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_04::upAnswersNumberSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round014_04::scaleChangeMarkSchedule), 0.85f, -1, 0);
        return;
    }
    m_pAnswersNumber[m_InitAniIndex]->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, 250.0f))),CallFunc::create([this]{
        m_pAnswersNumber[m_InitAniIndex]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        m_InitAniIndex++;
    }), NULL));
}

//渐变问号以提示
void Round014_04::scaleChangeMarkSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_04::scaleChangeMarkSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round014_04::scaleChangeSchedule), 0.85f, -1, 0);
        return;
    }
    m_pTargetMarkSp->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
}

//改变答案大小
void Round014_04::scaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_04::scaleChangeSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersNumber[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.2f),ScaleTo::create(0.4f, 1.0f), NULL));
}

/* 开启触摸 */
void Round014_04::openTouch()
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, m_pTargetMarkSp->getContentSize().width, m_pTargetMarkSp->getContentSize().height);
        if (rect.containsPoint(m_pTargetMarkSp->convertTouchToNodeSpace(touch))) {
            if (index == 1) {   //答对
                target->setVisible(false);
                m_pTargetMarkSp->setVisible(false);
                changeActor();          //改变转轮动作
                fadeOutAnswers();       //隐藏剩余答案
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersNumber[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answerNumbersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_answerNumbersEndPos[index]);
                }), NULL));
            }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answerNumbersEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_answerNumbersEndPos[index]);
            }), NULL));
        }

    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_answerNumbersEndPos[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersNumber[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersNumber[i]);
    } /*for*/
    
}

//隐藏剩余答案
void Round014_04::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswersNumber[i]->runAction(Spawn::create(FadeOut::create(0.5f),MoveBy::create(1.0f, Vec2(0.0f, -250)), NULL));
        }
    }
}

//改变actor
void Round014_04::changeActor()
{
    m_pReelActor->playWithSequenceAliasName(TRIGGER,false);
}

/*
 *  庆祝
 */
void Round014_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round014_04::endRound()
{
    
}

//下降飞梯
void Round014_04::downFlyLadder()
{
    m_pAirSp->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0.0f, -1500)),CallFunc::create([this]{
        moveActor();
    }), NULL));
}

/*
 *  分享界面点击ok后移动主角
 */

void Round014_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();

    m_pStageScene->moveAimiActorCB(MOVE_ACTOR_TIME, Vec2(800, 0), [this]{
        m_pStageScene->moveAimiActorCB(1.25f, Vec2(500, 150),[this]{
            m_pStageScene->moveAimiActorCB(0.25f, Vec2(100, 0.0f),[this]{
                airShipUp();
            });
        });
    },false);
    m_pStageScene->moveBeibeiActorCB(MOVE_ACTOR_TIME+0.55f, Vec2(1020, 0), [this]{
        m_pStageScene->moveBeibeiActor(0.95f, Vec2(380, 150),true);
    },false);
}

//飞船上升
void Round014_04::airShipUp()
{
    m_pAirSp->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500)));
    m_pStageScene->moveLeadingActor(2.0f, Vec2(0.0f, 1500));
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round014_04::fadeOutLayer), 2.5f);

}


//渐变变黑退出
void Round014_04::fadeOutLayer(float)
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
void Round014_04::roundEndShareCallBack(bool isShare)
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
    downFlyLadder();                //飞船下落
}

//分享回调
void Round014_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round014_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round014_04::roundEndShareCallBack, this,true));
}
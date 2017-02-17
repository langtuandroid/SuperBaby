//
//  Round046_03.cpp
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#include "Round046_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MS_ZORDER                       1
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define ANSWERS_ZORDER                  2

/*
 *  constructor
 */
Round046_03::Round046_03()
{
    m_answersPos[0] = Vec2(734, 500);
    m_answersPos[1] = Vec2(1004, 500);
    m_answersPos[2] = Vec2(1274, 500);
    m_answersPos[3] = Vec2(1544, 500);
    
    m_anwersEndPos[0] = Vec2(761, 330);
    m_anwersEndPos[1] = Vec2(1123, 315);
    m_anwersEndPos[2] = Vec2(1455, 324);
    
    m_msesPos[0] = Vec2(687, 305);
    m_msesPos[1] = Vec2(1048, 305);
    m_msesPos[2] = Vec2(1392, 305);
}

/*
 *  destructor
 */
Round046_03::~Round046_03()
{
    cleanUp();
}

//
Round046_03* Round046_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round046_03();
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
bool Round046_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        if (i == 1) {
            sprintf(buffer, "GC-GK10-3-DA02.png");
        } else
        {
            sprintf(buffer, "GC-GK10-3-DA01.png");
        }
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        m_pAnswersSp[i]->setOpacity(0);
        m_pAnswersSp[i]->setPosition(m_answersPos[i]);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "res/stage046/ms03_%02ld.json",i+1);
        m_pMSActor[i] = Actor::createWithGafConfigFile(buffer);
        m_pMSActor[i]->playWithSequenceAliasName(STANDBY,true);
        m_pMSActor[i]->setPosition(m_msesPos[i]);
        this->addChild(m_pMSActor[i], MS_ZORDER);
    }

    return true;
}

// 进入本回合
void Round046_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round046_03::upChangeScaleAnswersSchedule), 1.5f, -1, 0);   //
}

//下降出现并改变答案大小
void Round046_03::upChangeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round046_03::upChangeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex]->runAction(Sequence::create(Spawn::create(MoveBy::create(0.5f, Vec2(0.0f, -450.0f)),FadeIn::create(0.5f), NULL),CallFunc::create([this]{
        m_pAnswersSp[m_InitAniIndex]->setScaleX(1.05f);
        m_pAnswersSp[m_InitAniIndex]->setScaleY(0.95f);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pAnswersSp[m_InitAniIndex]->setScaleX(1.0f);
        m_pAnswersSp[m_InitAniIndex]->setScaleY(1.0f);
        m_InitAniIndex++;
    }), NULL));
}

//
void Round046_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage046/GC-GK10-3-MS01-D+C/GC-GK10-3-MS01-D+C.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage046/GC-CK10-3-MS02-D+C/GC-CK10-3-MS02-D+C.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage046/GC-CK10-3-MS03-D+C/GC-CK10-3-MS03-D+C.gaf");
}

/* 开启触摸 */
void Round046_03::openTouch()
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
            if (index == 1) {
                target->runAction(FadeOut::create(0.3f));
                moveAnswers();
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
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
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
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round046_03::moveAnswers()
{
    unsigned int index = 0;
    for (unsigned int i = 0; i < 4; i++) {
        if (i != 1) {
            index = i;
            if (i > 1) {
                index = index - 1;
            }
            m_pAnswersSp[i]->runAction(Sequence::create(Spawn::create(MoveTo::create(0.5f, m_anwersEndPos[index]),ScaleTo::create(0.5f, 0.5f), NULL),Spawn::create(FadeOut::create(0.1f),CallFunc::create([this,index]{
                changeActor(index);
            }), NULL), NULL));
        }
    }
}

void Round046_03::changeActor(const unsigned int index)
{
    m_pMSActor[index]->playWithSequenceAliasName(TRIGGER,true);
}

/*
 *  庆祝
 */
void Round046_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/*  回合结束回调 */
void Round046_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
//
//  Round041_03.cpp
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#include "Round041_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define NEST_PNG_FILE                   "GC-GK05-02-IT02.png"
#define NEST_POS                        Vec2(1016,1286)
#define NEST_ZORDER                     1
#define NEST_OVER_PNG_FILE              "GC-GK05-02-IT01.png"
#define NEST_OVER_POS                   Vec2(1019,1079)
#define NEST_OVER_ZORDER                3
#define MS_CONFIG_GAF_FILE              "res/stage041/ms01.json"
#define MS_GAF_FILE                     "res/stage041/GC-GK05-02-MS01-C+D1+D2/GC-GK05-02-MS01-C+D1+D2.gaf"
#define MS_POS                          Vec2(944,1093)
#define MS_ZORDER                       2
#define STANDBY                         "standby"                                    // 待机动画
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round041_03::Round041_03()
{
    m_answersPos[0] = Vec2(685, 222);
    m_answersPos[1] = Vec2(960, 298);
    m_answersPos[2] = Vec2(1250, 245);
    m_answersPos[3] = Vec2(1501, 298);
    m_answersPos[4] = Vec2(1778, 222);
}

/*
 *  destructor
 */
Round041_03::~Round041_03()
{
    cleanUp();
}

//
Round041_03* Round041_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round041_03();
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
bool Round041_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 5; i++) {
        sprintf(buffer, "GC-GK05-02-DA%02ld.png",i+1);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswers[i]->setUserData((void*)i);
        m_pAnswers[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        addChild(m_pAnswers[i],ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswers[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    m_pNestSp = Sprite::createWithSpriteFrameName(NEST_PNG_FILE);
    m_pNestSp->setPosition(NEST_POS);
    this->addChild(m_pNestSp, NEST_ZORDER);
    
    m_pNestOverSp = Sprite::createWithSpriteFrameName(NEST_OVER_PNG_FILE);
    m_pNestOverSp->setPosition(NEST_OVER_POS);
    this->addChild(m_pNestOverSp, NEST_OVER_ZORDER);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_CONFIG_GAF_FILE);
    m_pMSActor->setPosition(MS_POS);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    this->addChild(m_pMSActor, MS_ZORDER);

    return true;
}

// 进入本回合
void Round041_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round041_03::scaleChangeAnswersSchedule), 0.6f, -1, 0);
}

//改变答案的大小
void Round041_03::scaleChangeAnswersSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round041_03::scaleChangeAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswers[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//
void Round041_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

/* 开启触摸 */
void Round041_03::openTouch()
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pMSActor->getContentSize().width,
                         m_pMSActor->getContentSize().height);
        if (rect.containsPoint(m_pMSActor->convertTouchToNodeSpace(touch)))
        {
            if (index == 4) {   //答对
                target->runAction(FadeOut::create(0.3f));
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
                fadeOutAll();
                for (unsigned int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswers[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
            }
        } else
        {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswers[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswers[i]);
    } /*for*/
}

void Round041_03::changeActor()
{
    std::vector<std::string> aliasName;
    aliasName.push_back("triggerone");
    aliasName.push_back("triggertwo");
    m_pMSActor->playWithSequenceAliasName(aliasName,true);
}
void Round041_03::fadeOutAll()
{
    for (unsigned int i = 0; i < 5; i++) {
        if (i != 4) {
            m_pAnswers[i]->runAction(Spawn::create(MoveBy::create(0.4f, Vec2(0.0f, -200.0f)),FadeOut::create(0.5f), NULL));
        }
    }
}

/*
 *  庆祝
 */
void Round041_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

/*  回合结束回调 */
void Round041_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
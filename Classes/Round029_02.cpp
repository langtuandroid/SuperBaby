//
//  Round029_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Round029_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(1118,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(518,868)
#define WHITELIGHT_GAF_ZORDER           1
#define ANSWERS_ZORDER                 3
#define TOPIC_ZORDER                   2
#define MS_GAF_CONFIG_FILE              "res/stage027/ms.json"
#define MS_GAF_FILE                    "res/stage027/HY-GK05-2-MS01-D+C/HY-GK05-2-MS01-D+C.gaf"
#define MS_GAF_POS                     Vec2(963,508)
#define MS_GAF_ZORDER                  1
#define HINT_PNG_FILE                   "HY-GK05-2-TM02.png"
#define HINT_FADE_FILE                  "HY-GK05-2-TM02-2.png"
#define HINT_POS                        Vec2(1501,1051)
#define HINT_ZORDER                     2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
/*
 *  constructor
 */
Round029_02::Round029_02()
{
    
    m_answersPos[0] = Vec2(873, 651);
    m_answersPos[1] = Vec2(1114, 651);
    m_answersPos[2] = Vec2(1362, 651);
    
    m_topicsPos[0] = Vec2(650, 1051);
    m_topicsPos[1] = Vec2(853, 1051);
    m_topicsPos[2] = Vec2(1073, 1051);
    m_topicsPos[3] = Vec2(1277, 1051);
    m_topicsPos[4] = Vec2(1501, 1051);
    
}

/*
 *  destructor
 */
Round029_02::~Round029_02()
{
    cleanUp();
}

//
void Round029_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

//
Round029_02* Round029_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round029_02();
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
bool Round029_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
  
    char buffer[64];
    for (unsigned long i = 0; i < 5; i++) {
        if (i == 0) {
            sprintf(buffer, "SZ4-01.png");
        }else if (i == 2)
        {
            sprintf(buffer, "SZ4-04.png");
        }else if (i == 4)
        {
            sprintf(buffer, "SZ4-05.png");
        }else if (i == 1)
        {
            sprintf(buffer, "HY-GK05-2-TM01.png");
        }else
        {
            sprintf(buffer, "HY-GK05-2-TM01-2.png");
        }
        
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_topicsPos[i]);
        if (i == 4) {
            m_pTopicsSp[i]->setVisible(false);
        }
        this->addChild(m_pTopicsSp[i], TOPIC_ZORDER);
        if (i%2 == 0) {
            m_pTopicsSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else
        {
            m_pTopicsSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
        }
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    
    m_pHintFadeSp = Sprite::createWithSpriteFrameName(HINT_FADE_FILE);
    m_pHintFadeSp->setPosition(HINT_POS);
    this->addChild(m_pHintFadeSp, HINT_ZORDER);
    m_pHintFadeSp->setVisible(false);
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ4-%02ld.png",i+4);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
        m_pAnswersSp[i]->setScale(0.0f);
        if (i%2 == 0) {
            m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else
        {
            m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
        }
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
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    m_pMSActor->setSize(Size(300, 300));
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);

    return true;
}

// 进入本回合
void Round029_02::onEnterRound()
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
    
    changeScaleHint();
}

//依次放大答案以提示
void Round029_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round029_02::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

//改变大小以提示
void Round029_02::changeScaleHint()
{
    m_pHintSp->runAction(Sequence::create(ScaleTo::create(0.4f, 1.2f),
                                                 CallFunc::create([this]{
        m_pHintSp->setVisible(false);
        m_pHintFadeSp->setVisible(true);
    }),
                                                 DelayTime::create(0.2f),
                                                 CallFunc::create([this]{
        m_pHintFadeSp->setVisible(false);
        m_pHintSp->setVisible(true);
    }),
                                                 ScaleTo::create(0.3f, 1.0f),
                                                 ScaleTo::create(0.3f, 1.2f),
                                                 CallFunc::create([this]{
        m_pHintSp->setVisible(false);
        m_pHintFadeSp->setVisible(true);
    }),
                                                 DelayTime::create(0.2f),
                                                 CallFunc::create([this]{
        m_pHintFadeSp->setVisible(false);
        m_pHintSp->setVisible(true);
    }),
                                                 ScaleTo::create(0.3f, 1.0f),
                                                 CallFunc::create([this]{
        this->schedule(CC_SCHEDULE_SELECTOR(Round029_02::changeScaleAnswersSchedule), 0.4f, -1, 0);   //
    }), NULL));
}

/* 开启触摸 */
void Round029_02::openTouch()
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
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp->getContentSize().width, m_pHintSp->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
                target->setPosition(m_pHintSp->getPosition());
                m_pHintSp->setVisible(false);
                target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.15f, 1.0f),Spawn::create(FadeOut::create(0.3f),CallFunc::create([this,target]{
                }), NULL), NULL));
                m_pTopicsSp[4]->setVisible(true);
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
                fadeAnswers();
                changeMS();
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

void Round029_02::changeMS()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
}

//答对隐藏答案
void Round029_02::fadeAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswersSp[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

void Round029_02::moveMS()
{
    m_pMSActor->runAction(Sequence::create(MoveTo::create(3.0f, Vec2(2548, 2036)),CallFunc::create([this]{
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
}

void Round029_02::fadeTopics()
{
    for (unsigned int i = 0; i < 5; i++) {
        m_pTopicsSp[i]->runAction(FadeOut::create(0.3f));
    }
}

/*
 *  庆祝
 */
void Round029_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round029_02::endRound()
{
    fadeTopics();
    moveMS();
}
//
//  Round033_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/10.
//
//

#include "Round033_02.h"
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
#define WHITELIGHT_GAF_POS              Vec2(272,795)
#define WHITELIGHT_GAF_ZORDER           1
#define ANSWERS_ZORDER                  3
#define TOPICS_ZORDER                   2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round033_02::Round033_02()
:m_InitAniIndex(0)
{
    
    m_answersPos[0] = Vec2(945, 443);
    m_answersPos[1] = Vec2(1294, 443);
    
    m_topicsPos[0] = Vec2(222, 1067);
    m_topicsPos[1] = Vec2(529, 1067);
    m_topicsPos[2] = Vec2(836, 1067);
    m_topicsPos[3] = Vec2(1143, 1067);
    m_topicsPos[4] = Vec2(1450, 1067);
    m_topicsPos[5] = Vec2(1757, 1067);
}

/*
 *  destructor
 */
Round033_02::~Round033_02()
{
    cleanUp();
}

//
void Round033_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round033_02* Round033_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round033_02();
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
bool Round033_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    m_pWhiteLightActor->setPosAtNode(Vec2(-151, 747));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    char buffer[64];
    
    for (unsigned int i = 0; i < 6; i++) {
        if (i == 5 || i == 4) {
            sprintf(buffer, "HY-GK09-2-DA03.png");
        }else if (i % 2 == 0)
        {
            sprintf(buffer, "HY-GK09-2-DA01.png");
        }else if (i % 2 == 1)
        {
            sprintf(buffer, "HY-GK09-2-DA02.png");
        }
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicsSp[i], TOPICS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK09-2-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round033_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round033_02::changeScaleAnswersSchedule), 0.5f, -1, 0);   //
}

//改变答案大小以提示
void Round033_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round033_02::changeScaleAnswersSchedule));
        m_InitAniIndex = 4;
        this->schedule(CC_SCHEDULE_SELECTOR(Round033_02::changeScaleTopicsSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)));
}

//改变题目大小以提示
void Round033_02::changeScaleTopicsSchedule(float)
{
    if (m_InitAniIndex >= 6) {
        unschedule(CC_SCHEDULE_SELECTOR(Round033_02::changeScaleTopicsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round033_02::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
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
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
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
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pTopicsSp[4]->getContentSize().width, m_pTopicsSp[4]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[4]->convertTouchToNodeSpace(touch)))
        {
            if (index == 0) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pTopicsSp[5]->getContentSize().width, m_pTopicsSp[5]->getContentSize().height);
        if (rect.containsPoint(m_pTopicsSp[5]->convertTouchToNodeSpace(touch))) {
            if (index == 1) {
                isCorrect = true;
            }
        }
        if (isCorrect) {   //答对
            m_curCorrectNum++;
            if (index == 1) {
                target->setPosition(m_pTopicsSp[5]->getPosition());
                m_pTopicsSp[5]->setVisible(false);
            }else
            {
                target->setPosition(m_pTopicsSp[4]->getPosition());
                m_pTopicsSp[4]->setVisible(false);
            }
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 2) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
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
    for (unsigned int i = 1; i < 2; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/

}

/*
 *  庆祝
 */
void Round033_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round033_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

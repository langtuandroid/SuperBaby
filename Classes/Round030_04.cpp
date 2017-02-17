//
//  Round030_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round030_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(818,868)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define BUBBLE_ZORDER                   2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define FISH_ONE_GAF_CONFIG_FILE        "res/stage030/fishone.json"
#define FISH_TWO_GAF_CONFIG_FILE        "res/stage030/fishtwo.json"
#define FISH_THREE_GAF_CONFIG_FILE      "res/stage030/fishthree.json"
#define FISH_ONE_GAF_FILE               "res/stage030/HY-GK06-3-MS01-D/HY-GK06-3-MS01-D.gaf"
#define FISH_TWO_GAF_FILE               "res/stage030/HY-GK06-3-MS02-D/HY-GK06-3-MS02-D.gaf"
#define FISH_THREE_GAF_FILE             "res/stage030/HY-GK06-3-MS03-D/HY-GK06-3-MS03-D.gaf"
#define FISH_ZORDER                     3

/*
 *  constructor
 */
Round030_04::Round030_04() {
    m_fishsPos[0] = Vec2(1198, 323);
    m_fishsPos[1] = Vec2(1596, 653);
    m_fishsPos[2] = Vec2(889, 559);
}

/*
 *  destructor
 */
Round030_04::~Round030_04()
{
    cleanUp();
}

//
void Round030_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(FISH_ONE_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(FISH_TWO_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(FISH_THREE_GAF_FILE);
}

//
Round030_04* Round030_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round030_04();
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
bool Round030_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    float bubbleX = 664; float bubbleY = 1113; float offSetX = 407;
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-3-DA02.png");
        m_pBubbleSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBubbleSp[i]->setScale((1.0f-0.1f*i));
        m_pBubbleSp[i]->setPosition(Vec2(bubbleX+offSetX*i, bubbleY));
        this->addChild(m_pBubbleSp[i], BUBBLE_ZORDER);
        if (i%2 == 0) {
            m_pBubbleSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else
        {
            m_pBubbleSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
        }
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK06-3-DA01.png");
        m_pHighlightBubbleSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pHighlightBubbleSp[i]->setScale((1.0f-0.1f*i)*1.2f);
        m_pHighlightBubbleSp[i]->setPosition(Vec2(bubbleX+offSetX*i, bubbleY));
        this->addChild(m_pHighlightBubbleSp[i], BUBBLE_ZORDER);
        m_pHighlightBubbleSp[i]->setVisible(false);
        if (i%2 == 0) {
            m_pHighlightBubbleSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else
        {
            m_pHighlightBubbleSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
        }
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 0) {
            m_pFishsActor[i] = Actor::createWithGafConfigFile(FISH_ONE_GAF_CONFIG_FILE);
        }else if (i == 1){
            m_pFishsActor[i] = Actor::createWithGafConfigFile(FISH_TWO_GAF_CONFIG_FILE);
        }else
        {
            m_pFishsActor[i] = Actor::createWithGafConfigFile(FISH_THREE_GAF_CONFIG_FILE);
        }
        m_pFishsActor[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
        m_pFishsActor[i]->setUserData((void*)i);
        this->addChild(m_pFishsActor[i], FISH_ZORDER);
        m_pFishsActor[i]->playWithSequenceAliasName(STANDBY,true);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pFishsActor[all]->setPosition(m_fishsPos[randIndex]);
        m_fishsEndPos[all] = m_fishsPos[randIndex];
        all++;
    } /*while*/
    return true;
}

// 进入本回合
void Round030_04::onEnterRound()
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
    changeScaleFishs();
    this->schedule(CC_SCHEDULE_SELECTOR(Round030_04::changeScaleBubblesSchedule), 1.1f, -1, 2.0f);
}

//改变鱼大小
void Round030_04::changeScaleFishs()
{
    m_pFishsActor[0]->playWithSequenceAliasName(TRIGGER);
    m_pFishsActor[0]->setAnimationFinishedPlayDelegate([this]{
        m_pFishsActor[0]->playWithSequenceAliasName(STANDBY,true);
    });
    
    m_pFishsActor[1]->playWithSequenceAliasName(TRIGGER);
    m_pFishsActor[1]->setAnimationFinishedPlayDelegate([this]{
        m_pFishsActor[1]->playWithSequenceAliasName(STANDBY,true);
    });
    
    m_pFishsActor[2]->playWithSequenceAliasName(TRIGGER);
    m_pFishsActor[2]->setAnimationFinishedPlayDelegate([this]{
        m_pFishsActor[2]->playWithSequenceAliasName(STANDBY,true);
    });
}

//改变气泡大小
void Round030_04::changeScaleBubblesSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round030_04::changeScaleBubblesSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pBubbleSp[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.4f, (1.0f-m_InitAniIndex*0.1f)+0.1f),
                                          CallFunc::create([this]{
        m_pBubbleSp[m_InitAniIndex]->setVisible(false);
        m_pHighlightBubbleSp[m_InitAniIndex]->setVisible(true);
    }),
                                          DelayTime::create(0.2f),
                                          CallFunc::create([this]{
        m_pHighlightBubbleSp[m_InitAniIndex]->setVisible(false);
        m_pBubbleSp[m_InitAniIndex]->setVisible(true);
    }),
                                                            ScaleTo::create(0.4f, (1.0f-m_InitAniIndex*0.1f)),
                                                            CallFunc::create([this]{
        m_InitAniIndex++;
    }),NULL));
}

/* 开启触摸 */
void Round030_04::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setLocalZOrder(5);
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pBubbleSp[0]->getContentSize().width, m_pBubbleSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[0]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 0) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBubbleSp[1]->getContentSize().width, m_pBubbleSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[1]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBubbleSp[2]->getContentSize().width, m_pBubbleSp[2]->getContentSize().height);
        if (rect.containsPoint(m_pBubbleSp[2]->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
            }
        }
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_pBubbleSp[index]->getPosition());
            m_pBubbleSp[index]->runAction(FadeOut::create(0.3f));
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_fishsEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(FISH_ZORDER);
                target->setPosition(m_fishsEndPos[index]);
            }), NULL));
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pFishsActor[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pFishsActor[i]);
    } /*for*/
}

//答对后左移鱼
void Round030_04::actionFishs()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pFishsActor[i]->runAction(MoveBy::create(2.0f, Vec2(-1700, 0.0f)));
    }
}

/*
 *  庆祝
 */
void Round030_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round030_04::endRound()
{
    actionFishs();
    scheduleOnce([this](float){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 2.0f, "delaytime");
}
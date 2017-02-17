//
//  Round016_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#include "Round016_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PNG_FILE                    "res/stage016/XQ-GK04-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define CELEBRATE_DELAY                 5.0f
#define ROAD_TAG                        201
#define BOARD_TAG                       202
#define ROAD_PNG_FILE                   "XQ-GK04-BG-diban.png"
#define BOARD_PNG_FILE                  "XQ-GK04-3-IT01-1.png"
#define ANSWER_ZORDER                   3
#define TOPIC_ZORDER                    1
#define HINT_ZORDER                     2
#define TOPIC_POS                       Vec2(1125,799)
#define HINT_POS                        Vec2(1560,799)
#define REPEAT_TAG                      100

/*
 *  constructor
 */
Round016_03::Round016_03()
:m_InitAniIndex(0)
{
    m_answerNumbersPos[0] = Vec2(939, 361);
    m_answerNumbersPos[1] = Vec2(1203, 361);
    m_answerNumbersPos[2] = Vec2(1467, 361);
}

/*
 *  destructor
 */
Round016_03::~Round016_03()
{
    cleanUp();
}

//
void Round016_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    m_pStageScene->removeOBLeaderNode(ROAD_TAG);
}

//
Round016_03* Round016_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round016_03();
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
bool Round016_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pGlassBoardSp = Sprite::createWithSpriteFrameName(BOARD_PNG_FILE);
    m_pGlassBoardSp->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addBelowLeaderNode(m_pGlassBoardSp,Vec2(6242, 148),BOARD_TAG);
    
    m_pRoadSp = Sprite::createWithSpriteFrameName(ROAD_PNG_FILE);
    m_pRoadSp->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addBelowLeaderNode(m_pRoadSp,Vec2(6161, 40),ROAD_TAG);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    m_pTopicImageView = ImageView::create("XQ-GK04-3-TM01-1.png",ui::Widget::TextureResType::PLIST);
    m_pTopicImageView->setPosition(TOPIC_POS);
    this->addChild(m_pTopicImageView, TOPIC_ZORDER);
    
    m_pTargetSp = Sprite::createWithSpriteFrameName("XQ-GK04-3-TM01-3.png");
    m_pTargetSp->setPosition(HINT_POS);
    m_pBN->addChild(m_pTargetSp, HINT_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK04-3-DA01-%ld.png",i+1);
        m_pAnswerNumbers[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswerNumbers[i]->setUserData((void*)i);
        auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEAT_TAG);
        m_pAnswerNumbers[i]->runAction(repeat);
        m_pBN->addChild(m_pAnswerNumbers[i], ANSWER_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswerNumbers[all]->setPosition(m_answerNumbersPos[randIndex]);
        m_answerNumbersEndPos[all] = m_answerNumbersPos[randIndex];
        all++;
    } /*while*/
    
    
    return true;
}

// 进入本回合
void Round016_03::onEnterRound()
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
    
    fadeInAnswers();
    this->schedule(CC_SCHEDULE_SELECTOR(Round016_03::changeScaleAnswersSchedule), 0.85f, -1, 1.1f);
    
}

//渐变显示答案
void Round016_03::fadeInAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswerNumbers[i]->runAction(FadeIn::create(1.0f));
    }
}

//依次放大答案，以提示
void Round016_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round016_03::changeScaleAnswersSchedule));
        changeScaleTarget();
        return;
    }
    m_pAnswerNumbers[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
}

//改变提示问号的大小以提示
void Round016_03::changeScaleTarget()
{
    m_pTargetSp->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),
                                            ScaleTo::create(0.3f, 1.0f),
                                            ScaleTo::create(0.3f, 1.1f),
                                            ScaleTo::create(0.3f, 1.0f),
                                            CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }),NULL));
}

/* 开启触摸 */
void Round016_03::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f, m_pTargetSp->getContentSize().width, m_pTargetSp->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(m_pTargetSp->convertTouchToNodeSpace(touch))) {
            if (index == 1) {
                m_pTargetSp->runAction(FadeOut::create(0.2f));
                target->stopActionByTag(REPEAT_TAG);
                target->setPosition(HINT_POS);
                fadeOutAnswers();                   //隐藏答案

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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswerNumbers[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswerNumbers[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswerNumbers[i]);
    } /*for*/
    
}

//答对后隐藏剩余答案
void Round016_03::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswerNumbers[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

//延伸玻璃桥
void Round016_03::moveGlassBoard(float)
{
    m_pGlassBoardSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(1300, 0.0f)),CallFunc::create([this]{
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
}


/*
 *  庆祝
 */
void Round016_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round016_03::endRound()
{
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round016_03::moveGlassBoard), 0.5f);
}
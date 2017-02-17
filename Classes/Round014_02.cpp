//
//  Round014_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/12.
//
//

#include "Round014_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PNG_FILE                    "res/stage014/XQ-GK02-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define STARBALL_ZORDER                 3
#define ROBOT_FILE                      "XQ-GK02-2-IT01.png"
#define ROBOT_ZORDER                    1
#define ROBOT_POS                       Vec2(1500,513)
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define NUMBER_OF_ANSWER_RIGHT          2                                       // 答题正确数量达到该数字怎本回合结束
#define CELEBRATE_DELAY                 5.0f
#define REPEATACTION_TAG                100
#define TARGET_HINT_FILE                "XQ-GK02-2-IT02.png"
#define TARGET_HINT_ZORDER              2
#define BEGFORE_TAG                     200
#define ROAD_TAG                        201
#define WALL_TAG                        202

/*
 *  constructor
 */
Round014_02::Round014_02()
:m_InitAniIndex(0)
{
    
    m_starBallsPos[0] = Vec2(376, 965);
    m_starBallsPos[1] = Vec2(657, 1193);
    m_starBallsPos[2] = Vec2(852, 865);
    m_starBallsPos[3] = Vec2(1124, 1193);
    
    m_targetsPos[0] = Vec2(1371, 539);
    m_targetsPos[1] = Vec2(1682, 552);
}

/*
 *  destructor
 */
Round014_02::~Round014_02()
{
    cleanUp();
}

//
void Round014_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round014_02* Round014_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round014_02();
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
bool Round014_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBeforeSp = Sprite::createWithSpriteFrameName("XQ-GK02-BG-meng2.png");
    m_pBeforeSp->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addOverLeaderNode(m_pBeforeSp, Vec2(3680+1948,170), BEGFORE_TAG);
    
    m_pRoadSp = Sprite::createWithSpriteFrameName("XQ-GK02-BG-diban.png");
    m_pRoadSp->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addBelowLeaderNode(m_pRoadSp,Vec2(3680+2463, 39),ROAD_TAG);
    
    m_pWallSp = Sprite::createWithSpriteFrameName("XQ-GK02-BG-meng.png");
    m_pWallSp->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addBelowLeaderNode(m_pWallSp,Vec2(3680+1948, 170),WALL_TAG);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    srand((unsigned)time(0));
    m_curSameBallRandNum = rand()%3;
    
    char buffer[64];
    unsigned int number = 0;
    for (unsigned long i = 0; i < 4; i++) {
        if (m_curSameBallRandNum == i || m_curSameBallRandNum == i - 1) {
            sprintf(buffer, "XQ-GK02-2-DA%02d.png",m_curSameBallRandNum+1);
            m_pStarBalls[i] = Sprite::createWithSpriteFrameName(buffer);
            if (m_curSameBallRandNum == i-1) {
                number++;
            }
        }else
        {
            sprintf(buffer, "XQ-GK02-2-DA%02d.png",number+1);
            m_pStarBalls[i] = Sprite::createWithSpriteFrameName(buffer);
            number++;
        }
        m_pStarBalls[i]->setUserData((void*)i);
        RepeatForever* repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEATACTION_TAG);
        m_pStarBalls[i]->runAction(repeat);
        m_pBN->addChild(m_pStarBalls[i], STARBALL_ZORDER);
    }
    
    m_pRobot = Sprite::createWithSpriteFrameName(ROBOT_FILE);
    m_pRobot->setPosition(ROBOT_POS);
    m_pBN->addChild(m_pRobot, ROBOT_ZORDER);
    
    for (unsigned int i = 0; i < 2; i++) {
        m_ptargetHintSp[i] = Sprite::createWithSpriteFrameName(TARGET_HINT_FILE);
        m_ptargetHintSp[i]->setPosition(m_targetsPos[i]);
        m_pBN->addChild(m_ptargetHintSp[i], TARGET_HINT_ZORDER);
        m_ptargetHintSp[i]->setOpacity(0);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int ballIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[ballIndex]) continue;
        selected[ballIndex] = true;
        m_pStarBalls[all]->setPosition(m_starBallsPos[ballIndex]);
        m_starBallsEndPos[all] = m_starBallsPos[ballIndex];
        all++;
    } /*while*/
    
    
    return true;
}

// 进入本回合
void Round014_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round014_02::scaleChangeSchedule), 1.0f, -1, 0);

}

//依次改变星球大小
void Round014_02::scaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_02::scaleChangeSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round014_02::fadeTarget), 0.85f, -1, 0);
        return;
    }
    m_pStarBalls[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.5f, 1.2f),ScaleTo::create(0.5f, 1.0f), NULL));
}

//渐变显隐目标
void Round014_02::fadeTarget(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_02::fadeTarget));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_ptargetHintSp[m_InitAniIndex]->runAction(Sequence::create(FadeIn::create(0.4f),FadeOut::create(0.4f),CallFunc::create([this]{
        m_ptargetHintSp[m_InitAniIndex]->setOpacity(0);
        m_InitAniIndex++;
    }), NULL));
}


/* 开启触摸 */
void Round014_02::openTouch()
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
        bool isCorrect = false;
        int curRectNumber = -1;
        for (unsigned int i = 0; i < 2; i++) {
            Rect rect = Rect(0.0f, 0.0f, m_ptargetHintSp[i]->getContentSize().width, m_ptargetHintSp[i]->getContentSize().height);
            if (rect.containsPoint(m_ptargetHintSp[i]->convertTouchToNodeSpace(touch))) {
                if (!isCorrect)
                {
                    if (m_curSameBallRandNum == index || m_curSameBallRandNum == index-1) {
                        if (m_finishedNumber != i)
                        {
                            curRectNumber = i;
                            isCorrect = true;
                            m_finishedNumber = i;
                        }
                    }
                }
            }
        }
        if (isCorrect) /* 答题正确 */
        {
            target->stopActionByTag(REPEATACTION_TAG);
            target->setPosition(m_targetsPos[curRectNumber]);
            target->runAction(EaseBackIn::create(ScaleTo::create(0.2f, 0.9f)));
            m_curCorrectNumber++;
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
            if (NUMBER_OF_ANSWER_RIGHT == m_curCorrectNumber) /* 全部答对，过关 */
            {
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pStarBalls[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            } else {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
        } else {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_starBallsEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_starBallsEndPos[index]);
            }), NULL));
        }

    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_starBallsEndPos[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pStarBalls[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pStarBalls[i]);
    } /*for*/
    
}

//上移机器人
void Round014_02::upMoveRobot()
{
    m_pRobot->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(0.0f, 1500)),CallFunc::create([this]{
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
    m_pStarBalls[m_curSameBallRandNum++]->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500)));
    m_pStarBalls[m_curSameBallRandNum]->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500)));
}


/*
 *  庆祝
 */
void Round014_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round014_02::endRound()
{
    upMoveRobot();
}
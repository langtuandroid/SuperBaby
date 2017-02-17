//
//  Round001_01.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//

#include "Round001_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define WHEEL_GAF_CONFIG_FILE           "config/stage001/wheel.json"            // 数字转盘
#define WHEEL_GAF_FILE                  "res/stage001/SL-GK01-1-TM01-D+C/SL-GK01-1-TM01-D+C.gaf" // 数字转盘 gaf

#define WHEEL_STANDBY                   "standby"                               // 待机动画
#define WHEEL_TRIGGER                   "trigger"                               // 触发动画
#define WHEEL_POS                       Vec2(800.0f, 1170.0f)                   // 数字转盘位置
#define NUMBER_PLIST_FILE               "res/stage001/SL-GK01-1.plist"          // 数字图片
#define NUMBER_IMAGE_FILE               "res/stage001/SL-GK01-1.png"

#define NUMBER_WHEEL_ZORDER             1                                       // 数字转盘
#define NUMBER_PAD_NODE_ZORDER          2                                       // 小数字面板
#define NUMBER_PAD_POSX                 950.0f                                 //

#define INIT_DOWN_OFFSETY               -1120.0f                                // 下坠动画偏移值
#define INIT_DOWN_TIME                  0.6f                                    // 下坠动画时间
#define INIT_UP_OFFSETY                 236.0f                                  // 上升动画偏移值
#define INIT_UP_TIME                    0.3f                                    // 上升动画时间

#define SCALE_DURATION                  0.3f
#define SCALE_FACTOR                    1.5f

#define NUMBER_ZORDER                   1
#define NUMBER_PAD_ZORDER               2

// 答题正确数量达到该数字怎本回合结束
#define NUMBER_OF_ANSWER_RIGHT          2

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FadeOut_TIME                    2.                                      // 数字淡出时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define PASS_GAF_FILE                   "res/common/yanshi01/yanshi01.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3

Round001_01::Round001_01() {}

Round001_01::~Round001_01()
{
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->removeSpriteFramesFromFile(NUMBER_PLIST_FILE);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    GafAssetCache::getInstance()->removeGafAssetForKey(PASS_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(WHEEL_GAF_FILE);
}

//
Round001_01* Round001_01::createWithRoundIndex(const int roundIndex,
                                               StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round001_01();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene, pos))
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
bool Round001_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    float cx = 1310.0f, cy = Director::getInstance()->getVisibleSize().height + 90.0f;
    float radius = 400.0f, angle = 0.0f;
    
    for (unsigned int i = 0; i < 9; i++)
    {
        angle = M_PI * (8.0 - 1.0f * i) / 8.0f;
        m_numberInitPos[i].x = cx + cos(angle) * radius;
        m_numberInitPos[i].y = cy + sin(angle) * radius;
    } /*for*/
    
    appendAsyncLoadTexture(NUMBER_IMAGE_FILE);
    appendRemovedTexture(NUMBER_IMAGE_FILE);
    appendAsyncLoadGaf(WHEEL_GAF_FILE);
    
    preLoadRes();
    return true;
}

void Round001_01::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // 数字转盘 gaf
    m_pWheelActor = Actor::createWithGafConfigFile(WHEEL_GAF_CONFIG_FILE);
    m_pWheelActor->setPosition(WHEEL_POS);
    m_pWheelActor->playWithSequenceAliasName(WHEEL_STANDBY, true);
    addChild(m_pWheelActor, NUMBER_WHEEL_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(NUMBER_PLIST_FILE);
    m_pNumberBN = SpriteBatchNode::create(NUMBER_IMAGE_FILE);
    addChild(m_pNumberBN, NUMBER_PAD_NODE_ZORDER);
    
    char szBuffer[64];
    // 小数字
    const float scale = 1.2f;
    const float offsetX = 170.0f;
    for (unsigned long i = 0; i < 5; i++)
    {
        sprintf(szBuffer, "SL-GK01-1-DA%02ld.png", i + 1);
        m_pNumberPadSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pNumberBN->addChild(m_pNumberPadSp[i], NUMBER_PAD_ZORDER);
        m_pNumberPadSp[i]->setScale(scale);
        m_pNumberPadSp[i]->setUserData((void*)i);
        m_pNumberPadSp[i]->setVisible(false);
    } /*for*/
    // 随机设定位置
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5)
    {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randomIndex]) continue;
        selected[randomIndex] = true;
        m_pNumberPadSp[all]->setPosition(Vec2(NUMBER_PAD_POSX + randomIndex * offsetX,
                                              -m_pNumberPadSp[all]->getContentSize().height / 2.0f - 36.0f));
        m_pNumberPadSpUp[randomIndex] = m_pNumberPadSp[all];
        m_answersPos[all] = Vec2(NUMBER_PAD_POSX + randomIndex * offsetX,
                                 -m_pNumberPadSp[all]->getContentSize().height / 2.0f - 36.0f+INIT_UP_OFFSETY);
        all++;
    } /*while*/
    
    // 随机设定答案
    all = 0;
    bool selectedAnswer[5] = {false, false, false, false, false};
    while (all < 2)
    {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 5);
        if (selectedAnswer[randomIndex]) continue;
        selectedAnswer[randomIndex] = true;
        m_result[all++] = m_numbers[randomIndex];
    } /*while*/
    
    for (unsigned int i = 0; i < 10; i++)
    {
        sprintf(szBuffer, "SL-GK01-1-TM%02d.png", i + 1);
        if (i < 9) {
            m_pAnswerNumbersSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
            m_pNumberBN->addChild(m_pAnswerNumbersSp[i], NUMBER_ZORDER);
            m_pAnswerNumbersSp[i]->setPosition(Vec2(0.0f, -1000.0f));
        } else /*问号*/ {
            unsigned long result = 0;
            
            result = m_result[0] < m_result[1] ? m_result[0] : m_result[1];
            
            m_pQuestionMark[0] = Sprite::createWithSpriteFrameName(szBuffer);
            m_pQuestionMark[0]->setVisible(false);
            m_pQuestionMark[0]->setUserObject(m_pAnswerNumbersSp[result - 1]);
            m_pAnswerNumbersSp[result - 1]->setVisible(false);
            m_pQuestionMark[0]->setUserData((void*)result);
            
            result = m_result[0] > m_result[1] ? m_result[0] : m_result[1];
            m_pQuestionMark[1] = Sprite::createWithSpriteFrameName(szBuffer);
            m_pQuestionMark[1]->setVisible(false);
            m_pQuestionMark[1]->setUserObject(m_pAnswerNumbersSp[result - 1]);
            m_pAnswerNumbersSp[result - 1]->setVisible(false);
            m_pQuestionMark[1]->setUserData((void*)result);
            
            m_pNumberBN->addChild(m_pQuestionMark[0], NUMBER_ZORDER);
            m_pNumberBN->addChild(m_pQuestionMark[1], NUMBER_ZORDER);
        }
    } /*for*/
}

// 进入本回合
void Round001_01::onEnterRound()
{
    if (!UserDefault::getInstance()->getBoolForKey("delaytimeone", false)) {
        UserDefault::getInstance()->setBoolForKey("delaytimeone", true);
        scheduleOnce([this](float){
            // 开始数字下落动画
            m_InitAniIndex = m_downed = 0;
            schedule(CC_SCHEDULE_SELECTOR(Round001_01::numberDonwSchedule), INIT_DOWN_TIME-0.3f, -1, 0.0f);
        }, 5.0f, "delaytime");
    } else {
        // 开始数字下落动画
        m_InitAniIndex = m_downed = 0;
        schedule(CC_SCHEDULE_SELECTOR(Round001_01::numberDonwSchedule), INIT_DOWN_TIME, -1, 0.0f);
    }
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
}

/* 开启触摸 */
void Round001_01::openTouch()
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
            m_dragBegin = touch->getLocation();
            m_pNumberBN->reorderChild(target, NUMBER_PAD_ZORDER + 1);
            target->setScale(1.3f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto dragMove = touch->getLocation();
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 m_movePos = Vec2::ZERO;
        long index = (long)target->getUserData();
        m_movePos.x = m_answersPos[index].x + (dragMove.x - m_dragBegin.x);
        m_movePos.y = m_answersPos[index].y + (dragMove.y - m_dragBegin.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        unsigned long dragNumber = m_numbers[index];
        
        m_pNumberBN->reorderChild(target, NUMBER_PAD_ZORDER);
        target->setScale(1.2f);
        
        Sprite* pSp = nullptr;
        bool isCorrect = false;
        
        Rect rect = Rect(0.0f, 0.0f,
                         m_pQuestionMark[0]->getContentSize().width,
                         m_pQuestionMark[0]->getContentSize().height);
        if (rect.containsPoint(m_pQuestionMark[0]->convertTouchToNodeSpace(touch)) )
        {
            if (!isCorrect && (unsigned long)m_pQuestionMark[0]->getUserData() == dragNumber)
            {
                pSp = m_pQuestionMark[0];
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                         m_pQuestionMark[1]->getContentSize().width,
                         m_pQuestionMark[1]->getContentSize().height);
        if (rect.containsPoint(m_pQuestionMark[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && (unsigned long)m_pQuestionMark[1]->getUserData() == dragNumber)
            {
                pSp = m_pQuestionMark[1];
                isCorrect = true;
            }
        }
        
        if (isCorrect) /* 答题正确 */
        {
            target->setVisible(false);
            m_numberOfdragComplete++;
            pSp->setVisible(false);
            auto pNumberSp = static_cast<Sprite* >(pSp->getUserObject());
            pNumberSp->setPosition(pSp->getPosition());
            pNumberSp->setVisible(true);
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();

            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (NUMBER_OF_ANSWER_RIGHT == m_numberOfdragComplete) /* 全部答对，过关 */
            {
                for (unsigned int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pNumberPadSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                m_pWheelActor->playWithSequenceAliasName(WHEEL_TRIGGER, false);
            } else {
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
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_answersPos[index]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        m_pNumberBN->reorderChild(target, NUMBER_PAD_ZORDER);
        long index = (long)target->getUserData();
        target->setScale(1.2f);
        target->setPosition(m_answersPos[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pNumberPadSp[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pNumberPadSp[i]);
    } /*for*/
}

/* 播放过关引导 */
void Round001_01::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->getRoundUINode()->addChild(colorLayer,PASS_GAF_ZORDER);
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, colorLayer);
    dispatcher->setPriority(listenerDrag, -130);
    Actor* passActor = Actor::createWithGafFile(PASS_GAF_FILE,false);
    passActor->setPosAtNode(Vec2(300, 500));
    passActor->setPosition(Vec2(570, 300));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]
    {
        colorLayer->removeFromParent();
        passActor->setVisible(false);
    });
}

//
void Round001_01::numberUpSchedule(float dt)
{
    if (m_InitAniIndex == 5)
    {
        unschedule(CC_SCHEDULE_SELECTOR(Round001_01::numberUpSchedule));
        if (!UserDefault::getInstance()->getBoolForKey("slideone", false))
        {
            scheduleOnce([this](float)
            {
                playPassActor();
            }, 4.0f, "slideDelayTime");
            UserDefault::getInstance()->setBoolForKey("slideone", true);
            UserDefault::getInstance()->flush();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pNumberPadSpUp[m_InitAniIndex]->setVisible(true);
    m_pNumberPadSpUp[m_InitAniIndex]->runAction(EaseBackOut::create(move));
    m_InitAniIndex++;
}

//
void Round001_01::numberDonwSchedule(float dt)
{
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    if (m_downed >= 7)
    {
        if (m_downed == 8)
        {
            auto startScale = [this]
            {
                auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                                 ScaleTo::create(SCALE_DURATION, 1.0f),
                                                 ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                                 ScaleTo::create(SCALE_DURATION, 1.0f),
                                                 CallFunc::create(CC_CALLBACK_0(Round001_01::scaleComplete, this)), nullptr);
                m_pQuestionMark[0]->runAction(scaleSeq);
            };
            // 最后一个下落的精灵
            auto callFunc = CallFunc::create(startScale);
            auto action = Sequence::create(EaseBackOut::create(move), callFunc, nullptr);
            auto num = (long)m_pQuestionMark[1]->getUserData();
            m_pQuestionMark[1]->setVisible(true);
            m_pQuestionMark[1]->setPosition(m_numberInitPos[num - 1]);
            m_pQuestionMark[1]->runAction(action);
            m_downed++;
            unschedule(CC_SCHEDULE_SELECTOR(Round001_01::numberDonwSchedule));
            
            // 开始上升动画调度
            m_InitAniIndex = 0;
            schedule(CC_SCHEDULE_SELECTOR(Round001_01::numberUpSchedule), INIT_UP_TIME, -1, 0.0f);
        } else {
            auto num = (long)m_pQuestionMark[0]->getUserData();
            m_pQuestionMark[0]->setVisible(true);
            m_pQuestionMark[0]->setPosition(m_numberInitPos[num - 1]);
            m_pQuestionMark[0]->runAction(EaseBackOut::create(move));
            m_downed++;
        }
    } else {
        do
        {
            if (!m_pAnswerNumbersSp[m_InitAniIndex]->isVisible())
            {
                m_InitAniIndex++;
                continue;
            }
            break;
        } while (true);
        m_pAnswerNumbersSp[m_InitAniIndex]->setPosition(m_numberInitPos[m_InitAniIndex]);
        m_pAnswerNumbersSp[m_InitAniIndex]->runAction(EaseBackOut::create(move));
        m_InitAniIndex++;
        m_downed++;
    }
}

//
void Round001_01::scaleComplete()
{
    auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, 1.0f),
                                     ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f)
                                     , nullptr);
    m_pQuestionMark[1]->runAction(scaleSeq);
}

/*
 *  庆祝 
 */
void Round001_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*
 *  淡出所有数字
 */
void Round001_01::fadeOutNumber()
{
    for (auto &node : m_pNumberBN->getChildren())
    {
        node->setOpacity(255);
        node->runAction(FadeOut::create(FadeOut_TIME));
    } /*for*/
}

/*  回合结束回调 */
void Round001_01::endRound()
{
    fadeOutNumber();
    if (m_completeCB) {
        m_completeCB();
    }
}


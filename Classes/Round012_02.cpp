//
//  Round012_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round012_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                 6.0f                                        // 庆祝延时
#define BN_PNG_FILE                     "res/stage012/SL-GK12-IT.png"               // BN的png文件
#define BN_ZORDER                       3                                           // BN的z值
#define STANDBY_ONE                     "standbyone"                                // 待机1动画
#define STANDBY_TWO                     "standbytwo"                                // 待机2动画
#define TRIGGER_ONE                     "triggerone"                                // 触发1动画
#define TRIGGER_TWO                     "triggertwo"                                // 触发2动画
#define BASKET_IMAGE                    "res/stage012/SL-GK012-2-IT01.png"          // 吊篮
#define FROG_GAF_CONFIG_FILE            "config/stage012/frog.json"                 //青蛙gaf配置文件
#define FROG_GAF_POS                    Vec2(100,1500)                               //青蛙gaf坐标
#define FROG_GAF_ZORDRE                 2                                           //青蛙gaf的z值
#define FADE_IN_TIME                    1.0f                                        //渐变出现时间
#define MOVE_TIME                       0.5f                                        //蔬菜和水果移动时间
#define MOVE_DOWN_OFFSETY               -50.0f                                       //蔬菜和水果下移高度
#define MOVE_UP_OFFSETY                 50.0f                                       //蔬菜和水果上移高度
#define MOV_TIME                        0.2f                                         //答错回原位置移动时间
#define FADE_OUT_TIME                   0.5f                                         //到达目标区域渐变消失时间
#define ACTOR_MOV_TIME                 3.0f                                         //主角移动时间
#define FOOD_FADE_TIME                 1.0f                                         //剩余食物渐隐时间
#define FOOD_UP_OFFSETY                200                                          //剩余食物上升距离

/*
 *
  SL-GK12-2-TM01-D1		闭嘴待机
 SL-GK12-2-TM01-D2		张嘴待机
 SL-GK12-2-TM01-C1		吃东西
 SL-GK12-2-TM01-C2		跳下水
 */

/*
 *  constructor
 */
Round012_02::Round012_02() {}

/*
 *  destructor
 */
Round012_02::~Round012_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round012_02* Round012_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round012_02();
    if (pRound && pRound->initWithRoundIndex(roundIndex,pStageScene,pos))
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
bool Round012_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendRemovedTexture(BASKET_IMAGE);
    appendAsyncLoadTexture(BASKET_IMAGE);
    appendAsyncLoadGaf("res/stage012/SL-GK12-2-TM01-D+C/SL-GK12-2-TM01-D+C.gaf");
    
    preLoadRes();
    return true;
}

//
void Round012_02::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    //青蛙actor加载
    m_pFrogActor = Actor::createWithGafConfigFile(FROG_GAF_CONFIG_FILE);
    m_pFrogActor->setPosAtNode(FROG_GAF_POS);
    m_pFrogActor->playWithSequenceAliasName(STANDBY_ONE,true);
    addChild(m_pFrogActor, FROG_GAF_ZORDRE);
    
    // 纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    // 吊篮
    m_basketSp = Sprite::create(BASKET_IMAGE);
    m_basketSp->setPosition(Vec2(1180.0f, 1440.0f));
    addChild(m_basketSp, 1);
    
    float px = 1000.0f, py = 250.0f, radius = 800.0f, startarc = 26.0f, stepArc = 27.0f;
    // 食物
    char szBuffer[32];
    for (unsigned long i = 0; i < 5; i++)
    {
        sprintf(szBuffer, "SL-GK012-2-DA%02ld.png", i + 1);
        m_foodSps[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_foodSps[i]->setUserData((void*)i);
        m_foodSps[i]->setOpacity(0);
        m_pBN->addChild(m_foodSps[i]);
    } /*for*/
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5)
    {
        unsigned int foodIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[foodIndex]) continue;
        selected[foodIndex] = true;
        m_foodSps[foodIndex]->setPosition(Vec2(px + radius * cos((startarc + stepArc * all) * M_PI / 180.0f),
                                               py + radius * sin((startarc + stepArc * all) * M_PI / 180.0f)));
        m_originalPoints[foodIndex] = Vec2(px + radius * cos((startarc + stepArc * all) * M_PI / 180.0f),
                                           py + radius * sin((startarc + stepArc * all) * M_PI / 180.0f));
        all++;
    } /*while*/
}

//开始回合
void Round012_02::onEnterRound()
{
    this->schedule(CC_SCHEDULE_SELECTOR(Round012_02::fruitAndGreensFadeInSchedule), FADE_IN_TIME-0.5f, -1, 0);
}

//回合开始，水果蔬菜依次出现
void Round012_02::fruitAndGreensFadeInSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_02::fruitAndGreensFadeInSchedule));
        m_InitAniIndex = 0;    //重置为0
        this->schedule(CC_SCHEDULE_SELECTOR(Round012_02::fruitAndGreensMoveSchedule), MOVE_TIME, -1, 0);
        return;
    }
    
    m_foodSps[m_InitAniIndex++]->runAction(FadeIn::create(FADE_IN_TIME));
}

//回合开始，水果蔬菜依次移动
void Round012_02::fruitAndGreensMoveSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_02::fruitAndGreensMoveSchedule));
        openTouch();
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
            auto dispatcher = Director::getInstance()->getEventDispatcher();
            dispatcher->removeCustomEventListeners("endRound");
            endRound();
        });
        return;
    }
    m_foodSps[m_InitAniIndex++]->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(MOVE_TIME, Vec2(0.0f, MOVE_DOWN_OFFSETY))),MoveBy::create(MOVE_TIME, Vec2(0.0f, MOVE_UP_OFFSETY)), NULL));
}

//答对题改变青蛙actor,吃东西
void Round012_02::changeActorEatFood()
{
    m_pFrogActor->playWithSequenceAliasName(TRIGGER_ONE);
    m_pFrogActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pFrogActor->playWithSequenceAliasName(STANDBY_ONE,true);
    });
}

//当食物移动到目标区域，改变青蛙动作，张嘴
void Round012_02::changeActorOpenMouth()
{
    m_pFrogActor->playWithSequenceAliasName(STANDBY_TWO);
    m_pFrogActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pFrogActor->stop();
    });
}

//当食物移动离开目标区域，改变青蛙动作，闭嘴
void Round012_02::changeActorCloseMouth()
{
    m_pFrogActor->playWithSequenceAliasName(STANDBY_ONE,true);
}

//全部答对，青蛙跳水
void Round012_02::changeActorDive()
{
    m_pFrogActor->playWithSequenceAliasName(TRIGGER_TWO);
    m_pFrogActor->setAnimationFinishedPlayDelegate([this]
    {
        actorMovBridge();       //主角上桥
    });
}

//全部答对后，剩余食物上升渐隐消失
void Round012_02::foodUpFadeOutSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_02::foodUpFadeOutSchedule));
        changeActorDive();              //青蛙跳水
        return;
    }
    if (m_InitAniIndex == 0) {
        m_foodSps[2]->runAction(Spawn::create(MoveBy::create(FOOD_FADE_TIME, Vec2(0.0f, FOOD_UP_OFFSETY)),FadeOut::create(FOOD_FADE_TIME-0.5f), NULL));
    }else
    {
        m_foodSps[4]->runAction(Spawn::create(MoveBy::create(FOOD_FADE_TIME, Vec2(0.0f, FOOD_UP_OFFSETY)),FadeOut::create(FOOD_FADE_TIME-0.5f), NULL));
    }
    m_InitAniIndex++;
}

//主角行走到桥
void Round012_02::actorMovBridge()
{
    m_pStageScene->moveLeadingActor(ACTOR_MOV_TIME, Vec2(800, 0), true);
}

// 庆祝
void Round012_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

/* 开启触摸 */
void Round012_02::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        auto rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        if (rect.containsPoint(touchPoint))
        {
            long index = (long)target->getUserData();
            m_originalPoint = m_originalPoints[index];
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            if (clicknumber == 0 || clicknumber == 1 || clicknumber == 3) {
                changeActorOpenMouth();             //触摸到蔬菜，青蛙张嘴
            }
            return true;
        }
        
        return false;
    };
    listenerRight->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        auto touchTargetPoint = this->convertTouchToNodeSpace(touch);
        int curAreaNumber = -1;   //当前区域号
        if (m_rect.containsPoint(touchTargetPoint))
        {
            if (!isCorrect) {
                if (clicknumber == 0 || clicknumber == 1 || clicknumber == 3) {
                    isCorrect = true;
                    m_correctNumber++;
                    curAreaNumber = 0;
                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                    dispatcher->removeEventListenersForTarget(target);
                }
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            target->setPosition(m_centerRectsPos);
            target->runAction(FadeOut::create(FADE_OUT_TIME));      //符号渐变隐藏
            changeActorEatFood();                                   //青蛙吃东西
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1)
            {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            if (m_correctNumber == 3) {   //全部答对
                m_InitAniIndex = 0;                                     //重置为0
                this->schedule(CC_SCHEDULE_SELECTOR(Round012_02::foodUpFadeOutSchedule), FOOD_FADE_TIME-0.5f, -1, 0);     //剩余食物上升渐隐
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_foodSps[i]);
                } /*for*/
            }else
            {
                if (m_celebrateCB) {            //答对庆祝
                    m_celebrateCB(1);
                }
            }
        }else
        {
            if (clicknumber == 0 || clicknumber == 1 || clicknumber == 3) {
                changeActorCloseMouth();            //触摸完毕，不在正确区域，青蛙闭嘴
            }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_foodSps[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_foodSps[i]);
    }
}

/*  回合结束回调 */
void Round012_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


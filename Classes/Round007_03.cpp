//
//  Round007_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round007_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage007/SL-GK07-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define INIT_UP_TIME                   0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY                600.0f                                  // 上升动画偏移值
#define CLOCK_ACTOR_POS                Vec2(465.0f,1510.0f)                   //时钟actor坐标
#define CLOCK_GAF_CONFIG_FILE          "config/stage007/clock.json"            //时钟actor配置文件
#define CLOCK_GAF_ZORDER               1                                       //时钟actor的z值
#define TIME_ANSWER_ZORDER             2                                       //答案时间z值
#define CORRECT_EFFECT_GAF_FILE        "res/stage007/SL-GK07-3-TX01/SL-GK07-3-TX01.gaf"  //答对出现的解除魔法效果gaf
#define CORRECT_EFFECT_GAF_ZORDER      3                                       //答对时解除魔法效果的gaf
#define CORRECT_EFFECT_GAF_POS         Vec2(450.0f,1400.0f)                       //答对时解除魔法效果的gaf位置
#define INIT_DOWN_TIME                  0.8f                                    // 下降动画时间
#define INIT_DOWN_OFFSETY               -600.0f                                  // 下降动画偏移值
#define WATERFALL_TAG                  200
#define WATERFALL_GAF_FILE             "res/stage007/SL-GK07pubu/SL-GK07pubu.gaf"

/*
 *  constructor
 */
Round007_03::Round007_03():
m_InitAniIndex(0)
{}

/*
 *  destructor
 */
Round007_03::~Round007_03() {
    cleanUp();
}

/*
 *  清理
 */

void Round007_03::cleanUp()
{
    m_pStageScene->removeOBLeaderNode(WATERFALL_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(WATERFALL_GAF_FILE);
}

//
Round007_03* Round007_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round007_03();
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
bool Round007_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //答案位置
    m_answerTimePos[0] = Vec2(983, -250);
    m_answerTimePos[1] = Vec2(1400, -250);
    m_answerTimePos[2] = Vec2(1817, -250);
    
    appendAsyncLoadGaf("res/stage007/SL-GK07-3-MS01-D+C/SL-GK07-3-MS01-D+C.gaf");
    appendAsyncLoadGaf(CORRECT_EFFECT_GAF_FILE);
    preLoadRes();
    return true;
}

void Round007_03::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    // 加载时钟actor
    m_pClockActor = Actor::createWithGafConfigFile(CLOCK_GAF_CONFIG_FILE);
    m_pClockActor->setPosition(CLOCK_ACTOR_POS);
    m_pClockActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pClockActor, CLOCK_GAF_ZORDER);
    
    //加载BN
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载答案
    char daBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(daBuffer, "SL-GK07-3-DA%02ld.png",i+1);
        m_pAnswerTimes[i] = Sprite::createWithSpriteFrameName(daBuffer);
        m_pAnswerTimes[i]->setUserData((void*)i);
        m_pBN->addChild(m_pAnswerTimes[i], TIME_ANSWER_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pAnswerTimes[all]->setPosition(m_answerTimePos[Index]);
        all++;
    } /*while*/
}

//开始回合
void Round007_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_03::answersTimeUpSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
}

//回合开场动画，答案时间上升动画调度
void Round007_03::answersTimeUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_03::answersTimeUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pAnswerTimes[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//答对时加载解除魔法效果gaf
void Round007_03::addEffectActor()
{
    m_pFreeEffectActor = Actor::createWithGafFile(CORRECT_EFFECT_GAF_FILE,false);
    m_pFreeEffectActor->setPosition(CORRECT_EFFECT_GAF_POS);
    addChild(m_pFreeEffectActor, CORRECT_EFFECT_GAF_ZORDER);
    m_pFreeEffectActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pFreeEffectActor->setVisible(false);
    });
}

//答对时改变时钟的动作
void Round007_03::changeClockActor()
{
    m_pClockActor->playWithSequenceAliasName(TRIGGER);
}

//庆祝
void Round007_03::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

//
void Round007_03::answersTimeDownSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_03::answersTimeDownSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    m_pAnswerTimes[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

/* 开启触摸 */
void Round007_03::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            
            target->setScale(1.1f);
            return true;
        }
        
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        if (rect.containsPoint(touchPoint)) {
            if (clicknumber == 2) {
                changeClockActor();                 //改变时钟动作
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
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
                this->celebrate(CELEBRATE_DELAY);   //答对庆祝
                addEffectActor();                   //添加解除魔法效果gaf
                for (unsigned int i = 0; i < 3; i++) {   //移除监听
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswerTimes[i]);
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
                if (m_anwerErrorCB) {   //答错
                    m_anwerErrorCB();
                }
            }
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pAnswerTimes[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pAnswerTimes[i]);
    }
}

/*  回合结束回调 */
void Round007_03::endRound()
{
    m_InitAniIndex = 0;                 //重置为0，第一个数字
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_03::answersTimeDownSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
}

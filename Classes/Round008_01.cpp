//
//  Round008_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#include "Round008_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage008/SL-GK08-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage008/SL-GK08-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define FadeIn_TIME                    1                                         //钱币淡入时间
#define FadeOut_TIME                   1                                         //钱币淡出时间
#define CHICKEN_GAF_STANDBY_FILE       "res/stage008/SL-GK08-1MS01-D/SL-GK08-1MS01-D.gaf" //待机鸡的gaf文件
#define CHICKEN_GAF_TRIGGER_FILE       "res/stage008/SL-GK08-1MS01-C/SL-GK08-1MS01-C.gaf" //触发鸡的gaf文件
#define CHICK_GAF_STANDBY_ZORDER       1                                         //待机状态鸡的z值
#define CHICK_GAF_TRIGGER_ZORDER       2                                         //触发状态鸡的z值
#define STANDBY                        "standby"                                 // 待机动画
#define TRIGGER                        "trigger"                                 // 触发动画
#define CHICK_GAF_STANDBY_POS          Vec2(955.0f,850.0f)                      //待机状态鸡的gaf坐标
#define CHICK_GAF_TRIGGER_POS          Vec2(412.0f,1200.0f)                      //触发状态鸡的gaf坐标
#define MONEY_ZORDER                   1                                         //答案钱币的z值
#define INIT_FADE_TIME                 0.2f                                     // 渐变调用时间
/*
 *  constructor
 */
Round008_01::Round008_01()
:m_InitAniIndex(0)
{
}

/*
 *  destructor
 */
Round008_01::~Round008_01() {
    cleanUp();
}

/*
 *  清理
 */

void Round008_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round008_01* Round008_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round008_01();
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
bool Round008_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //设置答案钱币坐标
    m_moneysPos[0] = Vec2(771, 986);
    m_moneysPos[1] = Vec2(1156, 944);
    m_moneysPos[2] = Vec2(1506, 944);
    
    appendAsyncLoadTexture(BN_PNG_FILE);
    appendAsyncLoadGaf(CHICKEN_GAF_STANDBY_FILE);
    appendAsyncLoadGaf(CHICKEN_GAF_TRIGGER_FILE);
    preLoadRes();
    return true;
}

//
void Round008_01::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    
    // 纹理加载
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //添加待机状态母鸡gaf
    m_pStandbyChickenActor = Actor::createWithGafFile(CHICKEN_GAF_STANDBY_FILE);
    m_pStandbyChickenActor->setPosition(CHICK_GAF_STANDBY_POS);
    addChild(m_pStandbyChickenActor, CHICK_GAF_STANDBY_ZORDER);
    
    //添加钱币答案
    char qbBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(qbBuffer, "SL-GK08-1-DA%02ld.png",i+2);
        m_pMoneys[i] = Sprite::createWithSpriteFrameName(qbBuffer);
        m_pMoneys[i]->setUserData((void*)i);
        m_pBN->addChild(m_pMoneys[i], MONEY_ZORDER);
        m_pMoneys[i]->setOpacity(0);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pMoneys[all]->setPosition(m_moneysPos[Index]);
        all++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
}

//开始回合
void Round008_01::onEnterRound()
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
    //渐变出现钱币答案
    this->schedule(CC_SCHEDULE_SELECTOR(Round008_01::fadeInMoneysSchedule), INIT_FADE_TIME, -1, 0.0f);
    
}

//开始回合，渐变出现钱币答案
void Round008_01::fadeInMoneysSchedule(float dt)
{
    if (m_InitAniIndex >=3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_01::fadeInMoneysSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round008_01::changeScaleMoneysSchedule), 0.6f, -1, 0.0f);
        return;
    }
    
    m_pMoneys[m_InitAniIndex]->runAction(FadeIn::create(FadeIn_TIME));
    m_InitAniIndex++;
}

//改变答案尺寸以突出
void Round008_01::changeScaleMoneysSchedule(float)
{
    if (m_InitAniIndex >=3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_01::changeScaleMoneysSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pMoneys[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//答对改变鸡的动作
void Round008_01::changeChickenActor()
{
    //添加触发状态母鸡gaf
    m_pTriggerChickenActor = Actor::createWithGafFile(CHICKEN_GAF_TRIGGER_FILE,false);
    m_pTriggerChickenActor->setPosition(CHICK_GAF_TRIGGER_POS);
    addChild(m_pTriggerChickenActor, CHICK_GAF_TRIGGER_ZORDER);
    
    scheduleOnce([this](float){
        //隐藏待机状态gaf
        m_pStandbyChickenActor->setVisible(false);
    }, 0.1f, "hide");
}

//庆祝
void Round008_01::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

//离开回合，渐变隐藏钱币答案
void Round008_01::fadeOutMoneysSchedule(float dt)
{
    if (m_InitAniIndex >=3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_01::fadeOutMoneysSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pMoneys[m_InitAniIndex]->runAction(FadeOut::create(FadeOut_TIME));
    m_InitAniIndex++;
}

/* 开启触摸 */
void Round008_01::openTouch()
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
        if (rect.containsPoint(touchPoint))
        {
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
            if (clicknumber == 0) {
                changeChickenActor();                //改变鸡的动作
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                this->celebrate(CELEBRATE_DELAY);   //答对庆祝
                for (unsigned int i = 0; i < 3; i++) {   //移除监听
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pMoneys[i]);
                }
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {   //答错
                    m_anwerErrorCB();
                }
            }
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pMoneys[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pMoneys[i]);
    }
}

/*  回合结束回调 */
void Round008_01::endRound()
{
    m_InitAniIndex = 0;
    //渐变隐藏钱币答案
    this->schedule(CC_SCHEDULE_SELECTOR(Round008_01::fadeOutMoneysSchedule), INIT_FADE_TIME, -1, 0.0f);
}


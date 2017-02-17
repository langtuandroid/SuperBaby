//
//  Round006_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/2.
//
//

#include "Round006_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define CELEBRATE_DELAY                 5.0f                                     // 庆祝延时
#define INIT_UP_TIME                    0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY                 250.0f                                  // 上升动画偏移值
#define INIT_DOWN_TIME                   0.5f                                    // 下降动画时间
#define INIT_DOWN_OFFSETY               -270.0f                                  // 下降动画偏移值
#define NUMBER_ZORDER                    2                                       //数字Z值
#define BUTTERFLY_GAF_CONFIG_FILE        "config/stage006/butterfly.json"        //蝴蝶gaf
#define BUTTERFLY_POS                    Vec2(137,1433)                             //蝴蝶位置
#define BUTTERFLY_ZORDER                 1                                        //蝴蝶Z值
#define STANDBY_ONE                      "standbyone"                               // 五只待机动画
#define STANDBY_TWO                      "standbytwo"                               // 八只待机动画
#define TRIGGER                          "trigger"                               // 触发动画

/*
 *  constructor
 */
Round006_02::Round006_02():m_InitAniIndex(0)
{}

/*
 *  destructor
 */
Round006_02::~Round006_02() {
    cleanUp();
}

/*
 *  清理
 */
void Round006_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round006_02* Round006_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round006_02();
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
bool Round006_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage006/SL-GK06-2-TM-D+D2+C/SL-GK06-2-TM-D+D2+C.gaf");
    preLoadRes();
    return true;
}

//
void Round006_02::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    char nsBuffer[64];
    float nx = 800.0f; float offsetX = 250.0f; float ny = -100.0f;
    //被选择的数字
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(nsBuffer, "SZ3-%02ld.png",i+6);
        m_pChooseNumber[i] = Sprite::createWithSpriteFrameName(nsBuffer);
        m_pChooseNumber[i]->setUserData((void*)(i+6));
        addChild(m_pChooseNumber[i], NUMBER_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pChooseNumber[all]->setPosition(Vec2(nx+offsetX*Index, ny));
        all++;
    } /*while*/
    
    //加载蝴蝶
    m_pButterflyActor = Actor::createWithGafConfigFile(BUTTERFLY_GAF_CONFIG_FILE);
    m_pButterflyActor->setPosition(BUTTERFLY_POS);
    m_pButterflyActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pButterflyActor->playWithSequenceAliasName(STANDBY_TWO,true); //增加3只蝴蝶后切换为8只待机
        this->schedule(CC_SCHEDULE_SELECTOR(Round006_02::numberUpSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
    });
    addChild(m_pButterflyActor,BUTTERFLY_ZORDER);
}

//开始回合
void Round006_02::onEnterRound()
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
    m_pButterflyActor->playWithSequenceAliasName(STANDBY_ONE);
}

/* 开启触摸 */
void Round006_02::openTouch()
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
            if (clicknumber == 8) { //答对问题
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
                
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pChooseNumber[i]);
                } /*for*/
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
        
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pChooseNumber[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pChooseNumber[i]);
    }
}

/*
 *  回合开场动画，小数字上升动画调度
 */
void Round006_02::numberUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round006_02::numberUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pChooseNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//庆祝
void Round006_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

//蝴蝶飞走
void Round006_02::changeButterflyMove()
{
    m_pButterflyActor->playWithSequenceAliasName(TRIGGER);
}

//离场数字下降
void Round006_02::numberDownSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round006_02::numberDownSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    m_pChooseNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

/*  回合结束回调 */
void Round006_02::endRound()
{
    m_InitAniIndex = 0;                 //重置为0，第一个数字
    this->schedule(CC_SCHEDULE_SELECTOR(Round006_02::numberDownSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
    changeButterflyMove();                 //蝴蝶飞走
}

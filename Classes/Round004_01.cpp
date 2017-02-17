//
//  Round004_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#include "Round004_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define RABBIT_GAF_CONFIG_FILE           "config/stage004/rabbit.json"             //兔子配置文件
#define RABBIT_STANDBY                   "standby"                               // 待机动画
#define RABBIT_TRIGGER                   "trigger"                               // 触发动画
#define RABBIT_ZORDER                    1                                       //兔子Z值
#define RABBIT_POS                       Vec2(800,850)                           //兔子位置
#define NUMBER_BN_ZORDER                 2                                       //数字批处理节点Z值
#define NUMBER_ZORDER                    1                                       //数字Z值

#define BN_PLIST_FILE                   "res/stage004/SL-GK04-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                     "res/stage004/SL-GK04-IT.png"               // BN的 png 文件
#define CELEBRATE_DELAY                 5.0f                                     // 庆祝延时
#define INIT_UP_TIME                    0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY                 250.0f                                  // 上升动画偏移值
#define INIT_DOWN_TIME                   0.5f                                    // 下降动画时间
#define INIT_DOWN_OFFSETY               -260.0f                                  // 下降动画偏移值
#define FadeOut_TIME                    2                                       //数字淡出时间

/*
 *  constructor
 */
Round004_01::Round004_01()
:m_InitAniIndex(0)
{}

/*
 *  destructor
 */
Round004_01::~Round004_01()
{
    cleanUp();
}

/*
 *  清理
 */

void Round004_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage004/SL-GK04-1-MS01-D+C/SL-GK04-1-MS01-D+C.gaf");
}

//
Round004_01* Round004_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round004_01();
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
bool Round004_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char nsBuffer[64];
    float nx = 800.0f; float offsetX = 250.0f; float ny = -110.0f;
    //被选择的数字
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(nsBuffer, "SZ3-%02ld.png",i+2);
        m_pChooseNumber[i] = Sprite::createWithSpriteFrameName(nsBuffer);
        m_pChooseNumber[i]->setUserData((void*)(i+2));
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
    
    //兔子
    m_pRabbitNpc = Actor::createWithGafConfigFile(RABBIT_GAF_CONFIG_FILE);
    m_pRabbitNpc->setPosition(RABBIT_POS);
    addChild(m_pRabbitNpc, RABBIT_ZORDER);
    m_pRabbitNpc->playWithSequenceAliasName(RABBIT_TRIGGER,true);
    
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

//开始回合
void Round004_01::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round004_01::numberUpSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
}

/* 开启触摸 */
void Round004_01::openTouch()
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
            if (clicknumber == 3) { //答对问题
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
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
                //答错时主角说话
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
void Round004_01::numberUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round004_01::numberUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pChooseNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

/*
 *  选择正确时改变npc动作
 */
void Round004_01::changeNpcAni()
{
    m_pRabbitNpc->playWithSequenceAliasName(RABBIT_STANDBY,true);
}

//庆祝
void Round004_01::celebrate(const float celebrateDelay) {
    changeNpcAni(); //改变兔子npc动作
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

//离场数字下降
void Round004_01::numberDownSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round004_01::numberDownSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    m_pChooseNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

/*  回合结束回调 */
void Round004_01::endRound()
{
    m_InitAniIndex = 0;                 //重置为0，第一个数字
    this->schedule(CC_SCHEDULE_SELECTOR(Round004_01::numberDownSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
}


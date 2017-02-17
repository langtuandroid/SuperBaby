//
//  Round004_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#include "Round004_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define FROG_GAF_CONFIG_FILE           "config/stage004/frog.json"             //青蛙配置文件
#define FROG_STANDBY                   "standby"                               // 待机动画
#define FROG_TRIGGER                   "trigger"                               // 触发动画
#define FROG_POS                       Vec2(1000,800)                           //青蛙gaf的位置
#define FROG_ZORDER                    1                                       //青蛙Z值
#define FadeIn_TIME                        1                                             //乐器淡入时间
#define FadeOut_TIME                       1                                             //乐器淡出时间
#define CELEBRATE_DELAY                    5.0f                                          // 庆祝延时
#define INSTRUMENT_BN_ZORDER               1                                               //乐器批处理节点Z值
#define INSTRUMENT_ZORDER                  1                                              //单个乐器Z值

/*
 *  constructor
 */
Round004_04::Round004_04()
{
}

/*
 *  destructor
 */
Round004_04::~Round004_04() {
    cleanUp();
}

/*
 *  清理
 */

void Round004_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage004/SL-GK04-3-MS01-D+C/SL-GK04-3-MS01-D+C.gaf");
}

//
Round004_04* Round004_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round004_04();
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
bool Round004_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //加载青蛙gaf
    m_pFrog = Actor::createWithGafConfigFile(FROG_GAF_CONFIG_FILE);
    m_pFrog->setPosition(FROG_POS);
    addChild(m_pFrog, FROG_ZORDER);
    m_pFrog->playWithSequenceAliasName(FROG_TRIGGER,true);
    
    char yqBuffer[64];
    float yqX = 850.0f; float offsetX = 400.0f; float yqY = 300.0f;
    //被选择的数字
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(yqBuffer, "SL-GK04-3-DA%02ld.png",i+1);
        m_PINstruments[i] = Sprite::createWithSpriteFrameName(yqBuffer);
        m_PINstruments[i]->setUserData((void*)(i+1));
        m_PINstruments[i]->setOpacity(0);
        this->addChild(m_PINstruments[i], INSTRUMENT_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        if (all == 2) {
            m_PINstruments[all]->setPosition(Vec2(yqX+offsetX*Index, yqY+50));
        }else
        {
            m_PINstruments[all]->setPosition(Vec2(yqX+offsetX*Index, yqY));
        }
        all++;
    } /*while*/
    
    return true;
}

/* 开启触摸 */
void Round004_04::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
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
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        if (rect.containsPoint(touchPoint)) {
            if (clicknumber == 1) { //答对问题
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
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
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_PINstruments[i]);
                } /*for*/
            }else //答错
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
            }
        }
        
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_PINstruments[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_PINstruments[i]);
    }
}

//开始回合
void Round004_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    fadeInInstrument();   //渐变显示乐器
}

/*
 * 渐变显示乐器
 */
void Round004_04::fadeInInstrument()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_PINstruments[i]->runAction(FadeIn::create(FadeIn_TIME));
    }
    
}

/*
 * 渐变隐藏乐器
 */
void Round004_04::fadeOutInstrument()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_PINstruments[i]->runAction(FadeOut::create(FadeOut_TIME));
    }
    
}


/*
 * 改变青蛙动作
 */
void Round004_04::changeFrog()
{
    m_pFrog->playWithSequenceAliasName(FROG_STANDBY,true);
}

//庆祝
void Round004_04::celebrate(const float celebrateDelay) {
    changeFrog();       //改变青蛙动作
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/*  回合结束回调 */
void Round004_04::endRound()
{
    fadeOutInstrument();   //渐变隐藏乐器
    if (m_completeCB) {
        m_completeCB();
    }
}


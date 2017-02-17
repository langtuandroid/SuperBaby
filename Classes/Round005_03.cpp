//
//  Round005_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/1.
//
//

#include "Round005_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define SUNFLOWER_BN_ZORDER              1                                     //向日葵和虫BN的Z值
#define SUNFLOWER_PNG_FILE               "SL-GK05-2-TM01.png"                  //向日葵叶子png
#define SUNFLOWER_POS                    Vec2(1200.0f,470.0f)                     //向日葵叶子位置
#define CHANGESCALE_DELAY_TIME           0.2f                                  //虫子改变大小前延时
#define FadeIn_TIME                      2                                   //虫子和叶子淡入时间
#define FadeOut_TIME                     2                                   //虫子和叶子淡出时间
#define CHANGESCALE_TIME                 0.3f                                //虫子改变大小时间
#define SUNFLOWER_WORM_GAF_FILE          "res/stage005/SL-GK05-2-TM01-C/SL-GK05-2-TM01-C.gaf"    //向日葵虫子抖动gaf
#define SUNFLOWER_WORM_GAF_POS           Vec2(600.0f,1220.0f)                 //向日葵虫子抖动gaf的位置
#define SUNFLOWER_WORM_GAF_ZORDER        11                                   //向日葵虫子抖动gaf的z值

/*
 *  constructor
 */
Round005_03::Round005_03()
{}

/*
 *  destructor
 */
Round005_03::~Round005_03() {
    cleanUp();
}

/*
 *  清理
 */
void Round005_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round005_03* Round005_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round005_03();
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
bool Round005_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_wormsPos[0] = Vec2(1000, 290);
    m_wormsPos[1] = Vec2(910, 570);
    m_wormsPos[2] = Vec2(1050, 840);
    m_wormsPos[3] = Vec2(1380, 770);
    m_wormsPos[4] = Vec2(1400, 340);
    
   
    //加载向日葵和虫
    m_pSunflower = Sprite::createWithSpriteFrameName(SUNFLOWER_PNG_FILE);
    m_pSunflower->setPosition(SUNFLOWER_POS);
    this->addChild(m_pSunflower);
    m_pSunflower->setOpacity(0);
    
    char czBuffer[64];
    for (unsigned long i = 0; i < 5; i++) {
        sprintf(czBuffer, "SL-GK05-2-DA%02ld.png",i+1);
        m_pWorms[i] = Sprite::createWithSpriteFrameName(czBuffer);
        m_pWorms[i]->setPosition(m_wormsPos[i]);
        m_pWorms[i]->setUserData((void*)i);
        m_pWorms[i]->setOpacity(0);
        this->addChild(m_pWorms[i]);
    }

    return true;
}

// 开始回合
void Round005_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
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
    fadeInWormsAndLeaf();   //虫子和叶子渐变出现
    
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round005_03::changeScaleWorms), FadeIn_TIME);     //改变虫子大小
}

/*
 * 虫子渐变出现
 */
void Round005_03::fadeInWormsAndLeaf()
{
    for (unsigned int i = 0; i < 5; i++) {    //虫子渐变出现
        m_pWorms[i]->runAction(FadeIn::create(FadeIn_TIME));
    }
    //叶子
    m_pSunflower->runAction(FadeIn::create(FadeIn_TIME));
}

/*
 * 设置虫子逐个放大缩小
 */
void Round005_03::changeScaleWorms(float dt)
{
    int offSetTime = 1.0f;
    for (unsigned int i = 0; i < 5; i++) {
        if (i == 4) {
            m_pWorms[i]->runAction(Sequence::create(DelayTime::create(CHANGESCALE_DELAY_TIME+offSetTime*i),ScaleTo::create(CHANGESCALE_TIME,1.1f),ScaleTo::create(CHANGESCALE_TIME,1.0f),CallFunc::create(CC_CALLBACK_0(Round005_03::changeScaleWormsComplete, this)), NULL));
        }else
        {
            m_pWorms[i]->runAction(Sequence::create(DelayTime::create(CHANGESCALE_DELAY_TIME+offSetTime*i),ScaleTo::create(CHANGESCALE_TIME,1.1f),ScaleTo::create(CHANGESCALE_TIME,1.0f), NULL));
        }
    }
}

/* 开启触摸 */
void Round005_03::openTouch()
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
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
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
                for (unsigned int i = 0; i < 5; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pWorms[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pWorms[0]);   //注册绑定监听器和虫子sprite
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pWorms[i]);
    }
}

/*
 * 虫子改变大小完成
 */
void Round005_03::changeScaleWormsComplete()
{
    m_jumped = true;
    if (m_opentouched) {
        openTouch();
    }
}

//答对题之后添加向日葵抖动gaf
void Round005_03::addSunflowerGaf()
{
    //加载向日葵和叶子抖动的gaf
    m_pSunflowerActor = Actor::createWithGafFile(SUNFLOWER_WORM_GAF_FILE,false);
    m_pSunflowerActor->setPosition(SUNFLOWER_WORM_GAF_POS);
    addChild(m_pSunflowerActor,SUNFLOWER_WORM_GAF_ZORDER);
}

//庆祝
void Round005_03::celebrate(const float celebrateDelay) {
    
    addSunflowerGaf();       //添加向日葵抖动gaf
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round005_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

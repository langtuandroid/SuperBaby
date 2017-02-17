//
//  Round002_02.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round002_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_GAF_BG_FILE       "res/common/guanghuanxiaoguo/guanghuanxiaoguo.gaf"
#define CELEBRATE_GAF_FILE          "res/common/wujiaoxing02/wujiaoxing02.gaf"

// 七彩花
#define ACTOR1_JSON                 "config/stage002/SL-GK02-2-MS01-D+C.json"
#define BN_PNG_FILE                 "res/stage002/SL-GK02-IT.png"                   // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round002_02::Round002_02() {}

/*
 *  destructor
 */
Round002_02::~Round002_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}


//
Round002_02* Round002_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round002_02();
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
bool Round002_02::initWithRoundIndex(const int roundIndex, StageScene *pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage002/SL-GK02-2-MS01-D+C/SL-GK02-2-MS01-D+C.gaf");
    preLoadRes();
    return true;
}

//
void Round002_02::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char szBuffer[32];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK02-2-DA%02ld.png", i + 1);
        m_sps[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_sps[i]->setUserData((void*)i);
        m_pBN->addChild(m_sps[i], 2);
    } /*for*/
    
    Vec2 initPos[3];
    initPos[0] = Vec2(570.0f, 900.0f);
    initPos[1] = Vec2(1080.0f, 890.0f);
    initPos[2] = Vec2(1620.0f, 1080.0f);
    
    srand(unsigned(time(0)));
    bool allplaced[3] = {false, false, false};
    unsigned int placed = 0;
    while (placed < 3)
    {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_sps[randIndex]->setPosition(initPos[placed]);
        m_spScales[placed] = m_sps[randIndex];
        m_originalPoints[randIndex] = initPos[placed];
        placed++;
    } /*while*/
    
    // 七彩花
    m_pFlowerActor = Actor::createWithGafConfigFile(ACTOR1_JSON);
    m_pFlowerActor->setPosition(Vec2(1360.0f, 850.0f));
    addChild(m_pFlowerActor, 1);
    m_pFlowerActor->playWithSequenceAliasName("wait", true);
}

/* 开启触摸 */
void Round002_02::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScaleY(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScaleY(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) {
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
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(5.0f);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_sps[i]);
                } /*for*/
            }else
            {
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
    // 绑定监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_sps[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_sps[1]);
    dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_sps[2]);
}

//
void Round002_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round002_02::changeScaleAnswersSchedule), 0.6f, -1, 0);
}
//提示
void Round002_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round002_02::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_spScales[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//
void Round002_02::celebrate(const float delay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

/*  回合结束回调 */
void Round002_02::endRound()
{
    m_numberOfAlreadyJump = 0;
    // 七彩花跳到路边
    m_pFlowerActor->playWithSequenceAliasName("jump", false);
    m_pFlowerActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pFlowerActor->stopAllActions();
        m_pFlowerActor->playWithSequenceAliasName("wait", true);
    });
    m_pFlowerActor->runAction(MoveBy::create(2.2f, Vec2(0.0f, 120.0f)));
    
    if (m_completeCB) {
        m_completeCB();
    }
}


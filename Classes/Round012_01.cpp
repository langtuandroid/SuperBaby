//
//  Round012_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round012_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                 3.0f                                        // 庆祝延时

#define BN_PLIST_FILE                   "res/stage012/SL-GK12-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                     "res/stage012/SL-GK12-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值

#define STANDBY                          "standby"                                  // 待机动画
#define TRIGGER                          "trigger"                                  // 触发动画

#define SCALE_DURATION                  0.3f
#define SCALE_FACTOR                    1.5f

#define CORRECT_INDEX                   1

/*
 *  constructor
 */
Round012_01::Round012_01(){}
/*
 *  destructor
 */
Round012_01::~Round012_01() {
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round012_01* Round012_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round012_01();
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
bool Round012_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(BN_PNG_FILE);
    preLoadRes();
    return true;
}

//
void Round012_01::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    // 尺
    auto spChi1 = Sprite::createWithSpriteFrameName("SL-GK012-1-TM02.png");
    spChi1->setPosition(Vec2(800.0f, 830.0f));
    m_pBN->addChild(spChi1, 1);
    
    auto spChi2 = Sprite::createWithSpriteFrameName("SL-GK012-1-TM01.png");
    spChi2->setPosition(Vec2(1190.0f, 810.0f));
    m_pBN->addChild(spChi2, 1);
    
    auto spChi3 = Sprite::createWithSpriteFrameName("SL-GK012-1-TM01.png");
    spChi3->setPosition(Vec2(1620.0f, 810.0f));
    m_pBN->addChild(spChi3, 1);
    
    // 长颈鹿
    auto spLu1 = Sprite::createWithSpriteFrameName("SL-GK012-1-MS01.png");
    spLu1->setPosition(Vec2(970.0f, 518.0f));
    m_pBN->addChild(spLu1, 2);
    auto spLu2 = Sprite::createWithSpriteFrameName("SL-GK012-1-MS02.png");
    spLu2->setPosition(Vec2(1368.0f, 646.0f));
    m_pBN->addChild(spLu2, 2);
    auto spLu3 = Sprite::createWithSpriteFrameName("SL-GK012-1-MS03.png");
    spLu3->setPosition(Vec2(1800.0f, 674.0f));
    m_pBN->addChild(spLu3, 2);
    
    // 答案数字
    char szBuffer[32];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SZ1-%02ld.png", i + 1);
        m_pNumberSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pNumberSp[i]->setUserData((void*)i);
        addChild(m_pNumberSp[i], 3);
    } /*for*/
    m_pNumberSp[0]->setPosition(Vec2(800.0f, 860.0f));
    m_pNumberSp[1]->setPosition(Vec2(1180.0f, 670.0f));
    m_pNumberSp[2]->setPosition(Vec2(1640.0f, 780.0f));
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
}

//开始回合
void Round012_01::onEnterRound()
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
    
    m_pNumberSp[0]->runAction(Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f), NULL));
    scheduleOnce([this](float dt) {
        m_pNumberSp[1]->runAction(Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                                   ScaleTo::create(SCALE_DURATION, 1.0f), NULL));
    }, SCALE_DURATION * 2, "number2");
    scheduleOnce([this](float dt) {
        m_pNumberSp[2]->runAction(Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                                   ScaleTo::create(SCALE_DURATION, 1.0f),CallFunc::create([this]{
            m_jumped = true;
            if (m_opentouched) {
                openTouch();
            }
        }), NULL));
    }, SCALE_DURATION * 4, "number3");
}

//庆祝
void Round012_01::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

/* 开启触摸 */
void Round012_01::openTouch()
{
    auto touchEvent = EventListenerTouchOneByOne::create();
    touchEvent->onTouchBegan = [this] (Touch* pTouch, Event* pEvent) ->bool {
        auto target = (Sprite*)pEvent->getCurrentTarget();
        auto touchPoint = target->convertTouchToNodeSpace(pTouch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    touchEvent->onTouchEnded = [this] (Touch* pTouch, Event* pEvent){
        auto target = (Sprite*)pEvent->getCurrentTarget();
        target->setScale(1.0f);
        auto touchPoint = target->convertTouchToNodeSpace(pTouch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            unsigned long numberIndex = (unsigned long)target->getUserData();
            if (numberIndex == CORRECT_INDEX) {
                auto eventDispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    eventDispatcher->removeEventListenersForTarget(m_pNumberSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(pTouch->getLocation());     //播放答对特效
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
                celebrate(CELEBRATE_DELAY);
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
            }
        }
    };
    touchEvent->setSwallowTouches(true);
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent, m_pNumberSp[0]);
    eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent->clone(), m_pNumberSp[1]);
    eventDispatcher->addEventListenerWithSceneGraphPriority(touchEvent->clone(), m_pNumberSp[2]);
}

/*  回合结束回调 */
void Round012_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


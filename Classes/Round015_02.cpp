//
//  Round015_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#include "Round015_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                   "res/stage015/XQ-GK03-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                     "res/stage015/XQ-GK03-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define CELEBRATE_DELAY                 5.0f
#define HINT_ZORDER                     2
#define GRAPH_BOARD_ZORDER              1
#define HINT_PNG_FILE                   "XQ-GK03-2-DAxian.png"
#define NUMBER_OF_ANSWER_RIGHT          2                                       // 答题正确数量达到该数字怎本回合结束

/*
 *  constructor
 */
Round015_02::Round015_02()
:m_InitAniIndex(0)
{
    
    m_graphBoardsPos[0] = Vec2(668, 734);
    m_graphBoardsPos[1] = Vec2(1181, 734);
    m_graphBoardsPos[2] = Vec2(1714, 734);
    
    m_hintsPos[0] = Vec2(750, 681);
    m_hintsPos[1] = Vec2(1263, 681);
    m_hintsPos[2] = Vec2(1800, 681);
}

/*
 *  destructor
 */
Round015_02::~Round015_02()
{
    cleanUp();
}

//
void Round015_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    Director::getInstance()->getTextureCache()->removeTextureForKey(HINT_PNG_FILE);
}

//
Round015_02* Round015_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round015_02();
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
bool Round015_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK03-2-DA%02ld.png",i+1);
        m_pGraphBoards[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pGraphBoards[i]->setUserData((void*)i);
        m_pBN->addChild(m_pGraphBoards[i], GRAPH_BOARD_ZORDER);
        m_pGraphBoards[i]->setScale(0.8f);
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        m_pHintSp[i] = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
        m_pBN->addChild(m_pHintSp[i], HINT_ZORDER);
        m_pHintSp[i]->setScale(0.8f);
        m_pHintSp[i]->setOpacity(0);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int graphIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[graphIndex]) continue;
        selected[graphIndex] = true;
        m_pGraphBoards[all]->setPosition(m_graphBoardsPos[graphIndex]);
        m_pHintSp[all]->setPosition(m_hintsPos[graphIndex]);
        all++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

// 进入本回合
void Round015_02::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round015_02::fadeHintSchedule), 0.85f, -1, 0);
    
}

//渐变提示框
void Round015_02::fadeHintSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round015_02::fadeHintSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pHintSp[m_InitAniIndex]->runAction(Sequence::create(FadeIn::create(0.2f),
                                                          FadeOut::create(0.2f),
                                                          FadeIn::create(0.2f),
                                                          FadeOut::create(0.2f),
                                                          CallFunc::create([this]{
      m_pHintSp[m_InitAniIndex]->setOpacity(0);
      m_InitAniIndex++;
   }), NULL));
}


/* 开启触摸 */
void Round015_02::openTouch()
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
            target->setScale(0.9f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(0.8f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1 || index == 0) {
                m_pHintSp[index]->setOpacity(255);
                m_curCorrectNumber++;
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
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
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
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
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
        }
        if (m_curCorrectNumber == NUMBER_OF_ANSWER_RIGHT) /* 全部答对，过关 */
        {
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            celebrate(CELEBRATE_DELAY);
            for (unsigned int i = 0; i < 3; i++) {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pGraphBoards[i]);
            } /*for*/
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pGraphBoards[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pGraphBoards[i]);
    } /*for*/
    
}

/*
 *  庆祝
 */
void Round015_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round015_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
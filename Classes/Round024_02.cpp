//
//  Round024_02.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/09.
//
//

#include "Round024_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "GafAssetCache.h"
#include "NpcActor.h"
#include "CustomMessage.h"
#include "AppConfig.h"

// 大图
#define STAGE_IMAGE_FILE        "res/stage024/XQ-GK12-IT.png"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define SCALE_DURATION                  0.3f
#define SCALE_FACTOR                    1.5f


/*
 *  constructor
 */
Round024_02::Round024_02() {}

/*
 *  destructor
 */
Round024_02::~Round024_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round024_02* Round024_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round024_02();
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
bool Round024_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    // 大图
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 1);
    
    char szBuffer[64];
    float posx = 332.0f, posy = 912.0f, offsetx = 654.0f;
    for (long i = 0; i < 3; i++)
    {
        sprintf(szBuffer, "XQ-GK12-2-DA%02ld.png", i + 1);
        m_numberSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_numberSp[i]->setUserData((void*)i);
        m_numberSp[i]->setVisible(false);
        m_pSBN->addChild(m_numberSp[i], 2);
    } /*for*/
    
    // 随机设定位置
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3)
    {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randomIndex]) continue;
        selected[randomIndex] = true;
        m_numberSp[all]->setPosition(Vec2(posx + offsetx * randomIndex, posy + 200.0f));
        m_jnumberSp[randomIndex] = m_numberSp[all];
        all++;
    } /*while*/
    
    return true;
}

//
void Round024_02::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });

    m_jnumberSp[0]->setVisible(true);
    m_jnumberSp[0]->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(0.8f, Vec2(0.0f, -200.0f))),
                                               CallFunc::create(CC_CALLBACK_0(Round024_02::downCompleteCB, this)),
                                               NULL));
}

//
void Round024_02::downCompleteCB()
{
    if (++m_numberOfDowned == 3)
    {
        openTouch();
    } else {
        m_jnumberSp[m_numberOfDowned]->setVisible(true);
        m_jnumberSp[m_numberOfDowned]->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(0.8f, Vec2(0.0f, -200.0f))),
                CallFunc::create(CC_CALLBACK_0(Round024_02::downCompleteCB, this)), NULL));
    }
}

/*
 *  开启触摸 
 */
void Round024_02::openTouch()
{
    ///////////////////
    // 事件派发
    auto listenerTouch = EventListenerTouchOneByOne::create();
    listenerTouch->setSwallowTouches(true);
    listenerTouch->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        auto rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerTouch->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        long selNumber = (long)target->getUserData();
        auto rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint))
        {
            if (selNumber == 1) /*答题正确*/
            {
                auto sp = Sprite::createWithSpriteFrameName("XQ-GK12-2-IT02.png");
                sp->setPosition(target->getPosition());
                m_pSBN->addChild(sp, 1);
                sp->runAction(MoveTo::create(1.0f, Vec2(1158.0f, 255.0f)));
                
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++)
                {
                    dispatcher->removeEventListenersForTarget(m_numberSp[i]);
                } /*for*/
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x,
                                                            touch->getLocation().y));
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1)
                {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                playLeadAnsCorEff();
                
                m_pStageScene->getRoundUINode()->stopReadEffect();  // 停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();    // 停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                fadeTopic();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_numberSp[i]);
                } /*for*/
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    listenerTouch->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
    };
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch, m_numberSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerTouch->clone(), m_numberSp[i]);
    } /*for*/
}

void Round024_02::fadeTopic()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_numberSp[i]->runAction(FadeOut::create(0.3f));
    }
}

//
void Round024_02::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    //
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*
 *  回合结束回调
 */
void Round024_02::endRound()
{
    if (m_completeCB)
    {
        m_completeCB();
    }
}

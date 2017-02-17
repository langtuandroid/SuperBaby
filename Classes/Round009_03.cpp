//
//  Round007_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round009_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                5.0f                                    // 庆祝延时

#define GK_09_PNG                       "res/stage009/SL-GK09-IT.png"
#define DRAW_TAG                        101
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间


/*
 *  constructor
 */
Round009_03::Round009_03()
:m_correctNumber(0)
{}

/*
 *  destructor
 */
Round009_03::~Round009_03()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round009_03* Round009_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round009_03();
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
bool Round009_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pSBN = SpriteBatchNode::create(GK_09_PNG);
    addChild(m_pSBN, 1);
    
    char szBuffer[32];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK09-3-DA%02ld.png", i + 1);
        m_chipSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_chipSp[i]->setUserData((void*)i);
        m_pSBN->addChild(m_chipSp[i], 2);
    } /*for*/
    
    float posx = 1000.0f, posy = 166.0f, offsetx = 268.0f;
    bool placed[3] = {false, false, false};
    unsigned int allPlaced = 0;
    srand(unsigned(time(0)));
    while (allPlaced < 3) {
        auto randPlacePos = static_cast<unsigned int>(rand() % 3);
        if (placed[randPlacePos]) continue;
        placed[randPlacePos] = true;
        m_chipSp[randPlacePos]->setPosition(Vec2(posx, posy));
        m_originalPoints[randPlacePos] = Vec2(posx, posy);
        posx += offsetx;
        allPlaced++;
    } /*while*/

    m_pDraw = ImageView::create("SL-GK09-3-TM01-1.png",ui::Widget::TextureResType::PLIST);
    m_pDraw->setPosition(Vec2(1240.0f, 580.0f));
    this->addChild(m_pDraw, 0, DRAW_TAG);

    m_targetRect = Rect(998.0f, 632.0f, 166.0f, 134.0f);
    
    return true;
}

// 开始回合
void Round009_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round009_03::changeScaleSchedule), 0.6f, -1, 0);
}

void Round009_03::changeScaleSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round009_03::changeScaleSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_chipSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

// 庆祝
void Round009_03::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

//
void Round009_03::addListener()
{
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        unsigned long selIndex = (unsigned long)target->getUserData();

        if (m_targetRect.containsPoint(convertToNodeSpace(touch->getLocation())) ) {
            if (selIndex == m_correctNumber) /*答对了*/ {
//                m_chipSp[selIndex]->setAnchorPoint(Vec2::ZERO);
//                m_chipSp[selIndex]->setPosition(Vec2(m_targetRect.origin.x - 30.0f, m_targetRect.origin.y - 30.0f));
                m_chipSp[selIndex]->setVisible(false);
                m_pDraw->loadTexture("SL-GK09-3-TM01-2.png",ui::Widget::TextureResType::PLIST);
                // remove listener
                auto eventDisptacher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++) {
                    eventDisptacher->removeEventListenersForTarget(m_chipSp[i]);
                } /*for*/
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
                celebrate(3.0f);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[selIndex])),CallFunc::create([this,target,selIndex]{
                    target->setPosition(m_originalPoints[selIndex]);
                }), NULL));
            }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[selIndex])),CallFunc::create([this,target,selIndex]{
                target->setPosition(m_originalPoints[selIndex]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event) {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
    };
    auto eventDisptacher = Director::getInstance()->getEventDispatcher();
    eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag, m_chipSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_chipSp[i]);
    } /*for*/
}

/* 开启触摸 */
void Round009_03::openTouch()
{
    addListener();
}

/*  回合结束回调 */
void Round009_03::endRound()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != m_correctNumber) {
            m_chipSp[i]->runAction(FadeOut::create(2.0f));
        }
    } /*for*/
    
    if (m_completeCB) {
        m_completeCB();
    }
}

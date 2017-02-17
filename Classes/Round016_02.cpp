//
//  Round016_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#include "Round016_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PNG_FILE                     "res/stage016/XQ-GK04-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define CELEBRATE_DELAY                 5.0f
#define BUTTERFLY_ZORDER                3
#define BOARD_ZORDER                    1
#define BOARD_PNG_FILE                  "XQ-GK04-2-TM01-01.png"
#define BOARD_POS                       Vec2(1228,1041)
#define HINT_ZORDER                     2
#define HINT_PNG_FILE                   "XQ-GK04-2-TM01-02.png"
#define HINT_POS                        Vec2(1327,782)
#define MOV_TIME                        0.2f                                        // 答错回原位置移动时间

/*
 *  constructor
 */
Round016_02::Round016_02()
:m_InitAniIndex(0)
{
    m_butterflysPos[0] = Vec2(980, 290);
    m_butterflysPos[1] = Vec2(1228, 290);
    m_butterflysPos[2] = Vec2(1476, 290);
}

/*
 *  destructor
 */
Round016_02::~Round016_02()
{
    cleanUp();
}

//
void Round016_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round016_02* Round016_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round016_02();
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
bool Round016_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK04-2-DA01-%02ld.png",i+1);
        m_pButterflys[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pButterflys[i]->setUserData((void*)i);
        this->addChild(m_pButterflys[i], BUTTERFLY_ZORDER);
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pBN->addChild(m_pHintSp, HINT_ZORDER);
    m_pHintSp->setPosition(HINT_POS);
    
    
    m_pBoardSp = Sprite::createWithSpriteFrameName(BOARD_PNG_FILE);
    m_pBoardSp->setPosition(BOARD_POS);
    m_pBN->addChild(m_pBoardSp, BOARD_ZORDER);
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pButterflys[all]->setPosition(m_butterflysPos[randIndex]);
        m_butterflyEndPos[all] = m_butterflysPos[randIndex];
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round016_02::onEnterRound()
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
    
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round016_02::fadeHintSchedule), 2.0f);
}

//渐变提示框
void Round016_02::fadeHintSchedule(float)
{
    m_pHintSp->runAction(Sequence::create(FadeOut::create(0.4f),
                                                          FadeIn::create(0.4f),
                                                          FadeOut::create(0.4f),
                                                          FadeIn::create(0.4f), NULL));
}


/* 开启触摸 */
void Round016_02::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
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
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp->getContentSize().width, m_pHintSp->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(m_pHintSp->convertTouchToNodeSpace(touch))) {
            if (index == 2) {
                target->setPosition(Vec2(HINT_POS.x-2, HINT_POS.y));
                target->loadTexture("XQ-GK04-2-DA01-04.png",ui::Widget::TextureResType::PLIST);
                m_pHintSp->setVisible(false);
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
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pButterflys[i]);
                } /*for*/
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_butterflyEndPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_butterflyEndPos[index]);
                }), NULL));
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_butterflyEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_butterflyEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pButterflys[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pButterflys[i]);
    } /*for*/
    
}

//答对后上升板
void Round016_02::upBoard()
{
    m_pBoardSp->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500.0f)));
    m_pButterflys[2]->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500.0f)));
}

//答对后渐隐剩余蝴蝶
void Round016_02::fadeOutButterfly()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 2) {
            m_pButterflys[i]->runAction(FadeOut::create(0.5f));
        }
    }
}

/*
 *  庆祝
 */
void Round016_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round016_02::endRound()
{
    upBoard();
    fadeOutButterfly();
    scheduleOnce([this](float){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 2.0f, "delayTime");
}
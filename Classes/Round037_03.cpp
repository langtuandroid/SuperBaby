//
//  Round037_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/8.
//
//

#include "Round037_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PNG_FILE                    "res/stage037/GC-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值


// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define BIRDEGG_ZORDER                  2                                       //蛋的z值
#define BIRDNEST_ZORDER                 1                                       //窝的z值
#define BIRDNEST_MOST_NUM               2                                       //每个鸟窝存放蛋最大个数
#define COLORLAYER_TAG                  300
#define PASS_GAF_FILE                   "res/common/yanshi01/yanshi01.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3
/*
 *  constructor
 */
Round037_03::Round037_03()
:m_curCorrectNumber(0),
m_birdNestOneEggNum(0),
m_birdNestTwoEggNum(0)
{
    
    m_birdEggsPos[0] = Vec2(850, 300);
    m_birdEggsPos[1] = Vec2(1100, 150);
    m_birdEggsPos[2] = Vec2(1370, 280);
    m_birdEggsPos[3] = Vec2(1600, 180);
    
    m_birdNestsPos[0] = Vec2(1200, 680);
    m_birdNestsPos[1] = Vec2(1720, 638);
}

/*
 *  destructor
 */
Round037_03::~Round037_03()
{
    cleanUp();
}

//
Round037_03* Round037_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round037_03();
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
bool Round037_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "GC-GK01-3-DA.png");
        m_pBirdEggs[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBirdEggs[i]->setPosition(m_birdEggsPos[i]);
        m_pBirdEggs[i]->setScale(0.9f);
        m_pBN->addChild(m_pBirdEggs[i], BIRDEGG_ZORDER);
        m_pBirdEggs[i]->setUserData((void*)i);
    }
    
    //鸟窝一
    for (int i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK01-3-TM-C-%02d.png",i+1);
        m_pBirdNestOne[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBirdNestOne[i]->setPosition(m_birdNestsPos[0]);
        m_pBirdNestOne[i]->setScale(0.9f);
        m_pBN->addChild(m_pBirdNestOne[i], BIRDNEST_ZORDER);
        if (i != 0) {
            m_pBirdNestOne[i]->setVisible(false);
        }else
        {
            m_birdNestRect[0] = m_pBirdNestOne[i]->getBoundingBox();
        }
    }
    
    //鸟窝二
    for (int i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK01-3-TM-C-%02d.png",i+1);
        m_pBirdNestTwo[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBirdNestTwo[i]->setPosition(m_birdNestsPos[1]);
        m_pBirdNestTwo[i]->setScale(0.9f);
        m_pBN->addChild(m_pBirdNestTwo[i], BIRDNEST_ZORDER);
        if (i != 0) {
            m_pBirdNestTwo[i]->setVisible(false);
        }else
        {
            m_birdNestRect[1] = m_pBirdNestTwo[i]->getBoundingBox();
        }
    }
    
    return true;
}

// 进入本回合
void Round037_03::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        if (!UserDefault::getInstance()->getBoolForKey("slidethree", false)) {
            playPassActor();
            UserDefault::getInstance()->setBoolForKey("slidethree", true);
        }else
        {
            openTouch();        //开启触摸
        }
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
}

/* 播放过关引导 */
void Round037_03::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->addChild(colorLayer,PASS_GAF_ZORDER);
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, colorLayer);
    dispatcher->setPriority(listenerDrag, -130);
    Actor* passActor = Actor::createWithGafFile(PASS_GAF_FILE,false);
    passActor->setPosAtNode(Vec2(300, 500));
    passActor->setPosition(Vec2(570, 300));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]{
        colorLayer->removeFromParent();
        passActor->setVisible(false);
        openTouch();        //开启触摸
    });
}

//
void Round037_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(PASS_GAF_FILE);
}

/* 开启触摸 */
void Round037_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.0f);
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
        long index = (long)target->getUserData();
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        
        target->setScale(0.9f);
        if (m_birdNestRect[0].containsPoint(touchPoint))   //判断是否在左边鸟窝
        {
            if (m_birdNestOneEggNum < BIRDNEST_MOST_NUM) {
                target->setVisible(false);
                m_pBirdNestOne[m_birdNestOneEggNum]->setVisible(false);
                m_pBirdNestOne[m_birdNestOneEggNum+1]->setVisible(true);
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                m_birdNestOneEggNum++;
                m_curCorrectNumber++;
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_birdEggsPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_birdEggsPos[index]);
                }), NULL));
            }
        }else if (m_birdNestRect[1].containsPoint(touchPoint))  //判断是否在右边鸟窝
        {
            if (m_birdNestTwoEggNum < BIRDNEST_MOST_NUM) {
                target->setVisible(false);
                m_pBirdNestTwo[m_birdNestTwoEggNum]->setVisible(false);
                m_pBirdNestTwo[m_birdNestTwoEggNum+1]->setVisible(true);
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                m_birdNestTwoEggNum++;
                m_curCorrectNumber++;
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_birdEggsPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_birdEggsPos[index]);
                }), NULL));
            }
        } else {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_birdEggsPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_birdEggsPos[index]);
            }), NULL));
        }
        
        if (m_curCorrectNumber == 4) {      //全部答对
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            celebrate(CELEBRATE_DELAY);
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pBirdEggs[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pBirdEggs[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round037_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round037_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

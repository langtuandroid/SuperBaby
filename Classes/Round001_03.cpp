//
//  Round001_03.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/09.
//
//

#include "Round001_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 物品大图
#define GOODS_PLIST_FILE      "res/stage001/SL-GK01-IT.plist"
#define GOODS_IMAGE_FILE      "res/stage001/SL-GK01-IT.png"

// npc
#define NPC_FILE                "res/stage001/SL-GK01-2-NPC02-D/SL-GK01-2-NPC02-D.gaf"
#define NPC_POSITION            Vec2(700.0f, 450.0f)

// 兔子的gaf
#define MS_RABBIT_GAF_FILE      "config/stage001/ms_robbit.json"
#define MS_RABBIT_FILE          "res/stage001/SL-GK01-2-MS01-D+C/SL-GK01-2-MS01-D+C.gaf"
#define RABBIT_STANDBY          "standby"                               // 待机动画
#define RABBIT_TRIGGER          "trigger"                               // 触发动画

// 兔子的位置
#define RABBIT_GAF_POS          Vec2(1604.0f, 800.0f)
#define RABBIT_GSF_ZORDER       1                               // 兔子的Z值

#define GOODS_NODE_ZORDER       1                               // 物品Z值

#define TARGET_GOODS_ZORDER     1                               // 目标物品Z值
#define GOODS_ZORDER            2                               // 单个物品Z值

#define FadeIn_TIME             2                               // 物品淡入时间
#define FadeOut_TIME            2                               // 物品淡出时间

#define TARGET_GOODS_POSX       1007.0                          // 目标物品起始位置X值（从左至右）
#define TARGET_GOODS_POSY       400.0f                          // 目标物品位置y值

// 答题正确数量达到该数字怎本回合结束
#define NUMBER_OF_ANSWER_CORRECT        3

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define MOV_TIME                        0.2f                    // 答错回原位置移动时间


/*
 *  constructor
 */
Round001_03::Round001_03() {}

/*
 *  destructor
 */
Round001_03::~Round001_03()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    GafAssetCache::getInstance()->removeGafAssetForKey(NPC_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_RABBIT_FILE);
}

//
Round001_03* Round001_03::createWithRoundIndex(const int roundIndex,
                                               StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round001_03();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene, pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

bool Round001_03::initWithRoundIndex(const int roundIndex,
                                     StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_goodsInitPos[0] = Vec2(924.0f, 800.0f);
    m_goodsInitPos[1] = Vec2(1682.0f, 1100.0f);
    m_goodsInitPos[2] = Vec2(1402.0f, 900.0f);
    m_goodsInitPos[3] = Vec2(1702.0f, 800.0f);
    
    appendAsyncLoadTexture(GOODS_IMAGE_FILE);
    appendAsyncLoadGaf(NPC_FILE);
    appendAsyncLoadGaf(MS_RABBIT_FILE);
    preLoadRes();
    return true;
}

void Round001_03::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // gaf
    m_rabbit = Actor::createWithGafConfigFile(MS_RABBIT_GAF_FILE);
    m_rabbit->setPosition(RABBIT_GAF_POS);
    addChild(m_rabbit, RABBIT_GSF_ZORDER);
    m_rabbit->playWithSequenceAliasName(RABBIT_TRIGGER,true);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(GOODS_PLIST_FILE);
    m_pGoodsBN = SpriteBatchNode::create(GOODS_IMAGE_FILE);
    addChild(m_pGoodsBN,GOODS_NODE_ZORDER);
    
    char gsBuffer[64];
    for (unsigned long i = 0; i < 4; i++)
    {
        sprintf(gsBuffer, "SL-GK01-2-DA%02ld.png", i + 1);
        m_goodsSp[i] = Sprite::createWithSpriteFrameName(gsBuffer);
        m_pGoodsBN->addChild(m_goodsSp[i], GOODS_ZORDER);
        m_goodsSp[i]->setUserData((void*)i);
        m_goodsSp[i]->setPosition(Vec2(m_goodsInitPos[i].x, m_goodsInitPos[i].y));
    }/*for*/
    
    const float offsetX = 250.0f;
    for (unsigned int i = 0; i < 3; i++)
    {
        sprintf(gsBuffer, "SL-GK01-2-TM01-%d.png", i + 1);
        m_targetGoodsSp[i] = Sprite::createWithSpriteFrameName(gsBuffer);
        m_pGoodsBN->addChild(m_targetGoodsSp[i], TARGET_GOODS_ZORDER);
    }
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3)
    {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_targetGoodsSp[all]->setPosition(Vec2(TARGET_GOODS_POSX + offsetX * Index,
                                               TARGET_GOODS_POSY));
        all++;
    } /*while*/
    
    // npc
    m_pNpcActor = Actor::createWithGafFile(NPC_FILE, true, 1.0f, Vec2(-16.0f, 200.0f));
    m_pNpcActor->setPosition(NPC_POSITION);
    m_pNpcActor->setSize(Size(180.0f, 180.0f));
    addChild(m_pNpcActor, 1);
    
    if (m_pStageScene != nullptr) {
        m_pStageScene->appendRemovedTexture(GOODS_IMAGE_FILE);
        m_pStageScene->appendRemovedSpriteFrameFile(GOODS_PLIST_FILE);
    }
}

//
void Round001_03::onEnterRound()
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
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        processNpcCollect();
    });
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round001_03::changeScaleGoodsSchedule), 0.6f, -1, 0);
}

//改变物品大小
void Round001_03::changeScaleGoodsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round001_03::changeScaleGoodsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_goodsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
}

void Round001_03::openTouch()
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
        auto boundingBox = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (boundingBox.containsPoint(touchPoint))
        {
            long index = (long)target->getUserData();
            m_originalPoint =  m_goodsInitPos[index];
            m_touchBeganPoint = touch->getLocation();
            m_pGoodsBN->reorderChild(target, GOODS_ZORDER + 1);
            target->setPosition(m_originalPoint);
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
        unsigned long dragNumber = (unsigned long)target->getUserData();
        target->setScale(1.0f);
        m_pGoodsBN->reorderChild(target, GOODS_ZORDER);
        
        Sprite* pSp = nullptr;
        bool isCorrect = false;
        
        Rect rect = Rect(0.0f, 0.0f, m_targetGoodsSp[0]->getContentSize().width, m_targetGoodsSp[0]->getContentSize().height);
        if (rect.containsPoint(m_targetGoodsSp[0]->convertTouchToNodeSpace(touch))) {
            if (0 == dragNumber) {
                pSp = m_targetGoodsSp[0];
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_targetGoodsSp[1]->getContentSize().width, m_targetGoodsSp[1]->getContentSize().height);
        if (rect.containsPoint(m_targetGoodsSp[1]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && 3 == dragNumber) {
                pSp = m_targetGoodsSp[1];
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_targetGoodsSp[2]->getContentSize().width, m_targetGoodsSp[2]->getContentSize().height);
        if (rect.containsPoint(m_targetGoodsSp[2]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && 2 == dragNumber) {
                pSp = m_targetGoodsSp[2];
                isCorrect = true;
            }
        }
        
        if (isCorrect) /*答题正确*/
        {
            pSp->setVisible(false);
            m_numberOfdragComplete++;
            target->setPosition(pSp->getPosition());
            auto dispatcher = Director::getInstance()->getEventDispatcher();
            dispatcher->removeEventListenersForTarget(target);
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            if (NUMBER_OF_ANSWER_CORRECT == m_numberOfdragComplete) /*全部答对，过关*/
            {
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_goodsSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
            } else {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_goodsInitPos[dragNumber])),CallFunc::create([this,target,dragNumber]{
                target->setPosition(m_goodsInitPos[dragNumber]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_goodsSp[0]);
    for (unsigned int i = 1; i < 4; i++)
    {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_goodsSp[i]);
    } /*for*/
}

//
void Round001_03::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round001_03::endRound()
{
    fadeOutGoodsCelebrateComplete(); //渐变隐藏物品
    fadeOutRabbitCelebrateComplete(); //兔子改变动作
    if (m_completeCB)
    {
        m_completeCB();
    }
}

/*
 *  rabbit改变动作
 */
void Round001_03::fadeOutRabbitCelebrateComplete()
{
    m_rabbit->playWithSequenceAliasName(RABBIT_STANDBY);
    m_rabbit->setAnimationFinishedPlayDelegate([this] {
        m_rabbit->setVisible(false);
    });
}

/*
 * 渐变隐藏物品
 */
void Round001_03::fadeOutGoodsCelebrateComplete()
{
    for (unsigned int i=0; i < 4; i++)
    {
        m_goodsSp[i]->runAction(FadeOut::create(FadeOut_TIME));
    }
}


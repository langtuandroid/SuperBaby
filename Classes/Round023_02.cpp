//
//  Round023_02.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/09.
//
//

#include "Round023_02.h"
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
#define STAGE_IMAGE_FILE        "res/stage023/XQ-GK11-IT.png"
#define STAGE_PLIST_FILE        "res/stage023/XQ-GK11-IT.plist"

#define BRUSH_GAF_FILE          "res/stage023/XQ-GK11-2-IT-D+C/XQ-GK11-2-IT-D+C.gaf"
#define BRUSH_GAF_CONFIG        "res/stage023/XQ-GK11-2-IT-D+C.json"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define SCALE_DURATION                  0.3f
#define SCALE_FACTOR                    1.5f

/*
 *  constructor
 */
Round023_02::Round023_02() {}

/*
 *  destructor
 */
Round023_02::~Round023_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(BRUSH_GAF_FILE);
}

//
Round023_02* Round023_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round023_02();
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
bool Round023_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(STAGE_IMAGE_FILE);
    appendAsyncLoadGaf(BRUSH_GAF_FILE);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(STAGE_PLIST_FILE);
    m_pStageScene->appendRemovedSpriteFrameFile(STAGE_PLIST_FILE);
    m_pStageScene->appendRemovedTexture(STAGE_IMAGE_FILE);
    
    preLoadRes();
    return true;
}

//
void Round023_02::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // 大图
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 1);
    
    // 白板
    m_pNpcSp = Sprite::createWithSpriteFrameName("XQ-GK11-2-DA01.png");
    m_pNpcSp->setPosition(Vec2(1110.0f, 196.0f));
    m_pSBN->addChild(m_pNpcSp, 1);
    
    // 颜料
    m_pPigmentSp[0] = Sprite::createWithSpriteFrameName("XQ-GK11-2-DA02.png");
    m_pPigmentSp[0]->setPosition(Vec2(996.0f, 248.0f));
    m_pSBN->addChild(m_pPigmentSp[0], 2);
    m_pPigmentSp[0]->setUserData((void*)0);
    // 紫
    m_pPigmentSp[1] = Sprite::createWithSpriteFrameName("XQ-GK11-2-DA03.png");
    m_pPigmentSp[1]->setPosition(Vec2(1246.0f, 248.0f));
    m_pPigmentSp[1]->setUserData((void*)1);
    m_pSBN->addChild(m_pPigmentSp[1], 2);
    srand(unsigned(time(0)));
    int ridx = rand() % 2;
    if (ridx == 0) {
        m_pPigmentSp[0]->setPosition(Vec2(996.0f, 248.0f));
        m_pPigmentSp[1]->setPosition(Vec2(1246.0f, 248.0f));
    } else {
        m_pPigmentSp[1]->setPosition(Vec2(996.0f, 248.0f));
        m_pPigmentSp[0]->setPosition(Vec2(1246.0f, 248.0f));
    }
    m_originalPoints[0] = m_pPigmentSp[0]->getPosition();
    m_originalPoints[1] = m_pPigmentSp[1]->getPosition();
    
    m_pBrushActor = Actor::createWithGafConfigFile(BRUSH_GAF_CONFIG);
    m_pBrushActor->playWithSequenceAliasName("standby", true);
    m_pBrushActor->setPosition(Vec2(1594.0f, 418.0f));
    addChild(m_pBrushActor, 2);
    
    m_pCarrotsSps[0] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM02.png");
    m_pCarrotsSps[1] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM03.png");
    m_pCarrotsSps[2] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM02.png");
    m_pCarrotsSps[3] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM03.png");
    m_pCarrotsSps[4] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM02.png");
    m_pCarrotsSps[5] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM01.png");
    float posx = 584.0f, posy = 870.0f, offsetx = m_pCarrotsSps[0]->getContentSize().width + 72.0f;
    for (int i = 0; i < 6; i++)
    {
        m_pCarrotsSps[i]->setPosition(Vec2(posx + i * offsetx, posy));
        m_pSBN->addChild(m_pCarrotsSps[i], 1);
    } /*for*/
}

//
void Round023_02::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
   {
       dispatcher->removeCustomEventListeners("openTouch");
       m_opentouched = true;
       if (m_scaled) {
           openTouch();
       }
   });
    auto pScaleSp = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM04.png");
    pScaleSp->setPosition(m_pCarrotsSps[5]->getPosition());
    addChild(pScaleSp, 2);
    
    auto scaledCB = CallFunc::create([this, pScaleSp] {
        m_scaled = true;
        pScaleSp->setPosition(Vec2(0.0f, -10000.0f));
        if (m_opentouched) {
            openTouch();
        }
    });
    auto scaleSeq = Sequence::create(ScaleTo::create(SCALE_DURATION, SCALE_FACTOR),
                                     ScaleTo::create(SCALE_DURATION, 1.0f),
                                     scaledCB, NULL);
    pScaleSp->runAction(scaleSeq);
}

/*
 *  开启触摸 
 */
void Round023_02::openTouch()
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
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.3f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        long index = (long)target->getUserData();
        m_movePos.x = m_originalPoints[index].x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoints[index].y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        
        target->setScale(1.0f);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f,
                         m_pCarrotsSps[5]->getContentSize().width,
                         m_pCarrotsSps[5]->getContentSize().height);
        if (rect.containsPoint(m_pCarrotsSps[5]->convertTouchToNodeSpace(touch)) )
        {
            if (1 == index)
            {
                isCorrect = true;
            }
        }
        if (isCorrect) /* 答题正确 */
        {
            target->setVisible(false);
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPigmentSp[0]);
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPigmentSp[1]);
            
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            
            m_pBrushActor->setAnimationFinishedPlayDelegate([this]
            {
                auto pos = m_pCarrotsSps[5]->getPosition();
                m_pCarrotsSps[5]->setPosition(Vec2(0.0f, -10000.0f));
                m_pCarrotsSps[5] = Sprite::createWithSpriteFrameName("XQ-GK11-2-TM03.png");
                m_pCarrotsSps[5]->setPosition(pos);
                m_pSBN->addChild(m_pCarrotsSps[5], 1);
                
                m_pBrushActor->setPosition(Vec2(0.0f, -10000.0f));
                celebrate(CELEBRATE_DELAY);
            });
            m_pBrushActor->playWithSequenceAliasName("trigger", false);
        } else {
            if (m_anwerErrorCB)
            {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答错时主角说话
                playLeadAnsErrEff();
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(0.2f, m_originalPoints[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_originalPoints[index]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_originalPoints[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pPigmentSp[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pPigmentSp[1]);
}

//
void Round023_02::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*
 *  回合结束回调
 */
void Round023_02::endRound()
{
    m_pNpcSp->runAction(MoveBy::create(1.2f, Vec2(0.0f, -m_pNpcSp->getContentSize().height)));
    m_pPigmentSp[0]->runAction(MoveBy::create(1.2f, Vec2(0.0f, -m_pNpcSp->getContentSize().height)));
    if (m_completeCB)
    {
        m_completeCB();
    }
}


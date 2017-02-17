//
//  Round023_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/3/27.
//
//

#include "Round023_04.h"
#include "StageScene.h"
#include "ThemeSelScene.h"
#include "StageDataManager.h"
#include "StageData.h"
#include "LeadingActor.h"
#include "SocialShareNode.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SelStageScene.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "AppConfig.h"
#include "GafAssetCache.h"

// 大图
#define STAGE_IMAGE_FILE        "res/stage023/XQ-GK11-IT.png"
#define STAGE_PLIST_FILE        "res/stage023/XQ-GK11-IT.plist"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define MOVE_ACTOR_TIME                 5.0f                                // 分享界面ok过后主角移动时间
#define MOV_TIME                        0.3f                                // 答错回原位置移动时间

#define SCALE_ORIGIN                    0.34
#define SCALE_FACTOR                    1.2
#define SCALE_DURATION                  0.3f

//
#define NPC_GAF_FILE                    "res/stage023/XQ-GK11-4-TM02-D+C/XQ-GK11-4-TM02-D+C.gaf"
#define NPC_GAF_CONFIG                  "res/stage023/XQ-GK11-4-TM02-D+C.json"

// 房子和门
#define DOOR_IMG_FILE                   "res/stage023/XQ-GK11-4-IT02.png"
#define HOUSE_IMG_FILE                  "res/stage023/XQ-GK11-4-IT04-4.png"

// 门掩码
#define DOOR_MASK                       "res/stage023/XQ-GK11-4-IT04.png"
#define OVER_TAG                        501
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round023_04::Round023_04() {}

/*
 *  destructor
 */
Round023_04::~Round023_04()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    dispatcher->removeCustomEventListeners("openTouch");
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(NPC_GAF_FILE);
}

//
Round023_04* Round023_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round023_04();
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
bool Round023_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(DOOR_IMG_FILE);
    appendAsyncLoadTexture(HOUSE_IMG_FILE);
    appendAsyncLoadTexture(DOOR_MASK);
    appendRemovedTexture(DOOR_IMG_FILE);
    appendRemovedTexture(HOUSE_IMG_FILE);
    appendRemovedTexture(DOOR_MASK);
    
    appendAsyncLoadGaf(NPC_GAF_FILE);
    
    preLoadRes();
    return true;
}

void Round023_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    m_pSBN = SpriteBatchNode::create(STAGE_IMAGE_FILE);
    addChild(m_pSBN, 2);
    
    m_pNpcActor = Actor::createWithGafConfigFile(NPC_GAF_CONFIG);
    m_pNpcActor->setPosition(Vec2(928.0f + 1120.0f, 606.0f));
    m_pNpcActor->playWithSequenceAliasName("standby", true);
    addChild(m_pNpcActor, 1);
    
    //
    auto pHouseSp = Sprite::create(HOUSE_IMG_FILE);
    pHouseSp->setPosition(Vec2(2398.0f, 902.0f));
    addChild(pHouseSp, 2);
    
    m_pDoorSp = Sprite::create(DOOR_IMG_FILE);
    m_pDoorSp->setPosition(Vec2(1954.0f, 796.0f));
    addChild(m_pDoorSp, 3);
    
    // 形状
    char szBuffer[64];
    for (long i = 0; i < 3; i++)
    {
        sprintf(szBuffer, "XQ-GK11-4-DA%02ld.png", i + 1);
        m_npcSp[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_npcSp[i]->setUserData((void*)i);
        m_pSBN->addChild(m_npcSp[i], 1);
    } /*for*/
    
    // 随机
    float posx = 1002.0f, posy = 328.0f, offsetx = 288.0f;
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3)
    {
        unsigned int rdx = static_cast<unsigned int>(rand() % 3);
        if (selected[rdx]) continue;
        selected[rdx] = true;
        m_npcSp[all]->setPosition(Vec2(posx + rdx * offsetx, posy));
        m_jnpcSp[rdx] = m_npcSp[all];
        m_originalPoints[all] = m_npcSp[all]->getPosition();
        all++;
    } /*while*/
    for (int i = 0; i < 3; i++)
    {
        m_jnpcSp[i]->setPosition(Vec2(m_jnpcSp[i]->getPosition().x, m_jnpcSp[i]->getPosition().y - 600.0f));
    } /*for*/
    
    m_doorMaskSp = Sprite::create(DOOR_MASK);
    m_pStageScene->addOverLeaderNode(m_doorMaskSp, Vec2(14450, 904.0f), OVER_TAG);
}

/*
 *  进入回合
 */
void Round023_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
        dispatcher->removeCustomEventListeners("openTouch");
        m_isopentouched = true;
        if (m_isjumped)
        {
            openTouch();
        }
    });
    auto movedCB = CallFunc::create([this]
    {
        auto upedCB = CallFunc::create(std::bind(&Round023_04::upCompleteCB, this));
        long index = (long)m_jnpcSp[0]->getUserData();
        m_jnpcSp[0]->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME+0.3f, m_originalPoints[index])),
                                                upedCB, NULL));
    });
    m_pNpcActor->runAction(Sequence::create(MoveBy::create(4.0f, Vec2(-1120.0f, 0.0f)), movedCB, NULL));
}

//
void Round023_04::upCompleteCB()
{
    if (++m_numberOfUped == 3)
    {
        m_isjumped = true;
        if (m_isopentouched)
        {
            openTouch();
        }
    } else {
        auto upedCB = CallFunc::create(std::bind(&Round023_04::upCompleteCB, this));
        long index = (long)m_jnpcSp[m_numberOfUped]->getUserData();
        m_jnpcSp[m_numberOfUped]->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME+0.3f, m_originalPoints[index])), upedCB, NULL));
    }
}

/*
 *  开启触摸
 */
void Round023_04::openTouch()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            m_pSBN->reorderChild(target, 2);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos = Vec2::ZERO;
        
        long index = (long)target->getUserData();
        m_movePos.x = m_originalPoints[index].x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoints[index].y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        this->reorderChild(target, 1);
        
        long shapeIndex = (long)target->getUserData();
        bool isTouchIn = false;
        bool isCorrected = false;
        auto touchPoint = m_pNpcActor->convertTouchToNodeSpace(touch);
        float cx = m_pNpcActor->getSize().width / 2.0f;
        float cy = m_pNpcActor->getSize().height / 2.0f;
        auto targetRect = Rect(cx - target->getContentSize().width / 2.0f,
                               cy - target->getContentSize().height / 2.0f,
                               target->getContentSize().width,
                               target->getContentSize().height);
        if (targetRect.containsPoint(touchPoint))
        {
            isTouchIn = true;
            if (2 == shapeIndex)
            {
                isCorrected = true;
            }
        }
        if (isTouchIn && isCorrected)
        {
            target->setPosition(Vec2(0.0f, -10000.0f));
            m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x, touch->getLocation().y));    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            answerCorrectCB();
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) { // 沮丧
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[shapeIndex])), CallFunc::create([this, target, shapeIndex]
            {
                target->setPosition(m_originalPoints[shapeIndex]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_npcSp[0]);
    for (unsigned int i = 1; i < 3; i++)
    {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(),  m_npcSp[i]);
    }
}

/* 
 *  全部答对回调
 */
void Round023_04::answerCorrectCB()
{
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    for (unsigned int i = 0; i < 3; i++)
    {
        eventDispatcher->removeEventListenersForTarget(m_npcSp[i]);
    } /*for*/
    
    m_pStageScene->getRoundUINode()->stopReadEffect();          // 停止题目音效
    m_pStageScene->getRoundUINode()->endReadTopic();            // 停止读题目改变动
    
    
    Vec2 tpos = Vec2(m_npcSp[0]->getPosition().x, m_npcSp[0]->getPosition().y - 600.0f);
    m_npcSp[0]->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME+0.3f, tpos)), NULL));
    
    tpos = Vec2(m_npcSp[1]->getPosition().x, m_npcSp[1]->getPosition().y - 600.0f);
    m_npcSp[1]->runAction(Sequence::create(DelayTime::create(MOV_TIME), EaseBackOut::create(MoveTo::create(MOV_TIME+0.3f, tpos)), NULL));
    
    m_pNpcActor->playWithSequenceAliasName("trigger", false);
    m_pNpcActor->setAnimationFinishedPlayDelegate([this]
      {
          auto moveoutedCB = CallFunc::create([this]
          {
              m_npcIsMoveOut = true;
          });
          m_pNpcActor->runAction(Sequence::create(MoveBy::create(4.0f, Vec2(1120.0f, 0.0f)), moveoutedCB, NULL));
      });
    celebrate(CELEBRATE_DELAY); //庆祝
}

/*
 *  庆祝
 */
void Round023_04::celebrate(const float celebrateDelay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round023_04::endRound() {}

//回合结束调用
void Round023_04::roundEndShareCallBack(bool isShare)
{
    if (isShare)
    {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    } else {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    
    schedule([this] (float dt)
    {
        log("11");
        if (m_npcIsMoveOut) {
            unschedule("waitNpcOut");
            // 开门
            openDoor();
        }
    }, 0.1f, -1, 0.0f, "waitNpcOut");
}

//
void Round023_04::openDoor()
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);    // 停止分享音效
    
    // 门右移动
    auto doorMoveCompletedCB = CallFunc::create([this]
    {
        const float moveOffset = 2000.0f;
        m_pStageScene->getBeibeiActor()->walk(); m_pStageScene->getAimeeActor()->walk();
        
        m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(moveOffset, 0));
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round023_04::fadeOutLayer), MOVE_ACTOR_TIME-2.0f);    //退出
    });
    m_pDoorSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_pDoorSp->getContentSize().width + 16.0f, 0.0f)), Hide::create(), doorMoveCompletedCB, NULL));
}

/* 
 *  渐变变黑退出
 */
void Round023_04::fadeOutLayer(float dt)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width + 620.0f, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    schedule([this](float dt) {
        if (m_opacity == 155) {
            unschedule("add");
            if (m_pStageScene->isTrial())
            {
                // 试玩时停止播放背景音乐
                MyAudioEngine::getInstance()->stopAudio(0,true);
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            } else {
                // 过关
                m_pStageScene->stageClear();
            }
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.005f, "add");
}

//分享回调
void Round023_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round023_04::roundEndShareCallBack, this, true));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round023_04::roundEndShareCallBack, this, false));
}

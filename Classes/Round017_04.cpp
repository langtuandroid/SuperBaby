//
//  Round017_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#include "Round017_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "ThemeSelScene.h"
#include "LeadingActor.h"

#define STANDBY                         "standby"                                // 待机动画
#define TRIGGER                         "trigger"                             // 触发动画
#define CELEBRATE_DELAY                 5.0f
#define WOODS_ZORDER                    4
#define SAWMILL_GAF_FILE                "res/stage017/XQ-GK05-4-IT-D+C/XQ-GK05-4-IT-D+C.gaf "
#define SAWMILL_GAF_POS                 Vec2(511,442)
#define SAWMILL_GAF_ZORDER              1
#define SAWMILL_GAF_CONFIG_FILE         "config/stage017/sawmill.json"
#define MOVE_ACTOR_TIME                 2.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define BOARD_TAG                       202
#define TABLE_PNG_FILE                  "XQ-GK05-4-IT02.png"
#define TABLE_POS                       Vec2(1146,377)
#define TABLE_ZORDER                    2
#define WOOD_PNG_NORMAL_FILE            "XQ-GK05-4-TM01.png"
#define WOOD_PNG_TRIGGER_FILE           "XQ-GK05-4-TM02.png"
#define WOOD_POS                        Vec2(1246,477)
#define WOOD_ZORDER                     3

/*
 *  constructor
 */
Round017_04::Round017_04()
:m_InitAniIndex(0)
{
    m_woodsPos[0] = Vec2(725, -100);
    m_woodsPos[1] = Vec2(1126, -100);
    m_woodsPos[2] = Vec2(1527, -100);
}

/*
 *  destructor
 */
Round017_04::~Round017_04()
{
    cleanUp();
}

//
void Round017_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(SAWMILL_GAF_FILE);    
}

//
Round017_04* Round017_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round017_04();
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
bool Round017_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pSawMillActor = Actor::createWithGafConfigFile(SAWMILL_GAF_CONFIG_FILE);
    m_pSawMillActor->setPosition(SAWMILL_GAF_POS);
    this->addChild(m_pSawMillActor, SAWMILL_GAF_ZORDER);
    m_pSawMillActor->playWithSequenceAliasName(STANDBY,true);
    
    m_pTableSp = Sprite::createWithSpriteFrameName(TABLE_PNG_FILE);
    m_pTableSp->setPosition(TABLE_POS);
    this->addChild(m_pTableSp, TABLE_ZORDER);
    
    m_pWoodSp = ImageView::create(WOOD_PNG_NORMAL_FILE,ui::Widget::TextureResType::PLIST);
    m_pWoodSp->setPosition(WOOD_POS);
    this->addChild(m_pWoodSp, WOOD_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK05-4-DA%02ld.png",i+1);
        m_pWoodsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pWoodsSp[i], WOODS_ZORDER);
        m_pWoodsSp[i]->setUserData((void*)i);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pWoodsSp[all]->setPosition(m_woodsPos[randIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round017_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round017_04::upWoodsSchedule), 0.5f, -1, 0);
    
}

//改变尺寸以提示
void Round017_04::upWoodsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round017_04::upWoodsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pWoodsSp[m_InitAniIndex++]->runAction(Sequence::create(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, 300))), NULL));
}

/* 开启触摸 */
void Round017_04::openTouch()
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
            
            target->setScale(1.1f);
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
        
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {   //答对
                changeActor();          //改变actor动作
                changeWood();
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pWoodsSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
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
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pWoodsSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pWoodsSp[i]);
    } /*for*/
    
}

//改变木头状态
void Round017_04::changeWood()
{
    m_pWoodSp->loadTexture(WOOD_PNG_TRIGGER_FILE,ui::Widget::TextureResType::PLIST);
}

//改变actor
void Round017_04::changeActor()
{
    m_pSawMillActor->playWithSequenceAliasName(TRIGGER);
    m_pSawMillActor->setAnimationFinishedPlayDelegate([this]
    {
        m_InitAniIndex = 0;
        schedule(CC_SCHEDULE_SELECTOR(Round017_04::downMoveAnswersSchedule), 0.5f, -1, 0);
    });
}

//下移答案
void Round017_04::downMoveAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round017_04::downMoveAnswersSchedule));
        return;
    }
    m_pWoodsSp[m_InitAniIndex++]->runAction(MoveBy::create(0.5f, Vec2(0.0f, -300)));
}

/*
 *  庆祝
 */
void Round017_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round017_04::endRound()
{
}

//回合结束分享调用
void Round017_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    if (m_completeCB) {
        m_completeCB();
    }
}

//分享回调
void Round017_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round017_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round017_04::roundEndShareCallBack, this,true));
}
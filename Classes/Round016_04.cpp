//
//  Round016_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#include "Round016_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"

#define STANDBY                         "standby"                                // 待机动画
#define TRIGGER                         "trigger"                             // 触发动画
#define CELEBRATE_DELAY                 5.0f
#define GOODS_ZORDER                    3
#define MS_GAF_CONFIG_FILE              "config/stage016/ms.json"
#define MS_GAF_FILE                     "res/stage016/XQ-GK04-4-MS01-D+C/XQ-GK04-4-MS01-D+C.gaf "
#define MS_GAF_POS                      Vec2(1200,600)
#define MS_GAF_ZORDER                   1
#define MOVE_ACTOR_TIME                 3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define BOARD_TAG                       202


/*
 *  constructor
 */
Round016_04::Round016_04()
:m_InitAniIndex(0)
{
    m_goodsPos[0] = Vec2(648, 850);
    m_goodsPos[1] = Vec2(948, 850);
    m_goodsPos[2] = Vec2(1248, 850);
    m_goodsPos[3] = Vec2(1548, 850);
}

/*
 *  destructor
 */
Round016_04::~Round016_04()
{
    cleanUp();
}

//
void Round016_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
    m_pStageScene->removeOBLeaderNode(BOARD_TAG);

}

//
Round016_04* Round016_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round016_04();
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
bool Round016_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "XQ-GK04-4-DA01-%ld.png",i+1);
        m_pGoodsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pGoodsSp[i], GOODS_ZORDER);
        m_pGoodsSp[i]->setUserData((void*)i);
    }
    
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pGoodsSp[all]->setPosition(m_goodsPos[randIndex]);
        all++;
    } /*while*/
    
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setPosition(MS_GAF_POS);
    this->addChild(m_pMSActor, MS_GAF_ZORDER);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    
    return true;
}

// 进入本回合
void Round016_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round016_04::scaleChangeGoodsSchedule), 0.6f, -1, 0);
    
}

//改变尺寸以提示
void Round016_04::scaleChangeGoodsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round016_04::scaleChangeGoodsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pGoodsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round016_04::openTouch()
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
        long index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) {   //答对
                changeActor();          //改变ms动作
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    //答对时主角说话
                    playLeadAnsCorEff();
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pGoodsSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pGoodsSp[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pGoodsSp[i]);
    } /*for*/
    
}

//隐藏剩余答案
void Round016_04::fadeOutAnswers()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_pGoodsSp[i]->runAction(FadeOut::create(0.4f));
    }
}

//改变actor
void Round016_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]
    {
        fadeOutAnswers();
    });
}

/*
 *  庆祝
 */
void Round016_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round016_04::endRound() {}

/*
 *  分享界面点击ok后移动主角
 */
void Round016_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0), true);
}

//渐变变黑退出
void Round016_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//回合结束分享调用
void Round016_04::roundEndShareCallBack(bool isShare)
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
    moveActor();        //主角移动
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round016_04::fadeOutLayer), 2.0f);
}

//分享回调
void Round016_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round016_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round016_04::roundEndShareCallBack, this,true));
}

//
//  Round047_04.cpp
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#include "Round047_04.h"
#include "Player.h"
#include "StageScene.h"
#include "AppConfig.h"
#include "Actor.h"
#include "RoundUINode.h"
#include "MyAudioEngine.h"
#include "SocialShareNode.h"
#include "ThemeSelScene.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"
#include "StageData.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MS_GAF_CONFIG_FILE              "res/stage047/ms_03.json"
#define MS_GAF_FILE                     "res/stage047/GC-GK11-4-MS-D+C/GC-GK11-4-MS-D+C.gaf"
#define MS_ZORDER                       4
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define COLORLAYER_TAG                  300


/*
 *  constructor
 */
Round047_04::Round047_04(){
    m_msPos[0] = Vec2(648, 394);
    m_msPos[1] = Vec2(1022, 344);
    m_msPos[2] = Vec2(1402, 394);
    m_msPos[3] = Vec2(1773, 308);
}

/*
 *  destructor
 */
Round047_04::~Round047_04(){
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
}

/*
 *  初始化
 */
Round047_04* Round047_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round047_04();
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
bool Round047_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    for (unsigned long i = 0; i < 4; i++) {
        m_pMSActors[i] = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
        m_pMSActors[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        m_pMSActors[i]->setUserData((void*)i);
        m_pMSActors[i]->playWithSequenceAliasName(STANDBY,true);
        if (i == 2) {
            m_pMSActors[i]->setRotationSkewY(-180);
        }
        this->addChild(m_pMSActors[i], MS_ZORDER);
    }
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pMSActors[all]->setPosition(m_msPos[randIndex]);
        all++;
    } /*while*/

    return true;
}

//
void Round047_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round047_04::changeScaleActorsSchedule), 0.6f, -1, 0);
    
}

//改变答案的大小
void Round047_04::changeScaleActorsSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round047_04::changeScaleActorsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pMSActors[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round047_04::openTouch()
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
        target->setScale(1.0f);
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 2) {   //答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                changeActor();
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pMSActors[i]);
                } /*for*/
            } else {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pMSActors[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pMSActors[i]);
    } /*for*/
}

void Round047_04::changeActor()
{
    for (unsigned int i = 0; i < 4; i++) {
        m_pMSActors[i]->playWithSequenceAliasName(TRIGGER,true);
    }
}

void Round047_04::moveActors()
{
    for (unsigned int i = 0; i < 4; i++) {
        if (i == 2) {
            m_pMSActors[i]->runAction(Sequence::create(DelayTime::create(0.1f),Spawn::create(MoveBy::create(2.0f, Vec2(2000.0f, 0.0f)),MoveBy::create(0.5f, Vec2(0.0f, -300.0f)), NULL),CallFunc::create([this]{
                moveLeadActor();
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round047_04::fadeOutLayer), 3.0f);
            }), NULL));
        } else
        {
            m_pMSActors[i]->runAction(Sequence::create(RotateBy::create(0.1f, 0.0f,-180.0f),Spawn::create(MoveBy::create(2.0f, Vec2(2000.0f, 0.0f)),MoveBy::create(0.5f, Vec2(0.0f, -300.0f)), NULL), NULL));
        }
    }
}

/*
 *  庆祝
 */
void Round047_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round047_04::endRound() {}

//回合结束分享调用
void Round047_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.5f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.5f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    moveActors();

}

void Round047_04::moveLeadActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, 0.0f),true);
}

//渐变变黑退出
void Round047_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            }else
            {
                // 过关
                m_pStageScene->stageClear();
                //试玩时停止播放背景音乐
                if (m_pStageScene->isTrial())
                {
                    MyAudioEngine::getInstance()->stopAudio(0,true);
                }
            }
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round047_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round047_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round047_04::roundEndShareCallBack, this,true));
}
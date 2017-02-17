//
//  Round041_05.cpp
//  superBaby
//
//  Created by Administrator on 15/7/28.
//
//

#include "Round041_05.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "ThemeSelScene.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define MOVE_ACTOR_TIME                 3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                 2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define ANSWERS_ZORDER                  4
#define BRIDGE_PNG_FILE                 "GC-GK05-04-TM02.png"
#define BRIDGE_POS                      Vec2(1292,279)
#define BRIDGE_ZORDER                   1
#define HINT_PNG_FILE                   "GC-GK05-04-TM01.png"
#define HINT_POS                        Vec2(1239,274)
#define HINT_ZORDER                     2
#define HINT_LIGHT_PNG_FILE             "GC-GK05-04-TM04.png"

/*
 *  constructor
 */
Round041_05::Round041_05()
{
    m_answersPos[0] = Vec2(804, 896);
    m_answersPos[1] = Vec2(1182, 896);
    m_answersPos[2] = Vec2(1549, 896);

}

/*
 *  destructor
 */
Round041_05::~Round041_05()
{
    cleanUp();
}

//
void Round041_05::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round041_05* Round041_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round041_05();
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
bool Round041_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK05-04-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    m_pBridgeSp = Sprite::createWithSpriteFrameName(BRIDGE_PNG_FILE);
    m_pBridgeSp->setPosition(BRIDGE_POS);
    this->addChild(m_pBridgeSp, BRIDGE_ZORDER);
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    
    m_pHintLightSp = Sprite::createWithSpriteFrameName(HINT_LIGHT_PNG_FILE);
    m_pHintLightSp->setPosition(HINT_POS);
    m_pHintLightSp->setOpacity(0);
    this->addChild(m_pHintLightSp, HINT_ZORDER);

    return true;
}

void Round041_05::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round041_05::onEnterRound()
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
    
    changeScaleBridge();
}

void Round041_05::changeScaleBridge()
{
    m_pBridgeSp->runAction(Sequence::create(ScaleTo::create(0.5f, 1.1f),ScaleTo::create(0.5f, 1.0f), NULL));
    m_pHintSp->runAction(Sequence::create(ScaleTo::create(0.5f, 1.1f),ScaleTo::create(0.5f, 1.0f),CallFunc::create([this]{
        m_pHintLightSp->runAction(Sequence::create(FadeIn::create(0.5f),FadeOut::create(0.5f),CallFunc::create([this]{
            changeScaleAnswers();
        }), NULL));
    }), NULL));
}

//放大答案
void Round041_05::changeScaleAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i == 2) {
            m_pAnswersSp[i]->runAction(Sequence::create(ScaleTo::create(0.5f, 1.1f),ScaleTo::create(0.5f, 1.0f),CallFunc::create([this]{
                m_jumped = true;
                if (m_opentouched) {
                    openTouch();
                }
            }), NULL));
        } else
        {
            m_pAnswersSp[i]->runAction(Sequence::create(ScaleTo::create(0.5f, 1.1f),ScaleTo::create(0.5f, 1.0f), NULL));
        }
    }
}

/* 开启触摸 */
void Round041_05::openTouch()
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
            target->setLocalZOrder(10);
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
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
        auto index = (long)target->getUserData();
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp->getContentSize().width, m_pHintSp->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
                target->setPosition(m_pHintSp->getPosition());
                m_pHintSp->setVisible(false);
                
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
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
                celebrate(1.5f);
                upFadeOutAnswers();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
            }
        }else
        {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(ANSWERS_ZORDER);
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

void Round041_05::upFadeOutAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pAnswersSp[i]->runAction(Sequence::create(MoveBy::create(0.4f, Vec2(0.0f, 300.0f)),FadeOut::create(0.5f), NULL));
        }
    }
}

/*
 *  庆祝
 */
void Round041_05::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round041_05::endRound() {}

//回合结束分享调用
void Round041_05::roundEndShareCallBack(bool isShare)
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
    moveLeadActor();
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round041_05::fadeOutLayer), 3.0f);
}

void Round041_05::moveLeadActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, 0.0f),true);
}

//渐变变黑退出
void Round041_05::fadeOutLayer(float)
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
void Round041_05::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round041_05::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round041_05::roundEndShareCallBack, this,true));
}
//
//  Round029_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/4.
//
//

#include "Round029_04.h"
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
#define TOPIC_ZORDER                    2
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define HINT_POS                        Vec2(1222,981)
#define HINT_PNG_FILE                   "HY-GK05-4-lunkuo.png"
#define HINT_YELLOW_FILE                "HY-GK05-4-lunkuo2.png"
#define HINT_ZORDER                     3
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(1118,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(518,868)
#define WHITELIGHT_GAF_ZORDER           1

#define SCREW_PNG_FILE                  "HY-GK05-4-zhedang02.png"
#define SCREW_POS                       Vec2(2520,565)
#define SCREW_ZORDER                    2
#define DOOR_PNG_FILE                   "HY-GK05-4-IT01.png"
#define DOOR_POS                        Vec2(2373,365)
#define DOOR_ZORDER                     3
#define SHAOW_PNG_FILE                  "HY-GK05-4-mendi.png"
#define SHAOW_POS                       Vec2(2440,401)
#define SHAOW_ZORDER                    1
#define OVER_PNG_FILE                   "HY-GK05-4-zhedang.png"
#define OVER_POS                        Vec2(12921,565)
#define OVER_TAG                        203

/*
 *  constructor
 */
Round029_04::Round029_04()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(866, 429);
    m_answersPos[1] = Vec2(1170, 429);
    m_answersPos[2] = Vec2(1449, 429);
    
    m_topicsPos[0] = Vec2(370, 981);
    m_topicsPos[1] = Vec2(670, 981);
    m_topicsPos[2] = Vec2(934, 981);
    m_topicsPos[3] = Vec2(1510, 981);
    m_topicsPos[4] = Vec2(1754, 981);
}

/*
 *  destructor
 */
Round029_04::~Round029_04()
{
    cleanUp();
}

//
Round029_04* Round029_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round029_04();
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
bool Round029_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pPaopaoActor = Actor::createWithGafFile(PAOPAO_GAF_FILE);
    m_pPaopaoActor->setPosition(PAOPAO_GAF_POS);
    this->addChild(m_pPaopaoActor, PAOPAO_GAF_ZORDER);
    
    m_pWhiteLightActor = Actor::createWithGafFile(WHITELIGHT_GAF_FILE);
    m_pWhiteLightActor->setPosition(WHITELIGHT_GAF_POS);
    m_pWhiteLightActor->setSize(Size(300, 300));
    m_pWhiteLightActor->setPosAtNode(Vec2(-300, 400));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    
    char buffer[64];
    for (unsigned long i = 0; i < 5; i++) {
        if (i == 0) {
            sprintf(buffer, "HY-GK05-4-TM01.png");
        }else
        {
            sprintf(buffer, "HY-GK05-4-TM02.png");
        }
        m_pTopicSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicSp[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicSp[i], TOPIC_ZORDER);
        
        if (i%2 == 0) {
            m_pTopicSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
        }else
        {
            m_pTopicSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    
    m_pHintYellowSp = Sprite::createWithSpriteFrameName(HINT_YELLOW_FILE);
    m_pHintYellowSp->setPosition(HINT_POS);
    this->addChild(m_pHintYellowSp, HINT_ZORDER);
    m_pHintYellowSp->setOpacity(0);
    
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 2) {
            sprintf(buffer, "HY-GK05-4-DA01.png");
        }else
        {
            sprintf(buffer, "HY-GK05-4-TM%02ld.png",i+1);
        }
        
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        if (i%2 == 0) {
            m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else
        {
            m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
        }
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
    
    m_pScrewSp = Sprite::createWithSpriteFrameName(SCREW_PNG_FILE);
    m_pScrewSp->setPosition(SCREW_POS);
    this->addChild(m_pScrewSp, SCREW_ZORDER);
    
    m_pDoorSp = Sprite::createWithSpriteFrameName(DOOR_PNG_FILE);
    m_pDoorSp->setPosition(DOOR_POS);
    this->addChild(m_pDoorSp, DOOR_ZORDER);
    
    m_pShadowSp = Sprite::createWithSpriteFrameName(SHAOW_PNG_FILE);
    m_pShadowSp->setPosition(SHAOW_POS);
    this->addChild(m_pShadowSp, SHAOW_ZORDER);
    
    m_pOverSp = Sprite::createWithSpriteFrameName(OVER_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pOverSp, OVER_POS, OVER_TAG);
    
    return true;
}

void Round029_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round029_04::onEnterRound()
{
    log("this pos X == %f",this->getPosition().x);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
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
    changeScaleHint();
}

//放大答案以提示
void Round029_04::changeScaleHint()
{
    m_pHintYellowSp->runAction(Sequence::create(FadeIn::create(0.4f),
                                                FadeOut::create(0.4f),
                                                FadeIn::create(0.4f),
                                                FadeOut::create(0.4f),
                                                CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    })
                                                , NULL));
}

//
void Round029_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(PAOPAO_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(WHITELIGHT_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOOR_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(SHAOW_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(OVER_PNG_FILE);
}

/* 开启触摸 */
void Round029_04::openTouch()
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
            target->setLocalZOrder(5);
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
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintSp->getContentSize().width,
                         m_pHintSp->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 0) {
                m_pHintSp->setVisible(false);
                target->setPosition(HINT_POS);
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
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
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_answersEndPos[index]);
        target->setLocalZOrder(ANSWERS_ZORDER);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//答对移动鱼群
void Round029_04::moveFishs()
{
    for (unsigned int i = 0; i < 5; i++) {
        m_pTopicSp[i]->runAction(MoveBy::create(3.0f, Vec2(-2000.0f, 0.0f)));
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswersSp[i]->runAction(MoveBy::create(3.0f, Vec2(-2000.0f, 0.0f)));
    }
}

/*
 *  庆祝
 */
void Round029_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round029_04::endRound() {}

/*
 *  分享界面点击ok后移动主角
 */
void Round029_04::moveActor()
{
    m_pStageScene->manualScroll(800, [this]{
        openDoor();
    });
}

void Round029_04::openDoor()
{
    m_pDoorSp->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(m_pDoorSp->getContentSize().width + 16.0f, 0.0f)), Hide::create(), CallFunc::create([this]{
        m_pStageScene->getBeibeiActor()->walk(); m_pStageScene->getAimeeActor()->walk();
        m_pStageScene->moveLeadingActor(3.0f, Vec2(2000.0f, 0.0f));
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round029_04::fadeOutLayer), 2.0f);
    }), NULL));
}

//回合结束分享调用
void Round029_04::roundEndShareCallBack(bool isShare)
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
    moveFishs();
    scheduleOnce([this](float){
        moveActor();
    }, 2.0f, "delaytime");
}

//渐变变黑退出
void Round029_04::fadeOutLayer(float)
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
void Round029_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round029_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round029_04::roundEndShareCallBack, this,true));
}
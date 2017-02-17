//
//  Round027_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/2.
//
//

#include "Round027_04.h"
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
#define MS_GAF_CONFIG_FILE              "res/stage027/ms.json"
#define MS_GAF_FILE                     "res/stage027/HY-GK05-2-MS01-D+C/HY-GK05-2-MS01-D+C.gaf"
#define MS_POS                          Vec2(1037,569)
#define MS_ZORDER                       1
#define SHIP_GAF_FILE                   "res/stage025/HY-GK01-1-chuan-D/HY-GK01-1-chuan-D.gaf"
#define SHIP_POS                        Vec2(2434,1250)
#define SHIP_ZORDER                     1
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(224,1126)
#define BOGUANG_GAF_ZORDER              1
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define MERMAID_GAF_CONFIG_FILE         "res/stage027/mermaid.json"
#define MERMAID_GAF_FILE                "res/stage027/HY-GK03-4-meirenyu-C1+C2/HY-GK03-4-meirenyu-C1+C2.gaf"
#define MERMAID_GAF_POS                 Vec2(2048,1000)
#define MERMAID_ZORDER                  5
#define HINT_POS                        Vec2(1305,703)
#define HINT_PNG_FILE                   "HY-GK03-4-DA03.png"
/*
 *  constructor
 */
Round027_04::Round027_04()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(854, 229);
    m_answersPos[1] = Vec2(1032, 229);
    
    m_topicsPos[0] = Vec2(870, 749);
    m_topicsPos[1] = Vec2(974, 712);
    m_topicsPos[2] = Vec2(1084, 676);
    m_topicsPos[3] = Vec2(1200, 676);
    m_topicsPos[4] = Vec2(1305, 703);
    m_topicsPos[5] = Vec2(1400, 736);
    m_topicsPos[6] = Vec2(1500, 794);
}

/*
 *  destructor
 */
Round027_04::~Round027_04()
{
    cleanUp();
}

//
Round027_04* Round027_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round027_04();
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
bool Round027_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    m_pShipActor = Actor::createWithGafFile(SHIP_GAF_FILE);
    m_pShipActor->setPosition(SHIP_POS);
    this->addChild(m_pShipActor, SHIP_ZORDER);
    m_pShipActor->setRotationSkewY(180);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);
    
    srand(unsigned(time(0)));
    m_randNumber = rand()%2;
    
    char buffer[64];
    for (unsigned long i = 0; i < 7; i++) {
        if (m_randNumber == 0) {
            if (i%2 == 0) {
                sprintf(buffer, "HY-GK03-4-DA01.png");
            }else
            {
                sprintf(buffer, "HY-GK03-4-DA02.png");
            }
        }else
        {
            if (i%2 == 0) {
                sprintf(buffer, "HY-GK03-4-DA02.png");
            }else
            {
                sprintf(buffer, "HY-GK03-4-DA01.png");
            }
        }
        m_pTopicSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicSp[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicSp[i], TOPIC_ZORDER);
        if (i == 4) {
            m_pTopicSp[i]->setVisible(false);
        }
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, TOPIC_ZORDER);
    
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "HY-GK03-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->setScale(0.0f);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[all] = m_answersPos[randIndex];
        all++;
    } /*while*/
    
    m_pMermaidActor = Actor::createWithGafConfigFile(MERMAID_GAF_CONFIG_FILE);
    m_pMermaidActor->setPosition(MERMAID_GAF_POS);
    this->addChild(m_pMermaidActor, MERMAID_ZORDER);
    
    return true;
}

void Round027_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round027_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round027_04::changeScaleAnswersSchedule), 0.3f, -1, 0);
}

//海星变大出现
void Round027_04::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round027_04::changeScaleAnswersSchedule));
        changeScaleHint();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.5f)));
}

//放大答案以提示
void Round027_04::changeScaleHint()
{
    m_pHintSp->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),
                                          ScaleTo::create(0.3f, 1.0f),
                                          ScaleTo::create(0.3f, 1.1f),
                                          ScaleTo::create(0.3f, 1.0f),
                                          CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    })
                                          , NULL));
}

//
void Round027_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(SHIP_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(BOGUANG_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(MERMAID_GAF_FILE);
}

/* 开启触摸 */
void Round027_04::openTouch()
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
            target->setScale(1.4f);
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
        target->setScale(1.5f);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintSp->getContentSize().width,
                         m_pHintSp->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == m_randNumber) {
                target->setScale(1.0f);
                m_pHintSp->setVisible(false);
                target->setPosition(HINT_POS);
                target->setVisible(false);
                m_pTopicSp[4]->setVisible(true);
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
                changeActor();
                fadeAnswers();
                scheduleOnce([this](float){
                    m_InitAniIndex = 0;
                    this->schedule(CC_SCHEDULE_SELECTOR(Round027_04::changeScaleTopicSchedule), 0.2f, -1, 0);
                }, 1.0f, "delaytime");
                for (unsigned int i = 0; i < 2; i++) {
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
    for (unsigned int i = 1; i < 2; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//移动海龟
void Round027_04::moveMS()
{
    m_pMSActor->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(200, 60)),CallFunc::create([this]{
        moveActor();
    }), NULL));
}

void Round027_04::changeScaleTopicSchedule(float)
{
    if (m_InitAniIndex >= 7) {
        unschedule(CC_SCHEDULE_SELECTOR(Round027_04::changeScaleTopicSchedule));
        return;
    }
    m_pTopicSp[m_InitAniIndex++]->runAction(ScaleTo::create(0.2f, 0.0f));
}

//隐藏剩余答案
void Round027_04::fadeAnswers()
{
    for (unsigned int i = 0; i < 2; i++) {
        if (i != m_randNumber) {
            m_pAnswersSp[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

//答对改变ms
void Round027_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER,true);
}

/*
 *  庆祝
 */
void Round027_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round027_04::endRound() {}

/*
 *  分享界面点击ok后移动主角
 */
void Round027_04::moveActor()
{

    m_pStageScene->manualScroll(550, [this]{
        m_pMermaidActor->playWithSequenceAliasName(STANDBY);
        m_pMermaidActor->setAnimationFinishedPlayDelegate([this]{
            m_pStageScene->getBeibeiActor()->walk(); m_pStageScene->getAimeeActor()->walk();
            m_pStageScene->moveLeadingActorCB(MOVE_ACTOR_TIME, Vec2(980, 0),[this]{
                m_pStageScene->setVisibleAimiActor(false);
                m_pStageScene->setVisibleBeibeiActor(false);
                m_pMermaidActor->playWithSequenceAliasName(TRIGGER);
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round027_04::fadeOutLayer), 3.0f);
            });
        });
    });
}

//回合结束分享调用
void Round027_04::roundEndShareCallBack(bool isShare)
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
    moveMS();
}

//渐变变黑退出
void Round027_04::fadeOutLayer(float)
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
void Round027_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round027_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round027_04::roundEndShareCallBack, this,true));
}
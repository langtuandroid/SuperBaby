//
//  Round048_04.cpp
//  superBaby
//
//  Created by Administrator on 15/8/6.
//
//

#include "Round048_04.h"
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
//#include "adjustPosNode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                            // 答错回原位置移动时间
#define MOVE_ACTOR_TIME                 3.0f                                            //分享界面ok过后主角移动时间
#define COLORLAYER_TAG                  300
#define STANDBYONE                      "standbyone"                                   // 待机动画
#define STANDBYTWO                      "standbytwo"                                   // 待机动画
#define TRIGGERONE                      "triggerone"                                    // 触发动画
#define TRIGGERTWO                      "triggertwo"                                    // 触发动画
#define TRIGGERTHREE                    "triggerthree"                                    // 触发动画
#define TRIGGERFOUR                     "triggerfour"                                    // 触发动画
#define ANSWERS_ZORDER                  3
#define MS_GAF_CONFIG_FILE              "res/stage048/ms_01.json"
#define MS_GAF_FILE                     "res/stage048/GC-GK-12-4-TM-C1+D1+C2+D2/GC-GK-12-4-TM-C1+D1+C2+D2.gaf"
#define MS_POS                          Vec2(1534,1158)
#define MS_ZORDER                       2
#define GOOD_GAF_CONFIG_FILE            "res/stage048/ms_02.json"
#define GOOD_GAF_FILE                   "res/stage048/GC-GK12-SJ-C1+C2+C3/GC-GK12-SJ-C1+C2+C3.gaf"
#define GOOD_POS                        Vec2(1292,401)
#define GOOD_ZORDER                     4
#define ROCK_PNG_FILE                   "res/stage048/GC-GK12-4-IT04.png"
#define ROCK_POS                        Vec2(1646,738)
#define ROCK_ZORDER                     1
#define REPEAT_TAG                      102

/*
 *  constructor
 */
Round048_04::Round048_04()
{
    m_answersPos[0] = Vec2(706, 280);
    m_answersPos[1] = Vec2(973, 280);
    m_answersPos[2] = Vec2(1240, 280);
    
    m_answersRect[0] = Rect(459, 795, 210, 180);
    m_answersRect[1] = Rect(539, 994, 210, 180);
    
    m_endPos[0] = Vec2(511, 868);
    m_endPos[1] = Vec2(597, 1084);
}

/*
 *  destructor
 */
Round048_04::~Round048_04()
{
    cleanUp();
}

//
Round048_04* Round048_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round048_04();
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
bool Round048_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK12-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setOpacity(0);
        auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.6f, Vec2(0.0f, -10.f)),MoveBy::create(0.6f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEAT_TAG);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
        m_pAnswersSp[i]->runAction(repeat);
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
    
    m_pRockSp = Sprite::create(ROCK_PNG_FILE);
    m_pRockSp->setPosition(ROCK_POS);
    this->addChild(m_pRockSp, ROCK_ZORDER);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    m_pMSActor->setPosition(MS_POS);
    m_pMSActor->setScale(1.5f);
    this->addChild(m_pMSActor, MS_ZORDER);

    return true;
}

void Round048_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

void Round048_04::drawtest(const Rect& rect)
{
    DrawNode* drawNode = DrawNode::create();
    this->addChild(drawNode,50);
    Vec2 point[4];
    point[0] = Vec2(rect.origin.x, rect.origin.y);
    point[1] = Vec2(rect.origin.x, rect.origin.y + rect.size.height);
    point[2] = Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
    point[3] = Vec2(rect.origin.x + rect.size.width, rect.origin.y);
    drawNode->drawPolygon(point, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 1, 0, 1));
//    auto anode = adjustPosNode::createWithNode(drawNode);
//    addChild(anode, 100);
}


// 进入本回合
void Round048_04::onEnterRound()
{
//    drawtest(Rect(459, 795, 210, 180));
//    drawtest(Rect(492, 979, 210, 180));
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
    
    m_pMSActor->playWithSequenceAliasName(TRIGGERONE);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        m_pMSActor->playWithSequenceAliasName(STANDBYONE);
    });
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round048_04::fadeInAnswersSchedule), 0.4f, -1, 0);   //
}

//渐显答案
void Round048_04::fadeInAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round048_04::fadeInAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(FadeIn::create(0.4f));
}

//
void Round048_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(MS_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(GOOD_GAF_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(ROCK_PNG_FILE);
}

/* 开启触摸 */
void Round048_04::openTouch()
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
        target->setScale(1.0f);
        bool isCorrect = false;
        int bubbleIndex = 0;
        if (m_answersRect[0].containsPoint(this->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 2) {
                isCorrect = true;
                bubbleIndex = 0;
            }
        }
        if (m_answersRect[1].containsPoint(this->convertTouchToNodeSpace(touch))) {
            if (! isCorrect && index == 1) {
                isCorrect = true;
                bubbleIndex = 1;
            }
        }

        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_endPos[bubbleIndex]);
            target->runAction(FadeOut::create(0.2f));
            if (m_curCorrectNum < 2) {
                if (bubbleIndex == 0) {
                    m_pMSActor->playWithSequenceAliasName(TRIGGERFOUR);
                } else if (bubbleIndex == 1)
                {
                    m_pMSActor->playWithSequenceAliasName(TRIGGERTHREE);
                }
            }
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 2) {
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    if (i == 0) {
                        m_pAnswersSp[i]->runAction(FadeOut::create(0.2f));
                    } else
                    {
                        m_pAnswersSp[i]->setVisible(false);
                    }
                    
                }
                changeActor();
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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

void Round048_04::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGERTWO);
    m_pMSActor->setAnimationFinishedPlayDelegate([this]{
        m_pMSActor->playWithSequenceAliasName(STANDBYTWO);
    });
}

/*
 *  庆祝
 */
void Round048_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round048_04::endRound() {}

//回合结束分享调用
void Round048_04::roundEndShareCallBack(bool isShare)
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
    collectGood();
}

void Round048_04::collectGood()
{
    auto goodActor = Actor::createWithGafConfigFile(GOOD_GAF_CONFIG_FILE);
    std::vector<std::string> sequenceAliasName;
    sequenceAliasName.push_back(TRIGGERONE);
    sequenceAliasName.push_back(TRIGGERTWO);
    sequenceAliasName.push_back(TRIGGERTHREE);
    goodActor->playWithSequenceAliasName(sequenceAliasName,false,1,false,[this]{
        moveActor();
        scheduleOnce(CC_SCHEDULE_SELECTOR(Round048_04::fadeOutLayer), 3.0f);
    });
    goodActor->setPosition(GOOD_POS);
    this->addChild(goodActor,GOOD_ZORDER);
}

/*
 *  分享界面点击ok后移动主角
 */
void Round048_04::moveActor()
{
    m_pStageScene->moveLeadingActor(4.0f, Vec2(2000.0f, -130.0f),true);
}

//渐变变黑退出
void Round048_04::fadeOutLayer(float)
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
void Round048_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round048_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round048_04::roundEndShareCallBack, this,true));
}
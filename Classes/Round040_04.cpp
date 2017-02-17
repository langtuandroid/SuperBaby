//
//  Round040_04.cpp
//  superBaby
//
//  Created by Administrator on 15/7/24.
//
//

#include "Round040_04.h"
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
#define TOPICS_ZORDER                   2
#define ANSWER_POS                      Vec2(1465, 971)
#define STAGE_PNG_FILE                  "GC-GK04-4-IT01.png"
#define STAGE_POS                       Vec2(2455,144)
#define GRAPH_ONE_PNG_FILE              "GC-GK04-4-DA03.png"
#define GRAPH_TWO_PNG_FILE              "GC-GK04-4-DA01.png"


/*
 *  constructor
 */
Round040_04::Round040_04()
{
    m_answersPos[0] = Vec2(830, 314);
    m_answersPos[1] = Vec2(1134, 314);
    m_answersPos[2] = Vec2(1438, 314);
    
    m_topicsPos[0] = Vec2(817, 842);
    m_topicsPos[1] = Vec2(1466, 842);
    
    m_targetsPos[0] = Vec2(277, 352);
    m_targetsPos[1] = Vec2(265, 338);
    
}

/*
 *  destructor
 */
Round040_04::~Round040_04()
{
    cleanUp();
}

//
void Round040_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round040_04* Round040_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round040_04();
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
bool Round040_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK04-4-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "GC-GK04-4-TM%02d.png",i+3);
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        this->addChild(m_pTopicsSp[i], TOPICS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "GC-GK04-4-TM%02d.png",i+1);
        m_pTargetsImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pTargetsImageView[i]->setPosition(m_targetsPos[i]);
        m_pTopicsSp[i]->addChild(m_pTargetsImageView[i]);
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
    
    all = 0;
    bool selectedTopic[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selectedTopic[randIndex]) continue;
        selectedTopic[randIndex] = true;
        m_pTopicsSp[all]->setPosition(m_topicsPos[randIndex]);
        all++;
    } /*while*/
    
    m_pStageSp = Sprite::createWithSpriteFrameName(STAGE_PNG_FILE);
    m_pStageSp->setPosition(STAGE_POS);
    this->addChild(m_pStageSp);

    return true;
}

void Round040_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round040_04::onEnterRound()
{
    log("this is pos  X == %f",this->getPositionX());
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round040_04::changeScaleAnswersSchedule), 0.6f, -1, 0);   //
}

//改变答案大小
void Round040_04::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round040_04::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round040_04::changeScaleTopicSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}
//改变题目大小
void Round040_04::changeScaleTopicSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round040_04::changeScaleTopicSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicsSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round040_04::openTouch()
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        
        bool isCorrect = false;
        unsigned int topicNum = 0;
        Rect rect = Rect(0.0f, 0.0f,
                         m_pTargetsImageView[0]->getContentSize().width,
                         m_pTargetsImageView[0]->getContentSize().height);
        if (rect.containsPoint(m_pTargetsImageView[0]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 2)
            {
                topicNum = 0;
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pTargetsImageView[1]->getContentSize().width,
                    m_pTargetsImageView[1]->getContentSize().height);
        if (rect.containsPoint(m_pTargetsImageView[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 0)
            {
                topicNum = 1;
                isCorrect = true;
            }
        }
        
        if (isCorrect) {
            m_curCorrectNum++;
            target->runAction(FadeOut::create(0.2f));
            if (topicNum == 0) {
                m_pTargetsImageView[topicNum]->loadTexture(GRAPH_ONE_PNG_FILE,ui::Widget::TextureResType::PLIST);
            }else
            {
                m_pTargetsImageView[topicNum]->loadTexture(GRAPH_TWO_PNG_FILE,ui::Widget::TextureResType::PLIST);
            }
            m_pTopicsSp[topicNum]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.0f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
            target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.15f, 1.0f), NULL));
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (m_curCorrectNum == 2) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                fadeOutAnswers();
            }else
            {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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

void Round040_04::fadeOutAnswers()
{
    m_pAnswersSp[1]->runAction(FadeOut::create(0.3f));
}

/*
 *  庆祝
 */
void Round040_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round040_04::endRound() {}

//回合结束分享调用
void Round040_04::roundEndShareCallBack(bool isShare)
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
}

void Round040_04::moveLeadActor()
{
    m_pStageScene->manualScroll(800, [this]{
        m_pStageScene->moveLeadingActorCB(3.0f, Vec2(1200, 0.0f),[this]{
            m_pStageScene->moveLeadingActor(4.0f, Vec2(0.0f, 1700.0f));
            m_pStageSp->runAction(MoveBy::create(4.0f, Vec2(0.0f, 1700.0f)));
            scheduleOnce(CC_SCHEDULE_SELECTOR(Round040_04::fadeOutLayer), 3.0f);
        },true);
    });
}

//渐变变黑退出
void Round040_04::fadeOutLayer(float)
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
void Round040_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round040_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round040_04::roundEndShareCallBack, this,true));
}
//
//  Round046_01.cpp
//  superBaby
//
//  Created by Administrator on 15/8/4.
//
//

#include "Round046_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage046/GC-GK10-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage046/GC-GK10-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间
#define ANSWERS_ZORDER                  2
#define TOPICS_ZORDER                   1
#define TOPIC_LIGHT_PNG_FILE            "GC-GK10-1-TM04-C.png"
#define REPEAT_TAG                      102

/*
 *  constructor
 */
Round046_01::Round046_01()
{
    m_answersPos[0] = Vec2(741, -100);
    m_answersPos[1] = Vec2(1014, -100);
    m_answersPos[2] = Vec2(1287, -100);
    
    m_topicsPos[0] = Vec2(309,817);
    m_topicsPos[1] = Vec2(641,753);
    m_topicsPos[2] = Vec2(882,685);
    m_topicsPos[3] = Vec2(1154,753);
    m_topicsPos[4] = Vec2(1413,817);
    m_topicsPos[5] = Vec2(1713,897);
}
/*
 *  destructor
 */
Round046_01::~Round046_01()
{
    cleanUp();
}

//
void Round046_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round046_01* Round046_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round046_01();
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
bool Round046_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK10-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 6; i++) {
        sprintf(buffer, "GC-GK10-1-TM%02d.png",i+1);
        m_pTopicsImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pTopicsImageView[i]->setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicsImageView[i],TOPICS_ZORDER);
        auto repeat = RepeatForever::create(Sequence::create(MoveBy::create(0.6f, Vec2(0.0f, -10.f)),MoveBy::create(0.6f, Vec2(0.0f, 10.0f)), NULL));
        repeat->setTag(REPEAT_TAG);
        m_pTopicsImageView[i]->runAction(repeat);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

// 进入本回合
void Round046_01::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round046_01::upAnswersSchedule), 0.8f, -1, 0);   //
}

//依次上升答案
void Round046_01::upAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round046_01::upAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, 300.0f))));
}

/* 开启触摸 */
void Round046_01::openTouch()
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
            if (index == 0) {
                m_pTopicsImageView[4]->loadTexture(TOPIC_LIGHT_PNG_FILE,ui::Widget::TextureResType::PLIST);
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
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
}
//下降答案
void Round046_01::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round046_01::downAnswersSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(0.5f, Vec2(0.0f, -300.0f))));
}

//上升气球
void Round046_01::upTopics()
{
    for (unsigned int i = 0; i < 6; i++) {
        m_pTopicsImageView[i]->stopActionByTag(REPEAT_TAG);
        m_pTopicsImageView[i]->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500.0f)));
    }
}

/*
 *  庆祝
 */
void Round046_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round046_01::endRound()
{
    m_InitAniIndex = 0;
    this->schedule(CC_SCHEDULE_SELECTOR(Round046_01::downAnswersSchedule), 0.8f, -1, 0);   //
    upTopics();
}

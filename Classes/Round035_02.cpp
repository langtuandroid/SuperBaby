//
//  Round035_02.cpp
//  superBaby
//
//  Created by Administrator on 15/7/6.
//
//

#include "Round035_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(1118,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1

#define ANSWERS_ZORDER                 4

/*
 *  constructor
 */
Round035_02::Round035_02()
{
    
    m_answersPos[0] = Vec2(888, 958);
    m_answersPos[1] = Vec2(986, 578);
    m_answersPos[2] = Vec2(1482, 896);
    
}

/*
 *  destructor
 */
Round035_02::~Round035_02()
{
    cleanUp();
}

//
void Round035_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage035/HY-GK11-1-MS01/HY-GK11-1-MS01.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage035/HY-GK11-1-MS02/HY-GK11-1-MS02.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage035/HY-GK11-1-MS03/HY-GK11-1-MS03.gaf");
}

//
Round035_02* Round035_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round035_02();
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
bool Round035_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    m_pWhiteLightActor->setPosAtNode(Vec2(-151, 747));
    this->addChild(m_pWhiteLightActor, WHITELIGHT_GAF_ZORDER);
    
    char buffer[64];
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "res/stage035/HY-GK11-1-MS%02ld/HY-GK11-1-MS%02ld.gaf",i+1,i+1);
        m_pAnswersActor[i] = Actor::createWithGafFile(buffer);
        if (i == 0) {
            m_pAnswersActor[i]->setSize(Size(400, 300));
            m_pAnswersActor[i]->setPosAtNode(Vec2(-58, 345));
        }else if (i == 1)
        {
            m_pAnswersActor[i]->setSize(Size(350, 280));
            m_pAnswersActor[i]->setPosAtNode(Vec2(-82, 315));
        }else
        {
            m_pAnswersActor[i]->setSize(Size(300, 200));
            m_pAnswersActor[i]->setPosAtNode(Vec2(-126, 305));
        }
        m_pAnswersActor[i]->setUserData((void*)i);
        m_pAnswersActor[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        this->addChild(m_pAnswersActor[i], ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersActor[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round035_02::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round035_02::changeScaleAnswersSchedule), 0.8f, -1, 0);   //
}

//依次放大答案以提示
void Round035_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round035_02::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f), NULL));
}

/* 开启触摸 */
void Round035_02::openTouch()
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
        auto index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 2) {
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersActor[i]);
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
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersActor[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersActor[i]);
    } /*for*/
}

//答对后移动鱼
void Round035_02::moveFishes()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i == 2) {
            m_pAnswersActor[i]->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(-2000.0f, 0.0f)),CallFunc::create([this]{
                if (m_completeCB) {
                    m_completeCB();
                }
            }), NULL));
        }else
        {
            m_pAnswersActor[i]->runAction(MoveBy::create(2.0f, Vec2(-2000.0f, 0.0f)));
        }
    }
}

/*
 *  庆祝
 */
void Round035_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round035_02::endRound()
{
    moveFishes();
}
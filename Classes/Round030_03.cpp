//
//  Round030_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round030_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define SHELLS_ZORDER                     3
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(1218,1068)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1

/*
 *  constructor
 */
Round030_03::Round030_03()
{
    
    m_shellsPos[0] = Vec2(879, 987);
    m_shellsPos[1] = Vec2(1000, 463);
    m_shellsPos[2] = Vec2(1234, 1043);
    m_shellsPos[3] = Vec2(1285, 683);
    m_shellsPos[4] = Vec2(1641, 683);
}

/*
 *  destructor
 */
Round030_03::~Round030_03()
{
    cleanUp();
}

//
void Round030_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round030_03* Round030_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round030_03();
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
bool Round030_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    for (unsigned long i = 0; i < 5; i++) {
        if (i == 0 || i == 4) {
            sprintf(buffer, "HY-GK06-2-DA01.png");
        }else
        {
            sprintf(buffer, "HY-GK06-2-DA%02ld.png",i+1);
        }
        m_pShellsImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pShellsImageView[i]->setUserData((void*)i);
        this->addChild(m_pShellsImageView[i], SHELLS_ZORDER);
        if (i == 0) {
            m_pShellsImageView[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(10.0f, -10.f)),MoveBy::create(0.5f, Vec2(-10.0f, 10.0f)), NULL)));
        }else if (i == 1)
        {
            m_pShellsImageView[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else if (i == 2)
        {
            m_pShellsImageView[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(-10.0f, -10.f)),MoveBy::create(0.5f, Vec2(10.0f, 10.0f)), NULL)));
        }else if (i == 3)
        {
            m_pShellsImageView[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        }else
        {
            m_pShellsImageView[i]->setRotationSkewY(-180);
            m_pShellsImageView[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(-10.0f, -10.f)),MoveBy::create(0.5f, Vec2(10.0f, 10.0f)), NULL)));
        }
    }
    
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 5);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pShellsImageView[all]->setPosition(m_shellsPos[randIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round030_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round030_03::changeScaleshellsSchedule), 0.6f, -1, 0);   //
}

//依次放大船以提示
void Round030_03::changeScaleshellsSchedule(float)
{
    if (m_InitAniIndex >= 5) {
        unschedule(CC_SCHEDULE_SELECTOR(Round030_03::changeScaleshellsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pShellsImageView[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round030_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
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
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (0 == index || index == 4) {   //答对
                target->loadTexture("HY-GK06-2-DA05.png",ui::Widget::TextureResType::PLIST);
                m_curCorrectNum++;
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                if (m_curCorrectNum == 2) {
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    celebrate(CELEBRATE_DELAY);
                    for (unsigned int i = 0; i < 5; i++) {
                        if (i != 0 || i != 4) {
                            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pShellsImageView[i]);
                        }
                    } /*for*/
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
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pShellsImageView[0]);
    for (unsigned int i = 1; i < 5; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pShellsImageView[i]);
    } /*for*/
}

//答对移动鱼
void Round030_03::moveShells()
{
    for (unsigned int i = 0; i < 5; i++) {
        if (i == 0 || i == 4) {
            m_pShellsImageView[i]->runAction(ScaleTo::create(0.5f, 0.0f));
        }else
        {
            m_pShellsImageView[i]->runAction(MoveBy::create(2.0f, Vec2(0.0f, 1500.0f)));
        }
    }
    scheduleOnce([this](float){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 2.0f, "delay");
}

/*
 *  庆祝
 */
void Round030_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round030_03::endRound()
{
    moveShells();
}
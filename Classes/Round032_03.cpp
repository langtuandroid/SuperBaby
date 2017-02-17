//
//  Round032_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#include "Round032_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(1218,1068)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(872,795)
#define WHITELIGHT_GAF_ZORDER           1
#define ANSWERS_ZORDER                  3
#define CORAL_PNG_FILE                  "HY-GK08-3-IT.png"
#define CORAL_POS                       Vec2(1268,403)
#define CORAL_ZORDER                    4

/*
 *  constructor
 */
Round032_03::Round032_03() {
    
    m_answersPos[0] = Vec2(732, 623);
    m_answersPos[1] = Vec2(1052, 623);
    m_answersPos[2] = Vec2(1385, 623);
    m_answersPos[3] = Vec2(1715, 623);
}

/*
 *  destructor
 */
Round032_03::~Round032_03()
{
    cleanUp();
}

//
void Round032_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    char buffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "res/stage032/HY-GK08-3-DA%02d.png",i+1);
        Director::getInstance()->getTextureCache()->removeTextureForKey(buffer);
    }
    
    CC_SAFE_DELETE(m_pCoralImage);
    for (unsigned int i = 0; i < 4; i++) {
        CC_SAFE_DELETE(m_pAnswersImage[i]);
    }
}

//
Round032_03* Round032_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round032_03();
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
bool Round032_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    m_pCoralsSp = Sprite::createWithSpriteFrameName(CORAL_PNG_FILE);
    m_pCoralsSp->setPosition(CORAL_POS);
    this->addChild(m_pCoralsSp, CORAL_ZORDER);
    
    m_pCoralImage = new Image();
    m_pCoralImage->initWithImageFile("res/stage032/HY-GK08-3-IT.png");
    
    srand(unsigned(time(0)));
    m_curRandNum = rand()%3;
    
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        if (3 == i) {
            sprintf(buffer, "HY-GK08-3-DA%02ld.png",(long)m_curRandNum+1);
        }else
        {
            sprintf(buffer, "HY-GK08-3-DA%02ld.png",i+1);
        }
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
        m_pAnswersSp[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 10.f)),MoveBy::create(0.5f, Vec2(0.0f, -10.0f)), NULL)));
    }
    
    for (int i = 0; i < 4; i++) {
        if (3 == i) {
            sprintf(buffer, "res/stage032/HY-GK08-3-DA%02d.png",m_curRandNum+1);
        }else
        {
            sprintf(buffer, "res/stage032/HY-GK08-3-DA%02d.png",i+1);
        }
        m_pAnswersImage[i] = new Image();
        m_pAnswersImage[i]->initWithImageFile(buffer);
    }
    
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 4);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round032_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round032_03::changeScaleAnswersSchedule), 1.2f, -1, 0);
}

//改变答案大小以提示
void Round032_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round032_03::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round032_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        Rect rect = Rect(0.0f, 0.0f, m_pCoralsSp->getContentSize().width, m_pCoralsSp->getContentSize().height);
        Vec2 touchPoint = target->convertTouchToNodeSpace(touch);
        if (rect.containsPoint(m_pCoralsSp->convertTouchToNodeSpace(touch)))
        {
            unsigned char *data = m_pCoralImage->getData();
            Vec2 touchPointInSprite = m_pCoralsSp->convertTouchToNodeSpace(touch);
            
            int pa = 4*((m_pCoralImage->getHeight() - (int)(touchPointInSprite.y) - 1)*m_pCoralImage->getWidth() + (int)(touchPointInSprite.x)) + 3;
            unsigned int ap = data[pa];
            if (ap > 0) {
                return false;
            }
        }
        for (unsigned int i = 0; i < 4; i++) {
            if (m_pAnswersSp[i]->getBoundingBox().containsPoint(touchPoint)) {
                unsigned char *data = m_pAnswersImage[i]->getData();
                Vec2 touchPointInSprite = m_pAnswersSp[i]->convertTouchToNodeSpace(touch);
                
                int pa = 4*((m_pAnswersImage[i]->getHeight() - (int)(touchPointInSprite.y) - 1)*m_pAnswersImage[i]->getWidth() + (int)(touchPointInSprite.x)) + 3;
                unsigned int ap = data[pa];
                if (ap > 0) {
                    m_curSelIndex = i;
                    if (m_overedIndex != m_curSelIndex) {
                        m_pAnswersSp[i]->setScale(1.1f);
                    }
                    return true;
                }else
                {
                    continue;
                }
            }
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        m_pAnswersSp[m_curSelIndex]->setScale(1.0f);
        if (m_overedIndex != m_curSelIndex) {
            if (m_curSelIndex == m_curRandNum || m_curSelIndex == 3) {
                m_overedIndex = m_curSelIndex;
                m_pAnswersSp[m_curSelIndex]->runAction(MoveBy::create(0.5f, Vec2(0.0f, 30.0f)));
                m_curCorrectNumer++;
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                if (m_curCorrectNumer == 2) {
                    m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                    m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                    celebrate(CELEBRATE_DELAY);
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
                }else
                {
                    if (m_celebrateCB) {
                        m_celebrateCB(1);
                    }
                }
            }else
            {
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, this);
}

/*
 *  庆祝
 */
void Round032_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round032_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
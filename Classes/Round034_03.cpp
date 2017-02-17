//
//  Round034_03.cpp
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#include "Round034_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(818,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(518,868)
#define WHITELIGHT_GAF_ZORDER           1
#define ANCHORS_ZORDER                  2
#define ROPE_ZORDER                     3

/*
 *  constructor
 */
Round034_03::Round034_03() {
    
    m_nodesPos[0] = Vec2(605, 700);
    m_nodesPos[1] = Vec2(907, 710);
    m_nodesPos[2] = Vec2(1210, 711);
    
    m_ropesPos[0] = Vec2(189, 441);
    m_ropesPos[1] = Vec2(189, 441);
    m_ropesPos[2] = Vec2(169, 441);
    
    m_anchorsPos[0] = Vec2(125, 340);
    m_anchorsPos[1] = Vec2(151, 340);
    m_anchorsPos[2] = Vec2(153, 340);

}

/*
 *  destructor
 */
Round034_03::~Round034_03()
{
    cleanUp();
}

//
void Round034_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round034_03* Round034_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round034_03();
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
bool Round034_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    for (unsigned long i = 0; i < 3; i++) {
        m_pNodes[i] = LayerColor::create(Color4B(255, 255, 255, 0), 300, 600);
        m_pNodes[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        m_pNodes[i]->setUserData((void*)i);
        this->addChild(m_pNodes[i]);
    }
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK10-2-TM%02ld.png",i+2);
        m_pRopesImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pRopesImageView[i]->setPosition(m_ropesPos[i]);
        m_pNodes[i]->addChild(m_pRopesImageView[i], ROPE_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK10-2-TM01.png");
        m_pAnchorsImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pAnchorsImageView[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        m_pAnchorsImageView[i]->setPosition(m_anchorsPos[i]);
        m_pNodes[i]->addChild(m_pAnchorsImageView[i], ANCHORS_ZORDER);
        m_pAnchorsImageView[i]->runAction(RepeatForever::create(Sequence::create(RotateBy::create(0.6f, -20),RotateBy::create(1.2f, 40),RotateBy::create(0.6f, -20), NULL)));
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pNodes[all]->setPosition(m_nodesPos[all]);
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round034_03::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round034_03::changeScaleSchedule), 2.0f, -1, 0);   //
}

//依次放大锚和绳子
void Round034_03::changeScaleSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round034_03::changeScaleSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pNodes[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.7f, 1.1f),CallFunc::create([this]{
        char buffer[64];
        sprintf(buffer, "HY-GK10-2-TM%02d.png",m_InitAniIndex+6);
        m_pRopesImageView[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        m_pAnchorsImageView[m_InitAniIndex]->loadTexture("HY-GK10-2-TM05.png",ui::Widget::TextureResType::PLIST);
    }),DelayTime::create(0.5f),CallFunc::create([this]{
        char buffer[64];
        sprintf(buffer, "HY-GK10-2-TM%02d.png",m_InitAniIndex+2);
        m_pRopesImageView[m_InitAniIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        m_pAnchorsImageView[m_InitAniIndex]->loadTexture("HY-GK10-2-TM01.png",ui::Widget::TextureResType::PLIST);
    }),ScaleTo::create(0.7f, 1.0f),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
    
}

/* 开启触摸 */
void Round034_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
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
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height+100);
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 2) {   //答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pNodes[i]);
                } /*for*/
                m_InitAniIndex = 0;
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
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pNodes[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pNodes[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round034_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round034_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
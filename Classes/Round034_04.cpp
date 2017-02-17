//
//  Round034_04.cpp
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#include "Round034_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  4
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(818,868)
#define PAOPAO_GAF_ZORDER              10
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(518,868)
#define WHITELIGHT_GAF_ZORDER           1
#define BOARD_ZORDER                    2
#define BOARD_POS                       Vec2(1192,929)
#define SHADOW_PNG_FILE                 "res/stage034/"
#define SHADOW_ZORDER                    2

using namespace ui;

/*
 *  constructor
 */
Round034_04::Round034_04() {
    
    m_answersPos[0] = Vec2(970, 1104);
    m_answersPos[1] = Vec2(1192, 1104);
    m_answersPos[2] = Vec2(1419, 1104);
    m_answersPos[3] = Vec2(970, 929);
    m_answersPos[4] = Vec2(1192, 929);
    m_answersPos[5] = Vec2(1419, 929);
    m_answersPos[6] = Vec2(970, 728);
    m_answersPos[7] = Vec2(1192, 728);
    m_answersPos[8] = Vec2(1419, 728);
    
}

/*
 *  destructor
 */
Round034_04::~Round034_04()
{
    cleanUp();
}

//
void Round034_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
}

//
Round034_04* Round034_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round034_04();
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
bool Round034_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    sprintf(buffer, "res/stage034/HY-GK10-3-IT01.png");
    m_pBoardSp = Sprite::create(buffer);
    m_pBoardSp->setPosition(BOARD_POS);
    this->addChild(m_pBoardSp, BOARD_ZORDER);
    
    for (unsigned long i = 0; i < 9; i++) {
        if (i >= 1) {
            sprintf(buffer, "HY-GK10-3-DA%02ld.png",i+2);
        }else
        {
            sprintf(buffer, "HY-GK10-3-DA%02ld.png",i+1);
        }
        
        m_pAnswersImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pAnswersImageView[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        this->addChild(m_pAnswersImageView[i], ANSWERS_ZORDER);
    }
    
    sprintf(buffer, "HY-GK10-3-DA02.png");
    m_pShadowSp = Sprite::createWithSpriteFrameName(buffer);
    this->addChild(m_pShadowSp,SHADOW_ZORDER);
    
    unsigned int all = 0;
    bool selected[9] = {false, false, false, false, false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 9) {
        long randIndex = static_cast<long>(rand() % 9);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersImageView[all]->setPosition(m_answersPos[randIndex]);
        m_pAnswersImageView[all]->setUserData((void*)randIndex);
        if (all == 0) {
            m_pShadowSp->setPosition(Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y+35));
        }
        
        all++;
    } /*while*/

    return true;
}

// 进入本回合
void Round034_04::onEnterRound()
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
    changeScaleMaxAnswers();
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round034_04::changeScaleSmallAnswers), 0.6f);
}

//放大答案
void Round034_04::changeScaleMaxAnswers()
{
    for (unsigned int i = 0; i < 9; i++) {
        
        m_pAnswersImageView[i]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),CallFunc::create([this,i]{
            char buffer[64];
            if (i >= 1) {
                sprintf(buffer, "HY-GK10-3-DA%02dB.png",i+2);
            }else
            {
                sprintf(buffer, "HY-GK10-3-DA%02dB.png",i+1);
            }
            m_pAnswersImageView[i]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        }), NULL));
    }
    
}

//缩小答案
void Round034_04::changeScaleSmallAnswers(float)
{
    for (unsigned int i = 0; i < 9; i++) {
        char buffer[64];
        if (i >= 1) {
            sprintf(buffer, "HY-GK10-3-DA%02d.png",i+2);
        }else
        {
            sprintf(buffer, "HY-GK10-3-DA%02d.png",i+1);
        }
        m_pAnswersImageView[i]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
        if (i == 8) {
            m_pAnswersImageView[i]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.0f),CallFunc::create([this]{
                m_jumped = true;
                if (m_opentouched) {
                    openTouch();
                }
            }), NULL));
        }else
        {
           m_pAnswersImageView[i]->runAction(ScaleTo::create(0.4f, 1.0f));
        }
        
    }
}

/* 开启触摸 */
void Round034_04::openTouch()
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
            if (index == 5) {   //答对
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
                for (unsigned int i = 0; i < 9; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersImageView[i]);
                } /*for*/
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
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersImageView[0]);
    for (unsigned int i = 1; i < 9; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersImageView[i]);
    } /*for*/
}

//飞礼物
void Round034_04::flyGift()
{
    m_pShadowSp->setVisible(false);
    for (unsigned int i = 0; i < 9; i++) {
        if ((long)m_pAnswersImageView[i]->getUserData() == 5) {
            m_pAnswersImageView[i]->setLocalZOrder(10);
            m_pAnswersImageView[i]->runAction(Sequence::create(Spawn::create(EaseSineIn::create(JumpTo::create(0.8f, Vec2(400, 600), 250, 1.0f)),Sequence::create(DelayTime::create(0.7f),FadeOut::create(0.1f), NULL), NULL),CallFunc::create([this]{
                if (m_completeCB) {
                    m_completeCB();
                }
            }), NULL));
            break;
        }
    }
}

/*
 *  庆祝
 */
void Round034_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round034_04::endRound()
{
    flyGift();
}
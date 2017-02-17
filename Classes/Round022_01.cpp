//
//  Round022_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/22.
//
//

#include "Round022_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "AppConfig.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY             5.0f
#define STANDBY                     "standby"                                   // 待机动画
#define TRIGGER                     "trigger"                                    // 触发动画

#define BN_PLIST_FILE               "res/stage022/XQ-GK10-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                 "res/stage022/XQ-GK10-IT.png"               // BN的 png 文件
#define ANSWERS_ZORDER              3

#define DOORFRAME_TAG                  200
#define UPWALL_TAG                     201
#define LEFTWALL_TAG                   202
#define UPWALL_POS                      Vec2(-40, 1605)
#define LEFTWALL_POS                   Vec2(-74, 968)
#define DOORFRAME_PNG_FILE              "res/stage022/XQ-GK10-BGIT-02.png"
#define UPWALL_PNG_FILE                 "res/stage022/XQ-GK10-BGIT-04.png"
#define LEFTWALL_PNG_FILE              "res/stage022/XQ-GK10-BGIT-03.png"
#define DOORFRAME_POS                   Vec2(250,822)

#define COLORBOARD_PNG_FILE             "XQ-GK10-1-IT01.png"
#define COLORBOARD_POS                  Vec2(1138,-205)
#define COLORBOARD_ZORDER               1
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

#define FLOWER_NORMAL_PNG_FILE          "XQ-GK10-1-TM01.png"
#define FLOWER_POS                      Vec2(1134,923)
#define FLOWER_ZORDER                   1
#define HINT_ZORDER                     2

/*
 *  constructor
 */
Round022_01::Round022_01(){
    
    m_answersPos[0] = Vec2(1013, -143);
    m_answersPos[1] = Vec2(1265, -143);
    
    m_hintsPos[0] = Vec2(1023, 954);
    m_hintsPos[1] = Vec2(1138, 905);
}

/*
 *  destructor
 */
Round022_01::~Round022_01()
{
    m_pStageScene->removeOBLeaderNode(LEFTWALL_TAG);
    m_pStageScene->removeOBLeaderNode(UPWALL_TAG);
    m_pStageScene->removeOBLeaderNode(DOORFRAME_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    Director::getInstance()->getTextureCache()->removeTextureForKey(LEFTWALL_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(DOORFRAME_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(UPWALL_PNG_FILE);
}

//
Round022_01* Round022_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round022_01();
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
bool Round022_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pDoorFrameSp = Sprite::create(DOORFRAME_PNG_FILE);
    m_pStageScene->addBelowLeaderNode(m_pDoorFrameSp, DOORFRAME_POS, DOORFRAME_TAG);
    
    m_pUpWallSp = Sprite::create(UPWALL_PNG_FILE);
    m_pStageScene->addBelowLeaderNode(m_pUpWallSp, UPWALL_POS, UPWALL_TAG);
    
    m_pLeftWallSp = Sprite::create(LEFTWALL_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pLeftWallSp, LEFTWALL_POS, LEFTWALL_TAG);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pColorBoard = Sprite::createWithSpriteFrameName(COLORBOARD_PNG_FILE);
    m_pColorBoard->setPosition(COLORBOARD_POS);
    this->addChild(m_pColorBoard, COLORBOARD_ZORDER);
    
    m_pFlowerImageView = ImageView::create(FLOWER_NORMAL_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pFlowerImageView->setPosition(FLOWER_POS);
    this->addChild(m_pFlowerImageView, FLOWER_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "XQ-GK10-1-TM%02ld.png",i+5);
        m_pHintsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pHintsSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHintsSp[i], HINT_ZORDER);
        m_pHintsSp[i]->setOpacity(0);
    }
    
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(buffer, "XQ-GK10-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWERS_ZORDER);
    }
    
    srand(unsigned(time(0)));
    bool allplaced[2] = {false, false};
    unsigned int placed = 0;
    
    while (placed < 2) {
        auto randIndex = static_cast<unsigned int>(rand() % 2);
        if (allplaced[randIndex]) continue;
        allplaced[randIndex] = true;
        m_pAnswersSp[placed]->setPosition(m_answersPos[randIndex]);
        m_answersEndPos[placed] = Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y+400);
        placed++;
    } /*while*/
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

//
void Round022_01::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this, dispatcher] (EventCustom *)
    {
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
    upColorBoard();
}

//上升颜色板
void Round022_01::upColorBoard()
{
    m_pColorBoard->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0.0f, 400)),CallFunc::create([this]{
        this->schedule(CC_SCHEDULE_SELECTOR(Round022_01::changeScaleAnswersSchedule), 0.8f, -1, 0);
    }), NULL));
    
    for (unsigned int i = 0; i < 2; i++) {
        m_pAnswersSp[i]->runAction(MoveBy::create(1.0f, Vec2(0.0f, 400)));
    }
}

//改变大小出现
void Round022_01::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round022_01::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round022_01::fadeHintsSchedule), 1.2f, -1, 0);
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f), NULL));
}

//渐现提示出现
void Round022_01::fadeHintsSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round022_01::fadeHintsSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pHintsSp[m_InitAniIndex++]->runAction(Sequence::create(FadeIn::create(0.3f),
                                                             FadeOut::create(0.3f),
                                                             FadeIn::create(0.3f),
                                                             FadeOut::create(0.3f),
                                                             NULL));
}

/* 开启触摸 */
void Round022_01::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        unsigned long index = (unsigned long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pHintsSp[0]->getContentSize().width, m_pHintsSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pHintsSp[0]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && index == 1) {
                m_curCorrectNum++;
                isCorrect = true;
                m_fristCorrectNum = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pHintsSp[1]->getContentSize().width, m_pHintsSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pHintsSp[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && index == 0) {
                m_curCorrectNum++;
                isCorrect = true;
                m_fristCorrectNum = 1;
            }
        }
        if (isCorrect) {
            target->runAction(FadeOut::create(0.2f));
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
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 2) {
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round022_01::downColorBoard), 1.0f);
                m_pFlowerImageView->loadTexture("XQ-GK10-1-TM04.png",ui::Widget::TextureResType::PLIST);
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }else if (m_curCorrectNum == 1)
            {
                if (m_fristCorrectNum == 0) {
                    m_pFlowerImageView->loadTexture("XQ-GK10-1-TM02.png",ui::Widget::TextureResType::PLIST);
                }else
                {
                    m_pFlowerImageView->loadTexture("XQ-GK10-1-TM03.png",ui::Widget::TextureResType::PLIST);
                }
            }

            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
        }else
        {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 2; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listener->clone(), m_pAnswersSp[i]);
    } /*for*/
}

//答对下降颜色板
void Round022_01::downColorBoard(float)
{
    m_pColorBoard->runAction(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -400.0f)), NULL));
}

//
void Round022_01::celebrate(const float delay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round022_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
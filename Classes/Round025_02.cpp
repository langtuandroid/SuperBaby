//
//  Round025_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#include "Round025_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define NUMBER_MOVE_TIME                0.5f                                     // 数字上升时间
#define NUMBER_MOVE_OFFSETY             400                                     //数字移动距离
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define SEABIRD_MOVE_TIME               2.0f                                    //海鸟移动时间
#define SEABIRD_MOVE_OFFSETX            -2000                                   //海鸟移动距离
#define SEABIRD_GAF_FILE                "res/stage025/HY-GK01-2-TM01-D/HY-GK01-2-TM01-D.gaf"        //海鸟gaf文件
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(924,1026)

/*
 *  constructor
 */
Round025_02::Round025_02()
:m_InitAniIndex(0)
{

    m_answersNumberPos[0] = Vec2(724, -150);
    m_answersNumberPos[1] = Vec2(1024, -150);
    m_answersNumberPos[2] = Vec2(1324, -150);
    
    m_seaBirdsPos[0] = Vec2(2153, 1299);
    m_seaBirdsPos[1] = Vec2(2637, 1407);
    m_seaBirdsPos[2] = Vec2(2653, 1076);
    m_seaBirdsPos[3] = Vec2(3145, 1243);
    m_seaBirdsPos[4] = Vec2(3369, 1479);
    m_seaBirdsPos[5] = Vec2(3507, 1076);
}

/*
 *  destructor
 */
Round025_02::~Round025_02()
{
    cleanUp();
}

//
Round025_02* Round025_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round025_02();
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
bool Round025_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "SZ3-%02ld.png",i+6);
        m_pAnswersNumber[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersNumber[i]->setUserData((void*)i);
        this->addChild(m_pAnswersNumber[i]);
    }
    
    for (unsigned int i = 0; i < 6; i++) {
        m_pSeaBirdsActor[i] = Actor::createWithGafFile(SEABIRD_GAF_FILE);
        m_pSeaBirdsActor[i]->setPosition(m_seaBirdsPos[i]);
        this->addChild(m_pSeaBirdsActor[i]);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int numberIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[numberIndex]) continue;
        selected[numberIndex] = true;
        m_pAnswersNumber[all]->setPosition(m_answersNumberPos[numberIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round025_02::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    leftMoveSeaBirdFirst();     //第一次左移海鸥
}

//
void Round025_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

/* 开启触摸 */
void Round025_02::openTouch()
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
            target->setScale(1.2f);
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
            if (index == 0) {   //答对
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersNumber[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersNumber[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersNumber[i]);
    } /*for*/
}

//第一次左移海鸥
void Round025_02::leftMoveSeaBirdFirst()
{
    for (unsigned int i = 0; i < 6; i++) {
        if (i == 5) {
            m_pSeaBirdsActor[i]->runAction(Sequence::create(MoveBy::create(SEABIRD_MOVE_TIME, Vec2(SEABIRD_MOVE_OFFSETX, 0.0f)),CallFunc::create([this]{
                this->schedule(CC_SCHEDULE_SELECTOR(Round025_02::upAnswerNumber), NUMBER_MOVE_TIME, -1, 0);
            }), NULL));
        } else {
            m_pSeaBirdsActor[i]->runAction(MoveBy::create(SEABIRD_MOVE_TIME, Vec2(SEABIRD_MOVE_OFFSETX, 0.0f)));
        }
    }
}

//上升答案
void Round025_02::upAnswerNumber(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_02::upAnswerNumber));
        return;
    }
    m_pAnswersNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(NUMBER_MOVE_TIME, Vec2(0.0f, NUMBER_MOVE_OFFSETY))));
}

//第二次左移海鸥
void Round025_02::leftMoveSeaBirdSecond()
{
    for (unsigned int i = 0; i < 6; i++) {
        m_pSeaBirdsActor[i]->runAction(MoveBy::create(SEABIRD_MOVE_TIME, Vec2(SEABIRD_MOVE_OFFSETX, 0.0f)));
    }
}

//下降答案
void Round025_02::downAnswerNumber(float)
{
    if (m_InitAniIndex < 0) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_02::downAnswerNumber));
        return;
    }
    m_pAnswersNumber[--m_InitAniIndex]->runAction(FadeOut::create(0.3f));
}

/*
 *  庆祝
 */
void Round025_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round025_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
    leftMoveSeaBirdSecond();            //第二次移动海鸥离开回合
    this->schedule(CC_SCHEDULE_SELECTOR(Round025_02::downAnswerNumber), 0.3f, -1, 0);
}

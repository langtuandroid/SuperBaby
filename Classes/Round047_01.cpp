//
//  Round047_01.cpp
//  superBaby
//
//  Created by Administrator on 15/8/5.
//
//

#include "Round047_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage047/GC-GK11-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage047/GC-GK11-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间
#define ANSWERS_ZORDER                  4
#define TOPICS_ZORDER                   3
#define BACK_PNG_FILE                   "GC-GK11-1-TM02.png"
#define BACK_ZORDER                     2
#define HINT_PNG_FILE                   "GC-GK11-1-TM03.png"
#define HINT_ZORDER                     1

/*
 *  constructor
 */
Round047_01::Round047_01()
{
    m_answersPos[0] = Vec2(792, 600);
    m_answersPos[1] = Vec2(1066, 600);
    m_answersPos[2] = Vec2(1340, 600);
    
    m_backsPos[0] = Vec2(226,1000);
    m_backsPos[1] = Vec2(474,1000);
    m_backsPos[2] = Vec2(710,1000);
    m_backsPos[3] = Vec2(956,1000);
    m_backsPos[4] = Vec2(1208,1000);
    m_backsPos[5] = Vec2(1441,1000);
    m_backsPos[6] = Vec2(1691,1000);
    
    m_hintsPos[0] = Vec2(474, 1000);
    m_hintsPos[1] = Vec2(1208, 1000);
}
/*
 *  destructor
 */
Round047_01::~Round047_01()
{
    cleanUp();
}

//
void Round047_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round047_01* Round047_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene,const Vec2& pos)
{
    auto pRound = new Round047_01();
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
bool Round047_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK11-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setOpacity(0);
        this->addChild(m_pAnswersSp[i],ANSWERS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersSp[all]->setPosition(m_answersPos[randIndex]);
        m_randNum[randIndex] = all;
        m_answersEndPos[all] = Vec2(m_answersPos[randIndex].x, m_answersPos[randIndex].y-300);
        all++;
    } /*while*/
    
    for (unsigned int i = 0; i < 7; i++) {
        if (i == 0 || i == 3) {
            sprintf(buffer, "GC-GK11-1-DA03.png");
        } else if (i == 6)
        {
            sprintf(buffer, "GC-GK11-1-WH.png");
        } else
        {
            sprintf(buffer, "GC-GK11-1-DA02.png");
        }
        
        m_pTopicsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTopicsSp[i]->setPosition(m_backsPos[i]);
        this->addChild(m_pTopicsSp[i],TOPICS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 7; i++) {
        m_pBacksSp[i] = Sprite::createWithSpriteFrameName(BACK_PNG_FILE);
        m_pBacksSp[i]->setPosition(m_backsPos[i]);
        this->addChild(m_pBacksSp[i],BACK_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        m_pHintsSp[i] = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
        m_pHintsSp[i]->setPosition(m_hintsPos[i]);
        m_pHintsSp[i]->setOpacity(0);
        this->addChild(m_pHintsSp[i],HINT_ZORDER);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
    return true;
}

// 进入本回合
void Round047_01::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round047_01::downAnswersSchedule), 0.6f, -1, 0);   //
}

//依次下降出现答案
void Round047_01::downAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round047_01::downAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round047_01::showHintsSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_randNum[m_InitAniIndex++]]->runAction(Spawn::create(MoveBy::create(0.5f, Vec2(0.0f, -300.0f)),FadeIn::create(0.6f), NULL));
}

void Round047_01::showHintsSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round047_01::showHintsSchedule));
        changeScaleMark();
        return;
    }
    m_pHintsSp[m_InitAniIndex++]->runAction(Sequence::create(FadeIn::create(0.3f),
                                                           FadeOut::create(0.3f), NULL));
}

void Round047_01::changeScaleMark()
{
    m_pTopicsSp[6]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.2f),ScaleTo::create(0.3f, 1.0f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round047_01::openTouch()
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        auto touchPoint = m_pBacksSp[6]->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pBacksSp[6]->getContentSize().width, m_pBacksSp[6]->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (index == 2) {
                target->setPosition(m_pTopicsSp[6]->getPosition());
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
            }
        } else
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
//下降答案
void Round047_01::downAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 2) {
            if (i == 1) {
                m_pAnswersSp[i]->runAction(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -500.0f)),CallFunc::create([this]{
                    if (m_completeCB) {
                        m_completeCB();
                    }
                }), NULL));
            } else
            {
                m_pAnswersSp[i]->runAction(MoveBy::create(0.5f, Vec2(0.0f, -500.0f)));
            }
        }
    }
}

/*
 *  庆祝
 */
void Round047_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round047_01::endRound()
{
    downAnswers();
}

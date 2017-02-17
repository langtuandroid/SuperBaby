//
//  Round039_01.cpp
//  superBaby
//
//  Created by Administrator on 15/7/23.
//
//

#include "Round039_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage039/GC-GK03-TM.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage039/GC-GK03-TM.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWERS_ZORDER                  3
#define CAR_PNG_FILE                    "GC-GK03-1-TM-D.png"
#define CAR_PNG_END_FILE                "GC-GK03-1-TM-C.png"
#define CAR_POS                         Vec2(1162,501)
#define CAR_ZORDER                      1
#define MARK_PNG_FILE                   "GC-GK03-4-TM02.png"
#define MARK_PNG_LIGHT_FILE             "GC-GK03-4-TM02B.png"
#define MARK_POS                        Vec2(1248,414)
#define MARK_ZORDER                     2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round039_01::Round039_01()
{
    m_answersPos[0] = Vec2(865, 150);
    m_answersPos[1] = Vec2(1182, 150);
    m_answersPos[2] = Vec2(1499, 150);
}

/*
 *  destructor
 */
Round039_01::~Round039_01()
{
    cleanUp();
}

//
void Round039_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round039_01* Round039_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round039_01();
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
bool Round039_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pCarImageView = ImageView::create(CAR_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pCarImageView->setPosition(CAR_POS);
    this->addChild(m_pCarImageView, CAR_ZORDER);
    
    m_pMarkImageView = ImageView::create(MARK_PNG_LIGHT_FILE,ui::Widget::TextureResType::PLIST);
    m_pMarkImageView->setPosition(MARK_POS);
    m_pMarkImageView->setVisible(false);
    m_pMarkImageView->setScale(1.13f);
    this->addChild(m_pMarkImageView, MARK_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK03-1-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        m_pAnswersSp[i]->setScale(0.0f);
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
        m_answersEndPos[all] = m_answersPos[randIndex];
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
void Round039_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round039_01::changeScaleAnswersSchedule), 0.4f, -1, 0);   //
    
}

//依次放大答案以显示
void Round039_01::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_01::changeScaleAnswersSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round039_01::changeMarkSchedule), 0.5f, -1, 0);   //
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(0.4f, 1.0f)));
}

void Round039_01::changeMarkSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round039_01::changeMarkSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pMarkImageView->runAction(Sequence::create(CallFunc::create([this]{
        m_pMarkImageView->setVisible(true);
    }),DelayTime::create(0.4f),CallFunc::create([this]{
        m_pMarkImageView->setVisible(false);
        m_InitAniIndex++;
    }), NULL));
}

/* 开启触摸 */
void Round039_01::openTouch()
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
        auto index = (long)target->getUserData();
        auto touchPoint = m_pMarkImageView->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pMarkImageView->getContentSize().width, m_pMarkImageView->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 0) {
                target->setPosition(m_pMarkImageView->getPosition());
                m_pMarkImageView->setVisible(false);
                target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.15f, 1.0f),Spawn::create(FadeOut::create(0.1f),CallFunc::create([this]{
                    changeCar();
                }), NULL), NULL));
                
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
                fadeAnswers();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_answersEndPos[index])),CallFunc::create([this,target,index]{
                    target->setLocalZOrder(ANSWERS_ZORDER);
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
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

void Round039_01::fadeAnswers()
{
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 0) {
            m_pAnswersSp[i]->runAction(FadeOut::create(0.3f));
        }
    }
}

void Round039_01::changeCar()
{
    m_pCarImageView->loadTexture(CAR_PNG_END_FILE,ui::Widget::TextureResType::PLIST);
}

/*
 *  庆祝
 */
void Round039_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round039_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

//
//  Round018_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/18.
//
//

#include "Round018_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"
#include "LeadingActor.h"
#include "AppConfig.h"

#define CELEBRATE_DELAY                 5.0f
#define ARROWS_ZORDER                   2
#define ARROWS_GAF_CONFIG_FILE          "config/stage018/arrows.json"
#define ARROWS_GAF_FILE                 "res/stage018/XQ-GK06-3-IT-D+C/XQ-GK06-3-IT-D+C.gaf"
#define ARROWS_POS                      Vec2(967,1098)
#define BOARD_POS                       Vec2(1090,835)
#define BOARD_PNG_NORMAL_FILE           "XQ-GK06-3-TM02.png"
#define BOARD_PNG_FILE                  "XQ-GK06-3-TM04.png"
#define BOARD_ZORDER                    1

#define HINT_PNG_FILE                   "XQ-GK06-3-TM03.png"
#define HINT_POS                        Vec2(1213,840)
#define HINT_ZORDER                     2
#define ANSWER_ZORDER                   3
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define MOV_TIME                        0.2f                                        // 答错回原位置移动时间
#define BRIDGE_PNG_FILE                 "res/common/XQ-GK01-2-IT01.png"
#define BRIDGE_END_POS                  Vec2(9105,319)
#define BRIDGE_TAG                      200

/*
 *  constructor
 */
Round018_03::Round018_03()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(734, 225);
    m_answersPos[1] = Vec2(1050, 225);
    m_answersPos[2] = Vec2(1366, 225);
}

/*
 *  destructor
 */
Round018_03::~Round018_03()
{
    cleanUp();
}

//
void Round018_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(ARROWS_GAF_FILE);
    m_pStageScene->removeOBLeaderNode(BRIDGE_TAG);
    Director::getInstance()->getTextureCache()->removeTextureForKey(BRIDGE_PNG_FILE);

}

//
Round018_03* Round018_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round018_03();
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
bool Round018_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoardImageView = ImageView::create(BOARD_PNG_NORMAL_FILE,ui::Widget::TextureResType::PLIST);
    m_pBoardImageView->setPosition(BOARD_POS);
    this->addChild(m_pBoardImageView, BOARD_ZORDER);
    
    m_pArrowActor = Actor::createWithGafConfigFile(ARROWS_GAF_CONFIG_FILE);
    m_pArrowActor->playWithSequenceAliasName(STANDBY,true);
    m_pArrowActor->setPosition(ARROWS_POS);
    this->addChild(m_pArrowActor, ARROWS_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "XQ-GK06-3-DA%02ld.png",i+1);
        m_pAnswersSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pAnswersSp[i]->setUserData((void*)i);
        this->addChild(m_pAnswersSp[i], ANSWER_ZORDER);
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
    
    m_pHintSp = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
    m_pHintSp->setPosition(HINT_POS);
    this->addChild(m_pHintSp, HINT_ZORDER);
    m_pHintSp->setOpacity(0);
    
    m_pDrawbridgeSp = Sprite::create(BRIDGE_PNG_FILE);
    m_pStageScene->addBelowLeaderNode(m_pDrawbridgeSp, BRIDGE_END_POS, BRIDGE_TAG);
    
    return true;
}

// 进入本回合
void Round018_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round018_03::changeScaleAnswersSchedule), 0.6f, -1, 0.0f);
    
}

//依次放大答案，以提示
void Round018_03::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round018_03::changeScaleAnswersSchedule));
        fadeHint();
        return;
    }
    m_pAnswersSp[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//渐隐渐显以提示
void Round018_03::fadeHint()
{
    m_pHintSp->runAction(Sequence::create(FadeIn::create(0.3f),
                                          DelayTime::create(0.2f),
                                          FadeOut::create(0.3f),
                                          FadeIn::create(0.3f),
                                          DelayTime::create(0.2f),
                                          FadeOut::create(0.3f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round018_03::openTouch()
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
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
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp->getContentSize().width, m_pHintSp->getContentSize().height);
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        if (rect.containsPoint(m_pHintSp->convertTouchToNodeSpace(touch))) {
            if (index == 0) {
                target->runAction(FadeOut::create(0.2f));
                m_pBoardImageView->loadTexture(BOARD_PNG_FILE,ui::Widget::TextureResType::PLIST);
                changeActor();
                
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersSp[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
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
                    target->setPosition(m_answersEndPos[index]);
                }), NULL));
            }
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
                target->setPosition(m_answersEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersSp[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersSp[i]);
    } /*for*/
    
}

//答对后改变actor动作
void Round018_03::changeActor()
{
    m_pArrowActor->playWithSequenceAliasName(TRIGGER,true);
}

//答对后隐藏剩余答案
void Round018_03::fadeOutAnswers(float)
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pAnswersSp[i]->runAction(FadeOut::create(0.3f));
    }
}

/*
 *  庆祝
 */
void Round018_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round018_03::fadeOutAnswers), 1.0f);
}

/*  回合结束回调 */
void Round018_03::endRound()
{

    m_pBoardImageView->runAction(FadeOut::create(0.3f));
    m_pArrowActor->setVisible(false);
    auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
    auto moveDuration = 1220 / bgScrollSpeed;
    
    m_pStageScene->moveLeadingActorCB(moveDuration, Vec2(1220, 0), [this]
    {
        //此处艾米和蓓蓓移动时间必须根据bgScrollSpeed换算设置
        m_pStageScene->moveAimiActorCB(1.56f, Vec2(400, 150),[this]{
            m_pStageScene->moveAimiActorCB(0.39f, Vec2(100, 0), [this]{
                if (m_completeCB) {
                    m_completeCB();
                }
            });
        },false);
        //同上
        m_pStageScene->moveBeibeiActorCB(0.85f, Vec2(220, 0), [this]{
            m_pStageScene->moveBeibeiActorCB(1.09f, Vec2(280, 150.0f));
        },false);
    },false);
    m_pStageScene->manualBackgroundScroll(1623.0f,nullptr);
}

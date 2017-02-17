//
//  Round045_03.cpp
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#include "Round045_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define ANSWER_PNG_FILE                 "GC-GK09-3-DA01.png"
#define ANSWER_POS                      Vec2(996,946)
#define ANSWER_ZORDER                   3
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define HINT_PNG_FILE                   "GC-GK09-3-DA0B.png"
#define HINT_LIGHT_PNG_FILE             "GC-GK09-3-DA0C.png"
#define HINT_POS                        Vec2(1322,348)
#define HINT_ZORDER                     2
#define MS_GAF_CONFIG_FILE              "res/stage045/ms03.json"
#define MS_GAF_FILE                     "res/stage045/GC-GK09-3-MS-D+C/GC-GK09-3-MS-D+C.gaf"
#define MS_POS                          Vec2(1147,162)
#define MS_ZORDER                       1
#define SHAWDOW_PNG_FILE                "GC-GK09-3-DA01-ying.png"
#define SHAWDOW_POS                     Vec2(1004, 863)
#define SHAWDOW_ZORDER                  1
#define STANDBY                         "standby"                                    // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画

/*
 *  constructor
 */
Round045_03::Round045_03()
{
}

/*
 *  destructor
 */
Round045_03::~Round045_03()
{
    cleanUp();
}

//
Round045_03* Round045_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round045_03();
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
bool Round045_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pHintImageView = ImageView::create(HINT_PNG_FILE,ui::Widget::TextureResType::PLIST);
    m_pHintImageView->setPosition(HINT_POS);
    this->addChild(m_pHintImageView,HINT_ZORDER);
    
    m_pAnswerSp = Sprite::createWithSpriteFrameName(ANSWER_PNG_FILE);
    m_pAnswerSp->setPosition(ANSWER_POS);
    this->addChild(m_pAnswerSp, ANSWER_ZORDER);
    
    m_pShawdowSp = Sprite::createWithSpriteFrameName(SHAWDOW_PNG_FILE);
    m_pShawdowSp->setPosition(SHAWDOW_POS);
    this->addChild(m_pShawdowSp, SHAWDOW_ZORDER);
    
    m_pMSActor = Actor::createWithGafConfigFile(MS_GAF_CONFIG_FILE);
    m_pMSActor->playWithSequenceAliasName(STANDBY,true);
    m_pMSActor->setPosition(MS_POS);
    this->addChild(m_pMSActor, MS_ZORDER);

    return true;
}

// 进入本回合
void Round045_03::onEnterRound()
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
    changeScaleAnswer();
}

//改变答案的大小
void Round045_03::changeScaleAnswer()
{
    m_pAnswerSp->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),
                                            ScaleTo::create(0.3f, 1.0f),
                                            ScaleTo::create(0.3f, 1.1f),
                                            ScaleTo::create(0.3f, 1.0f),
                                            CallFunc::create([this]{
        changeScaleHint();
    }), NULL));
}

void Round045_03::changeScaleHint()
{
    m_pHintImageView->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),
                                                 CallFunc::create([this]{
        m_pHintImageView->loadTexture(HINT_LIGHT_PNG_FILE,ui::Widget::TextureResType::PLIST);
    }),
                                                 ScaleTo::create(0.4f, 1.0f),
                                                 CallFunc::create([this]{
        m_pHintImageView->loadTexture(HINT_PNG_FILE,ui::Widget::TextureResType::PLIST);
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));}

//
void Round045_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

/* 开启触摸 */
void Round045_03::openTouch()
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
        m_pShawdowSp->setVisible(false);
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintImageView->getContentSize().width,
                         m_pHintImageView->getContentSize().height);
        if (rect.containsPoint(m_pHintImageView->convertTouchToNodeSpace(touch)))
        {
            target->setPosition(m_pHintImageView->getPosition());
            target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.15f, 1.0f),FadeOut::create(0.1f),CallFunc::create([this]{
                m_pHintImageView->runAction(FadeOut::create(0.2f));
                changeActor();
            }), NULL));
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
            
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
        } else
        {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, ANSWER_POS)),CallFunc::create([this,target,index]{
                m_pShawdowSp->setVisible(true);
                target->setLocalZOrder(ANSWER_ZORDER);
                target->setPosition(ANSWER_POS);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswerSp);
}

void Round045_03::changeActor()
{
    m_pMSActor->playWithSequenceAliasName(TRIGGER);
}

/*
 *  庆祝
 */
void Round045_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/*  回合结束回调 */
void Round045_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
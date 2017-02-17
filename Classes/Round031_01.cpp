//
//  Roung031_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#include "Round031_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage031/HY-GK07-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage031/HY-GK07-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                 "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                  Vec2(318,868)
#define PAOPAO_GAF_ZORDER               1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(272,795)
#define WHITELIGHT_GAF_ZORDER           1
#define ANSWERS_ZORDER                  4
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

#define FISH_THREE_GAF_CONFIG_FILE      "res/stage030/fishthree.json"
#define FISH_THREE_GAF_FILE             "res/stage030/HY-GK06-3-MS03-D/HY-GK06-3-MS03-D.gaf"
#define FISH_GAF_ZORDER                 1
#define STANDBY                         "standby"                                   // 待机动画
#define TRIGGER                         "trigger"                                    // 触发动画
#define HINT_PNG_FILE                   "HY-GK07-1-MS01 -bian.png"
#define HINT_ZORDER                     2
#define HIGH_HINT_PNG_FILE              "HY-GK07-1-MS01-bai.png"
#define MARK_PNG_FILE                   "HY-GK07-1-wenhao.png"
#define MARK_ZORDER                     3

/*
 *  constructor
 */
Round031_01::Round031_01()
{
    m_fishesPos[0] = Vec2(718, 1139);
    m_fishesPos[1] = Vec2(718, 923);
    m_fishesPos[2] = Vec2(718, 695);
    m_fishesPos[3] = Vec2(1042, 1139);
    m_fishesPos[4] = Vec2(1042, 695);
    m_fishesPos[5] = Vec2(1144, 1139);
    m_fishesPos[6] = Vec2(1144, 923);
    m_fishesPos[7] = Vec2(1465, 1139);
    m_fishesPos[8] = Vec2(1465, 923);
    m_fishesPos[9] = Vec2(1465, 695);

    m_hintsPos[0] = Vec2(906, 986);
    m_hintsPos[1] = Vec2(1263, 759);
    
    m_answersPos[0] = Vec2(942, 303);
    m_answersPos[1] = Vec2(1332, 303);
}

/*
 *  destructor
 */
Round031_01::~Round031_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(FISH_THREE_GAF_FILE);
}

//
Round031_01* Round031_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round031_01();
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
bool Round031_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    for (unsigned int i = 0; i < 10; i ++) {
        m_pFishesActor[i] = Actor::createWithGafConfigFile(FISH_THREE_GAF_CONFIG_FILE);
        m_pFishesActor[i]->setPosition(m_fishesPos[i]);
        if (i < 5) {
            m_pFishesActor[i]->setRotationSkewY(-180);
        }
        this->addChild(m_pFishesActor[i], FISH_GAF_ZORDER);
        m_pFishesActor[i]->playWithSequenceAliasName(STANDBY,true);
    }
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    
    for (unsigned long i = 0; i < 2; i++) {
        m_pAnswersActor[i] = Actor::createWithGafConfigFile(FISH_THREE_GAF_CONFIG_FILE);
        if (i == 0) {
            m_pAnswersActor[i]->setRotationSkewY(-180);
        }
        m_pAnswersActor[i]->setUserData((void*)i);
        this->addChild(m_pAnswersActor[i], ANSWERS_ZORDER);
        m_pAnswersActor[i]->playWithSequenceAliasName(STANDBY,true);
        m_pAnswersActor[i]->setAnchorPoint(Vec2(0.5f, 0.5f));
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        m_pHintSp[i] = Sprite::createWithSpriteFrameName(HINT_PNG_FILE);
        if (i == 0) {
            m_pHintSp[i]->setRotationSkewY(-180);
        }
        m_pHintSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHintSp[i], HINT_ZORDER);
        m_pHintSp[i]->setScale(0.6f);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        m_pHighLightHintSp[i] = Sprite::createWithSpriteFrameName(HIGH_HINT_PNG_FILE);
        if (i == 0) {
            m_pHighLightHintSp[i]->setRotationSkewY(-180);
        }
        m_pHighLightHintSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHighLightHintSp[i], HINT_ZORDER);
        m_pHighLightHintSp[i]->setVisible(false);
        m_pHighLightHintSp[i]->setScale(0.6f);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        m_pMarksSp[i] = Sprite::createWithSpriteFrameName(MARK_PNG_FILE);
        m_pMarksSp[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pMarksSp[i], MARK_ZORDER);
        m_pMarksSp[i]->setScale(0.5f);
    }
    
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pAnswersActor[all]->setPosition(m_answersPos[randIndex]);
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
void Round031_01::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round031_01::changeScaleHintSchedule), 1.2f, -1, 0);   //
    
}

//改变提示大小
void Round031_01::changeScaleHintSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round031_01::changeScaleHintSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round031_01::changeScaleAnswersSchedule), 0.6f, -1, 0);   //
        return;
    }
    m_pHintSp[m_InitAniIndex]->runAction(Sequence::create(DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHighLightHintSp[m_InitAniIndex]->setVisible(true);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHighLightHintSp[m_InitAniIndex]->setVisible(false);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHighLightHintSp[m_InitAniIndex]->setVisible(true);
    }),DelayTime::create(0.3f),CallFunc::create([this]{
        m_pHighLightHintSp[m_InitAniIndex]->setVisible(false);
        m_InitAniIndex++;
    }), NULL));
}

//改变答案大小以提示
void Round031_01::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round031_01::changeScaleAnswersSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pAnswersActor[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

/* 开启触摸 */
void Round031_01::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            target->setLocalZOrder(5);
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto index = (long)target->getUserData();
        target->setScale(1.0f);
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp[0]->getContentSize().width, m_pHintSp[0]->getContentSize().height);
        if (rect.containsPoint(m_pHintSp[0]->convertTouchToNodeSpace(touch))) {
            if (index == 0) {
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pHintSp[1]->getContentSize().width, m_pHintSp[1]->getContentSize().height);
        if (rect.containsPoint(m_pHintSp[1]->convertTouchToNodeSpace(touch))) {
            if (index == 1) {
                isCorrect = true;
            }
        }
        if (isCorrect) {
            m_curCorrectNum++;
            target->setPosition(m_hintsPos[index]);
            m_pHintSp[index]->setVisible(false);
            m_pMarksSp[index]->setVisible(false);
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (m_curCorrectNum == 2) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            }else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersActor[0]);
    for (unsigned int i = 1; i < 2; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersActor[i]);
    } /*for*/
}

void Round031_01::moveFishes()
{
    for (unsigned int i = 0; i < 10; i++) {
        if (i < 5) {
            m_pFishesActor[i]->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(2000.0f, 0.0f)),CallFunc::create([this,i]{
                m_pFishesActor[i]->setVisible(false);
            }), NULL));
        }else
        {
            m_pFishesActor[i]->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(-2000.0f, 0.0f)),CallFunc::create([this,i]{
                m_pFishesActor[i]->setVisible(false);
            }), NULL));
        }
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        if (i == 0) {
            m_pAnswersActor[i]->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(2000.0f, 0.0f)),CallFunc::create([this,i]{
                m_pAnswersActor[i]->setVisible(false);
            }), NULL));
        }else
        {
            m_pAnswersActor[i]->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(-2000.0f, 0.0f)),CallFunc::create([this,i]{
                m_pAnswersActor[i]->setVisible(false);
            }), NULL));
        }
    }
}

/*
 *  庆祝
 */
void Round031_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round031_01::endRound()
{
    
    moveFishes();
    scheduleOnce([this](float){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 3.0f, "delaytime");
}
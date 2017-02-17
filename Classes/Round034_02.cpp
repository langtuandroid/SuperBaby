//
//  Round034_02.cpp
//  superBaby
//
//  Created by Administrator on 15/6/23.
//
//

#include "Round034_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

USING_NS_CC;

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define PAOPAO_GAF_FILE                "res/stage028/HY-GK-paopao01/HY-GK-paopao01.gaf"
#define PAOPAO_GAF_POS                 Vec2(1118,868)
#define PAOPAO_GAF_ZORDER              1
#define WHITELIGHT_GAF_FILE             "res/stage028/HY-GK-guang/HY-GK-guang.gaf"
#define WHITELIGHT_GAF_POS              Vec2(272,795)
#define WHITELIGHT_GAF_ZORDER           1
#define ANSWERS_ZORDER                 3
#define TOPIC_ZORDER                   2
#define SHIP_PNG_FILE                  "HY-GK10-1-TM02.png"
#define SHIP_POS                        Vect(714,708)
#define SHIP_ZORDER                     5
#define MOUNT_PNG_FILE                  "HY-GK10-1-TM06.png"
#define MOUNT_POS                       Vect(1260,1044)
#define MOUNT_ZORDER                    4
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define BN_PLIST_FILE                  "res/stage034/HY-GK10-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage034/HY-GK10-IT.png"               // BN的 png 文件
#define BOARD_PNG_FILE                  "res/stage034/HY-GK10-1-TM01.png"
#define BOARD_POS                       Vec2(1004,936)
#define BOARD_ZORDER                    1
/*
 *  constructor
 */
Round034_02::Round034_02()
{
    
    m_answersPos[0] = Vec2(788, 1004);
    m_answersPos[1] = Vec2(1016, 890);
    m_answersPos[2] = Vec2(1186, 750);
    
    m_topicsPos[0] = Vec2(980, 917);
    m_topicsPos[1] = Vec2(1004, 846);
    m_topicsPos[2] = Vec2(1047, 789);
    
    m_topicsNormalStr[0] = "HY-GK10-1-TM05.png";
    m_topicsNormalStr[1] = "HY-GK10-1-TM04.png";
    m_topicsNormalStr[2] = "HY-GK10-1-TM03.png";
    
    m_topicsStr[0] = "HY-GK10-1-TM08.png";
    m_topicsStr[1] = "HY-GK10-1-TM10.png";
    m_topicsStr[2] = "HY-GK10-1-TM12.png";
    
    m_answersNormalStr[0] = "HY-GK10-1-DA01.png";
    m_answersNormalStr[1] = "HY-GK10-1-DA02.png";
    m_answersNormalStr[2] = "HY-GK10-1-DA03.png";
    
    m_answersStr[0] = "HY-GK10-1-TM07.png";
    m_answersStr[1] = "HY-GK10-1-TM09.png";
    m_answersStr[2] = "HY-GK10-1-TM11.png";
}

/*
 *  destructor
 */
Round034_02::~Round034_02()
{
    cleanUp();
}

//
void Round034_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    Director::getInstance()->getTextureCache()->removeTextureForKey(BOARD_PNG_FILE);
}

//
Round034_02* Round034_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round034_02();
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
bool Round034_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
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
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_pBoardSp =Sprite::create(BOARD_PNG_FILE);
    m_pBoardSp->setPosition(BOARD_POS);
    this->addChild(m_pBoardSp, BOARD_ZORDER);
    
    for (unsigned long i = 0; i < 3; i++) {
        m_pAnswersImageView[i] = ImageView::create(m_answersNormalStr[i],ui::Widget::TextureResType::PLIST);
        m_pAnswersImageView[i] -> setPosition(m_answersPos[i]);
        m_pAnswersImageView[i]->setUserData((void*)i);
        this->addChild(m_pAnswersImageView[i], ANSWERS_ZORDER);
    }
    
    for (unsigned long i = 0; i < 3; i++) {
        m_pTopicsImageView[i] = ImageView::create(m_topicsNormalStr[i],ui::Widget::TextureResType::PLIST);
        m_pTopicsImageView[i] -> setPosition(m_topicsPos[i]);
        this->addChild(m_pTopicsImageView[i], TOPIC_ZORDER);
    }
    
    m_pShipSp = Sprite::createWithSpriteFrameName(SHIP_PNG_FILE);
    m_pShipSp->setPosition(SHIP_POS);
    this->addChild(m_pShipSp, SHIP_ZORDER);
    
    m_pMountSp =Sprite::createWithSpriteFrameName(MOUNT_PNG_FILE);
    m_pMountSp->setPosition(MOUNT_POS);
    this->addChild(m_pMountSp, MOUNT_ZORDER);
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }

    return true;
}

// 进入本回合
void Round034_02::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round034_02::changeScaleAnswersSchedule), 1.1f, -1, 0);   //
    this->schedule(CC_SCHEDULE_SELECTOR(Round034_02::changeScaleTopicSchedule), 1.1f, -1, 0);   //
    
}

//依次放大城堡以提示
void Round034_02::changeScaleAnswersSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round034_02::changeScaleAnswersSchedule));
        return;
    }
    m_pAnswersImageView[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.05f),CallFunc::create([this]{
        m_pAnswersImageView[m_InitAniIndex]->loadTexture(m_answersStr[m_InitAniIndex],ui::Widget::TextureResType::PLIST);
    }),DelayTime::create(0.2f),CallFunc::create([this]{
        m_pAnswersImageView[m_InitAniIndex]->loadTexture(m_answersNormalStr[m_InitAniIndex],ui::Widget::TextureResType::PLIST);
    }),ScaleTo::create(0.4f, 1.0f),CallFunc::create([this]{
        m_InitAniIndex++;
    }), NULL));
}

//依次放大题目以提示
void Round034_02::changeScaleTopicSchedule(float)
{
    if (m_InitIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round034_02::changeScaleTopicSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pTopicsImageView[m_InitIndex]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.05f),CallFunc::create([this]{
        m_pTopicsImageView[m_InitIndex]->loadTexture(m_topicsStr[m_InitIndex],ui::Widget::TextureResType::PLIST);
    }),DelayTime::create(0.2f),CallFunc::create([this]{
        m_pTopicsImageView[m_InitIndex]->loadTexture(m_topicsNormalStr[m_InitIndex],ui::Widget::TextureResType::PLIST);
    }),ScaleTo::create(0.4f, 1.0f),CallFunc::create([this]{
        m_InitIndex++;
    }), NULL));
}

/* 开启触摸 */
void Round034_02::openTouch()
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
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        if (rect.containsPoint(touchPoint)) {
            if (index == 1) {
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pAnswersImageView[i]);
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
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pAnswersImageView[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pAnswersImageView[i]);
    } /*for*/
}

//答对后移动船
void Round034_02::moveShip()
{
    m_pShipSp->runAction(Sequence::create(MoveTo::create(1.0f, Vec2(m_pMountSp->getPositionX()-20, m_pMountSp->getPositionY()-10)),Spawn::create(ScaleTo::create(0.3f, 0.8f),MoveBy::create(0.3f, Vec2(30, 50)), NULL),CallFunc::create([this]{
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
}

/*
 *  庆祝
 */
void Round034_02::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round034_02::endRound()
{
    moveShip();
}
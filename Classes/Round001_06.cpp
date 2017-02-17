//
//  Round001_06.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/3/27.
//
//

#include "Round001_06.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SelStageScene.h"
#include "StageDataManager.h"
#include "AppConfig.h"
#include "GafAssetCache.h"
#include "MyAudioEngine.h"

#define PUZZLE_BORAD_FULL_PNG         "res/stage001/SL-GK01-4-TM01-2.png"   // 完整的拼图板png
#define PUZZLE_BORAD_FULL_ZORDER      1                                     // 完整的拼图板Z值
#define SNAKE_GAF_CONFIG_FILE         "config/stage001/snake.json"          // 蛇gaf配置文件
#define SNAKE_GAF_FILE                "res/stage001/SL-GK01-4-MS01-D+C/SL-GK01-4-MS01-D+C.gaf"          // 蛇 gaf文件

#define SNAKE_ZORDER                  1                                     // 蛇的z值
#define SNAKE_INIT_POS                Vec2(1370.0f, 854.0f)                 // 蛇的初始位置
#define SNAKE_STANDBY                   "standby"                           // 待机动画
#define SNAKE_TRIGGER                   "trigger"                           // 触发动画

#define PUZZLE_IMAGE_FILE             "res/stage001/SL-GK01-IT.png"         // 拼图图片
#define PUZZLE_BOARD_PNG              "SL-GK01-4-TM01-1.png"                // 拼图板png名
#define PUZZLE_NODE_BOARD_ZORDER      2                                     // 拼图板Z值
#define PUZZLE_NODE_ZORDER            3                                     // 拼图块Z值
#define PUZZLE_BOARD_INIT_POS         Vec2(968.0f, 700.0)                   // 拼图板的初始位置

#define FadeIn_TIME                   2.0f                                  // 物品淡入时间
#define FadeOut_TIME                  2.0f                                  // 物品淡出时间

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f

#define MOVE_ACTOR_TIME                 6.0f                                // 分享界面ok过后主角移动时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define COLORLAYER_TAG                  300

Round001_06::Round001_06() {}

Round001_06::~Round001_06()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    
    GafAssetCache::getInstance()->removeGafAssetForKey(SNAKE_GAF_FILE);
}

//
Round001_06* Round001_06::createWithRoundIndex(const int roundIndex,
                                               StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round001_06();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene, pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

bool Round001_06::initWithRoundIndex(const int roundIndex,
                                     StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pPuzzleRect = Rect(1065.0f, 365.0f, 190.0f, 190.0f);    //设置拼图区域
    appendAsyncLoadTexture(PUZZLE_BORAD_FULL_PNG);
    appendRemovedTexture(PUZZLE_BORAD_FULL_PNG);
    appendAsyncLoadGaf(SNAKE_GAF_FILE);
    
    preLoadRes();
    return true;
}

void Round001_06::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    // 完整拼图板
    m_pPuzzleBoardFull = Sprite::create(PUZZLE_BORAD_FULL_PNG);
    addChild(m_pPuzzleBoardFull, PUZZLE_BORAD_FULL_ZORDER);
    m_pPuzzleBoardFull->setPosition(PUZZLE_BOARD_INIT_POS);
    m_pPuzzleBoardFull->setVisible(false);
    
    // batch
    m_pPuzzleNode = SpriteBatchNode::create(PUZZLE_IMAGE_FILE);
    addChild(m_pPuzzleNode);
    
    // 拼图块
    float lx = 700; float offsetX = 250; float ly = 140;
    char ptBuffer[64];
    for (unsigned long i = 0; i < 3; i++)
    {
        sprintf(ptBuffer, "SL-GK01-4-DA%02ld.png",i+1);
        m_pPuzzleChoosePiece[i] = Sprite::createWithSpriteFrameName(ptBuffer);
        m_pPuzzleNode->addChild(m_pPuzzleChoosePiece[i], PUZZLE_NODE_ZORDER);
        m_pPuzzleChoosePiece[i]->setUserData((void*)i);
        m_pPuzzleChoosePiece[i]->setOpacity(0);
        m_pPuzzleChoosePiece[i]->runAction(FadeIn::create(FadeIn_TIME));
    }
    
    // 随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3)
    {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pPuzzleChoosePiece[all]->setPosition(Vec2(lx + offsetX * Index, ly));
        m_originalPoints[all] = Vec2(lx + offsetX * Index, ly);
        all++;
    } /*while*/
    
    // 挡路蛇
    m_pSnakeActor = Actor::createWithGafConfigFile(SNAKE_GAF_CONFIG_FILE);
    m_pSnakeActor->playWithSequenceAliasName(SNAKE_TRIGGER,true);
    addChild(m_pSnakeActor, SNAKE_ZORDER);
    m_pSnakeActor->setPosition(SNAKE_INIT_POS);
    
    // 拼图板
    m_pPuzzleBoard = Sprite::createWithSpriteFrameName(PUZZLE_BOARD_PNG);
    m_pPuzzleNode->addChild(m_pPuzzleBoard, PUZZLE_NODE_BOARD_ZORDER);
    m_pPuzzleBoard->setPosition(PUZZLE_BOARD_INIT_POS);
    m_pPuzzleBoard->setOpacity(0);
    m_pPuzzleBoard->runAction(FadeIn::create(FadeIn_TIME));
}

void Round001_06::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        auto boundingBox = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (boundingBox.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            m_pPuzzleNode->reorderChild(target, PUZZLE_NODE_ZORDER + 1);
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
        target->setScale(1.0f);
        m_pPuzzleNode->reorderChild(target, PUZZLE_NODE_ZORDER);
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        
        unsigned long dragNumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        if (m_pPuzzleRect.containsPoint(touchPoint))
        {
            if (dragNumber == 2) { //答题正确
                isCorrect = true;
            }
        }
        
        if (isCorrect) { /* 答对题目*/
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());    //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            m_pPuzzleBoardFull->setVisible(true);
            m_pPuzzleBoard->setVisible(false);
            target->setVisible(false);
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            this->celebrate(CELEBRATE_DELAY); //庆祝
            for (unsigned int i = 0; i < 3; i++) {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPuzzleChoosePiece[i]);
            } /*for*/
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                //答错时主角说话
                playLeadAnsErrEff();
            }
            //答错时主角说话
            playLeadAnsErrEff();
            
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[dragNumber])),CallFunc::create([this,target,dragNumber]{
                target->setPosition(m_originalPoints[dragNumber]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event) {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pPuzzleChoosePiece[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pPuzzleChoosePiece[i]);
    }
}

void Round001_06::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
}

void Round001_06::celebrate(const float celebrateDelay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*
 *  淡出隐藏拼图
 */
void Round001_06::fadeOutPuzzle()
{
    m_pPuzzleBoardFull->runAction(FadeOut::create(FadeOut_TIME));
    for (unsigned int i = 0; i < 3; i++)
    {
        m_pPuzzleChoosePiece[i]->runAction(FadeOut::create(FadeOut_TIME));
    } /*for*/
}

/*
 *  淡出隐藏拼图的同时切换蛇的动作
 */
void Round001_06::changeSnakeAction()
{
    if (m_pSnakeActor != nullptr) {
        m_pSnakeActor->playWithSequenceAliasName(SNAKE_STANDBY);
    }
}

void Round001_06::endRound() {}

//回合结束调用
void Round001_06::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    } else {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    
    fadeOutPuzzle();                   //淡出隐藏拼图
    changeSnakeAction();              //改变蛇动作
    
    //
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000.0f, 0.0f), true);
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round001_06::fadeOutLayer), 2.0f);
}

//渐变变黑退出
void Round001_06::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    // 停止分享音效
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

/* 社交分享回调 */
void Round001_06::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round001_06::roundEndShareCallBack, this, true));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round001_06::roundEndShareCallBack, this, false));
}


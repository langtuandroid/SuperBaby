//
//  Round014_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/12.
//
//

#include "Round014_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage014/XQ-GK02-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage014/XQ-GK02-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define PIECE_ZORDER                    3
#define TARGET_POS                      Vec2(414.0f,759.0f)
#define TARGET_ZORDER                   1
#define YELLOW_PIECE_POS                Vec2(603.0f,982.0f)
#define YELLOW_PIECE_ZORDER             2
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define CELEBRATE_DELAY                 5.0f
#define BEFORE_ZORDER                   2
#define BEFORE_PNG_FILE                 "res/stage014/XQ-GK02-BG01_001.png"     //前景层图片

/*
*  constructor
*/
Round014_01::Round014_01()
:m_InitAniIndex(0)
{
    m_piecesPos[0] = Vec2(1040, 1043);
    m_piecesPos[1] = Vec2(1396, 911);
    m_piecesPos[2] = Vec2(1500, 1171);
}

/*
 *  destructor
 */
Round014_01::~Round014_01()
{
    cleanUp();
}

//
void Round014_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    Director::getInstance()->getTextureCache()->removeTextureForKey(BEFORE_PNG_FILE);
}

//
Round014_01* Round014_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round014_01();
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
bool Round014_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto beforeLayer = Sprite::create(BEFORE_PNG_FILE);
    beforeLayer->setAnchorPoint(Vec2(0.0f, 0.0f));
    beforeLayer->setPosition(Vec2(-740.0f, 0.0f));
    addChild(beforeLayer, BEFORE_ZORDER);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++)
    {
        sprintf(buffer, "XQ-GK02-1-DA%02ld.png",i+1);
        m_pPieces[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pPieces[i]->setUserData((void*)i);
        m_pBN->addChild(m_pPieces[i],PIECE_ZORDER);
        m_pPieces[i]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
    } /*for*/
    
    m_pTargetImageView = ImageView::create("XQ-GK02-1-TM01-1.png",ui::Widget::TextureResType::PLIST);
    m_pTargetImageView->setPosition(TARGET_POS);
    this->addChild(m_pTargetImageView, TARGET_ZORDER);

    m_pYellowPiece = Sprite::createWithSpriteFrameName("XQ-GK02-1-IT.png");
    m_pYellowPiece->setPosition(YELLOW_PIECE_POS);
    m_pBN->addChild(m_pYellowPiece, YELLOW_PIECE_ZORDER);
    m_pYellowPiece->setOpacity(0);
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int pieceIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[pieceIndex]) continue;
        selected[pieceIndex] = true;
        m_pPieces[all]->setPosition(m_piecesPos[pieceIndex]);
        m_piecesEndPos[all] = m_piecesPos[pieceIndex];
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
void Round014_01::onEnterRound()
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

    this->schedule(CC_SCHEDULE_SELECTOR(Round014_01::scaleChangeSchedule), 1.0f, -1, 0);
}

//依次改变碎片大小
void Round014_01::scaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_01::scaleChangeSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round014_01::fadeYellowPiece), 0.85f, -1, 0);
        return;
    }
    
    m_pPieces[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.5f, 1.2f),ScaleTo::create(0.5f, 1.0f), NULL));
}

//渐变显隐黄色目标块
void Round014_01::fadeYellowPiece(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round014_01::fadeYellowPiece));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pYellowPiece->runAction(Sequence::create(FadeIn::create(0.4f),FadeOut::create(0.4f),CallFunc::create([this]{
        m_pYellowPiece->setOpacity(0);
        m_InitAniIndex++;
    }), NULL));
}


/* 开启触摸 */
void Round014_01::openTouch()
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
        long index = (long)target->getUserData();

        target->setScale(1.0f);
        Rect rect = Rect(0.0f, 0.0f, m_pYellowPiece->getContentSize().width, m_pYellowPiece->getContentSize().height);
        if (rect.containsPoint(m_pYellowPiece->convertTouchToNodeSpace(touch))) /* 答题正确 */
        {
            if (index == 2) {
                target->setVisible(false);
                m_pTargetImageView->loadTexture("XQ-GK02-1-TM01-2.png",ui::Widget::TextureResType::PLIST);
                fadeOutPieces();        //渐隐剩余碎片
                // 播放答对特效
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                // 播放答对音效
                playAnswerCorrectEffect();
                
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPieces[i]);
                } /*for*/
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_piecesEndPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_piecesEndPos[index]);
                }), NULL));
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_piecesEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_piecesEndPos[index]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_piecesEndPos[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pPieces[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pPieces[i]);
    } /*for*/
}

//渐变隐藏剩余碎片
void Round014_01::fadeOutPieces()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pPieces[i]->runAction(FadeOut::create(0.3f));
    }
}

/*
 *  庆祝
 */
void Round014_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/* 回合结束回调 */
void Round014_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

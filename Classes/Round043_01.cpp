//
//  Round043_01.cpp
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#include "Round043_01.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define BN_PLIST_FILE                  "res/stage043/GC-GC07-IT.plist"              // BN的plist 文件
#define BN_PNG_FILE                    "res/stage043/GC-GC07-IT.png"               // BN的 png 文件

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间
#define BOARD_PNG_FILE                  "GC-GC07-1-IT.png"
#define BOARD_POS                       Vec2(1148,-354)
#define BOARD_ZORDER                    1
#define COLORS_ZORDER                   3
#define HINTS_ZORDER                    2
#define HINT_LIGHT_PNG_FILE             "GC-GC07-1-TS.png"
#define HINT_LIGHT_POS                  Vec2(1600,990)
#define HINT_LIGHT_ZORDER               4
#define DOOR_ZORDER                     1

/*
 *  constructor
 */
Round043_01::Round043_01()
{
    m_hintsPos[0] = Vec2(1453, 968);
    m_hintsPos[1] = Vec2(1603, 1005);
    m_hintsPos[2] = Vec2(1751, 966);

    m_colorsPos[0] = Vec2(928,-300);
    m_colorsPos[1] = Vec2(1148,-300);
    m_colorsPos[2] = Vec2(1368,-300);
    
    m_leftColorsPos[0] = Vec2(831, 968);
    m_leftColorsPos[1] = Vec2(982, 1005);
    m_leftColorsPos[2] = Vec2(1130, 966);
    
    m_doorsPos[0] = Vec2(938, 762);
    m_doorsPos[1] = Vec2(1559, 762);
    
    m_colorsStr[0] = "GC-GC07-1-TX01-C.png";
    m_colorsStr[1] = "GC-GC07-1-TX02-C.png";
    m_colorsStr[2] = "GC-GC07-1-TX03-C.png";
}

/*
 *  destructor
 */
Round043_01::~Round043_01()
{
    cleanUp();
}

//
void Round043_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    for (unsigned int i = 0; i < 3; i++) {
        CC_SAFE_DELETE(m_pImages[i]);
    }
}

//
Round043_01* Round043_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round043_01();
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
bool Round043_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    char buffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GC07-1-TX%02d.png",i+1);
        m_pHintsImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        m_pHintsImageView[i]->setPosition(m_hintsPos[i]);
        this->addChild(m_pHintsImageView[i],HINTS_ZORDER);
    }
    
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(buffer, "GC-GC07-1-TM02.png");
        m_pDoorsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pDoorsSp[i]->setPosition(m_doorsPos[i]);
        this->addChild(m_pDoorsSp[i],DOOR_ZORDER);
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GC07-1-TX%02d-C.png",i+1);
        m_pLeftColorsSp[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pLeftColorsSp[i]->setPosition(m_leftColorsPos[i]);
        this->addChild(m_pLeftColorsSp[i],HINTS_ZORDER);
    }
    
    
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GC07-1-DA%02ld.png",i+1);
        m_pColors[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pColors[i]->setUserData((void*)i);
        this->addChild(m_pColors[i],COLORS_ZORDER);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randIndex]) continue;
        selected[randIndex] = true;
        m_pColors[all]->setPosition(m_colorsPos[randIndex]);
        m_colorsEndPos[all] = Vec2(m_colorsPos[randIndex].x, m_colorsPos[randIndex].y+550);
        all++;
    } /*while*/
    
    m_pColorBoard = Sprite::createWithSpriteFrameName(BOARD_PNG_FILE);
    m_pColorBoard->setPosition(BOARD_POS);
    this->addChild(m_pColorBoard, BOARD_ZORDER);
    
    m_pHintLightSp = Sprite::createWithSpriteFrameName(HINT_LIGHT_PNG_FILE);
    m_pHintLightSp->setPosition(HINT_LIGHT_POS);
    m_pHintLightSp->setOpacity(0);
    this->addChild(m_pHintLightSp, HINT_LIGHT_ZORDER);
    
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "res/stage043/GC-GC07-1-TX%02d.png",i+1);
        m_pImages[i] = new Image();
        m_pImages[i]->initWithImageFile(buffer);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }

    return true;
}

// 进入本回合
void Round043_01::onEnterRound()
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
    upColorsBoard();
}
//上升颜色板
void Round043_01::upColorsBoard()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pColors[i]->runAction(MoveBy::create(0.5f, Vec2(0.0f, 550.0f)));
    }
    m_pColorBoard->runAction(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 550.0f)),CallFunc::create([this]{
        this->schedule(CC_SCHEDULE_SELECTOR(Round043_01::changeScaleColorsSchedule), 0.8f, -1, 0);   //
    }), NULL));
}

//依次放大颜色
void Round043_01::changeScaleColorsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round043_01::changeScaleColorsSchedule));
        showHintLightSp();
        return;
    }
    m_pColors[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.4f, 1.1f),ScaleTo::create(0.4f, 1.0f), NULL));
}

//显示高亮提示
void Round043_01::showHintLightSp()
{
    m_pHintLightSp->runAction(Sequence::create(FadeIn::create(0.3f),
                                               FadeOut::create(0.3f),
                                               FadeIn::create(0.3f),
                                               FadeOut::create(0.3f),
                                               CallFunc::create([this]{
                                                    m_jumped = true;
                                                    if (m_opentouched) {
                                                        openTouch();
                                                    }
                                                }), NULL));
}

/* 开启触摸 */
void Round043_01::openTouch()
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
            target->setLocalZOrder(10);
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
        bool isCorrect = false;
        int hintIndex = 0;
        for (int i = 0; i < 3; i++) {
            auto touchPoint = m_pHintsImageView[i]->convertTouchToNodeSpace(touch);
            Rect rect = Rect(0.0f, 0.0f, m_pHintsImageView[i]->getContentSize().width, m_pHintsImageView[i]->getContentSize().height);
            if (rect.containsPoint(touchPoint)) {
                unsigned char *data = m_pImages[i] ->getData();
                
                int pa = 4 * ((m_pImages[i]->getHeight() - (int)(touchPoint.y) - 1) * m_pImages[i]->getWidth() + (int)(touchPoint.x)) + 3;
                unsigned int ap = data[pa];
                if (ap > 0) {
                    if (index == i) {
                        isCorrect = true;
                        hintIndex = i;
                    }
                    break;
                } else {
                    continue;
                }
            }
        } /*for*/
        if (isCorrect) {
            m_curCorrectNum++;
            target->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.15f, 1.0f),FadeOut::create(0.1f),CallFunc::create([this,hintIndex]{
                m_pHintsImageView[hintIndex]->loadTexture(m_colorsStr[hintIndex],ui::Widget::TextureResType::PLIST);
            }), NULL));
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            if (m_curCorrectNum == 3) {
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }else
            {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
            }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_colorsEndPos[index])),CallFunc::create([this,target,index]{
                target->setLocalZOrder(COLORS_ZORDER);
                target->setPosition(m_colorsEndPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pColors[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pColors[i]);
    } /*for*/
}

//下降颜色板
void Round043_01::downColorsBoard()
{
    m_pColorBoard->runAction(Sequence::create(MoveBy::create(0.4f, Vec2(0.0f, -550.0f)),CallFunc::create([this]{
        if (m_completeCB) {
            m_completeCB();
        }
    }), NULL));
}

/*
 *  庆祝
 */
void Round043_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round043_01::endRound()
{
    downColorsBoard();
}

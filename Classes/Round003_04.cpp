//
//  Round003_04.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round003_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define TREE_PNG_FILE               "res/stage003/SL-GK03-2-TM01.png"   // 树
#define TREE_BLACK_FILE             "res/stage003/SL-GK03-2-TM06.png"   // 用于表示树内部的黑色精灵
#define TREE_PNG_FILE7              "res/stage003/SL-GK03-2-TM07.png"   // 用于遮挡主角

#define SL_MAP_BG_FILE              "res/stage003/SL-MAP-BG.plist"
#define SL_MAP_BGPNG_FILE           "res/stage003/SL-MAP-BG.png"

// 门
#define DOOR_FRAMENAME              "SL-GK03-2-TM02.png"
// star blue
#define STARBG_FRAMENAME            "SL-GK03-2-TM04.png"
// star yellow
#define STAR_FRAMENAME              "SL-GK03-2-TM05.png"

#define TREE_POSX                   1800.0f
#define TREE_POSY                   820.0f

#define DOOR_TAG                    101
#define STAR_BG_TAG                 102
#define OVER_ACTOR_TAG              103

#define CORRECT_ANSWER              1
#define MOV_TIME                    0.2f                                    // 答错回原位置移动时间

/*
 *  constructor
 */
Round003_04::Round003_04() {}

/*
 *  destructor
 */
Round003_04::~Round003_04()
{
    m_pStageScene->removeOBLeaderNode(OVER_ACTOR_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round003_04* Round003_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round003_04();
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
bool Round003_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(TREE_PNG_FILE);
    appendAsyncLoadTexture(TREE_BLACK_FILE);
    appendAsyncLoadTexture(SL_MAP_BGPNG_FILE);
    appendAsyncLoadTexture(TREE_PNG_FILE7);
    appendRemovedTexture(TREE_PNG_FILE);
    appendRemovedTexture(TREE_BLACK_FILE);
    appendRemovedTexture(TREE_PNG_FILE7);
    
    preLoadRes();
    return true;
}

//
void Round003_04::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    m_pOverActorNode = Node::create();
    m_pStageScene->addOverLeaderNode(m_pOverActorNode, this->getPosition(), OVER_ACTOR_TAG);
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(SL_MAP_BG_FILE);
    
    float posx = 0.0f, posy = 0.0f, offsetx = 0.0f;
    // 树
    auto treeSp = Sprite::create(TREE_PNG_FILE);
    addChild(treeSp, 3);
    treeSp->setPosition(Vec2(TREE_POSX, TREE_POSY));
    
    // 门
    auto doorSp = Sprite::createWithSpriteFrameName(DOOR_FRAMENAME);
    doorSp->setPosition(Vec2(1420.0f, 480.0f));
    addChild(doorSp, 2, DOOR_TAG);
    
    // 树洞阴影
    auto treeBlackSp = Sprite::create(TREE_BLACK_FILE);
    treeBlackSp->setPosition(Vec2(1460.0f, 460.0f));
    addChild(treeBlackSp, 1);
    
    m_pSBN = SpriteBatchNode::create(SL_MAP_BGPNG_FILE);
    addChild(m_pSBN, 11);
    char szBuffer[32];
    posx = 740.0f;posy = 130.0f;offsetx = 230.0f;
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(szBuffer, "SL-GK03-2-DA%02ld.png", i + 1);
        m_pQuestionSps[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pSBN->addChild(m_pQuestionSps[i], 2);
        m_pQuestionSps[i]->setUserData((void*)i);
    } /*for*/
    
    //随机
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4)
    {
        unsigned int Index = static_cast<unsigned int>(rand() % 4);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pQuestionSps[all]->setPosition(Vec2(posx + offsetx*Index, posy));
        m_originalPoints[all] = Vec2(posx+offsetx*Index, posy);
        all++;
    } /*while*/
    
    auto sp_starbg = Sprite::createWithSpriteFrameName(STARBG_FRAMENAME);
    sp_starbg->setPosition(Vec2(1380.0f, 900.0f));
    this->addChild(sp_starbg, 10, STAR_BG_TAG);
    
    // 添加用于遮挡主角的树皮，达到主角从树中穿过的效果
    m_pTree7Sp = Sprite::create(TREE_PNG_FILE7);
    m_pOverActorNode->addChild(m_pTree7Sp, 1);
    m_pTree7Sp->setPosition(Vec2(TREE_POSX + 100.0f, TREE_POSY - 305.0f));
    m_pTree7Sp->setVisible(false);
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(SL_MAP_BG_FILE);
        m_pStageScene->appendRemovedTexture(SL_MAP_BGPNG_FILE);
    }
}

//
void Round003_04::onEnterRound()
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
}

/* 开启触摸 */
void Round003_04::openTouch()
{
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            long index = (long)target->getUserData();
            m_originalPoint = m_originalPoints[index];
            target->setPosition(m_originalPoint);
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            m_pSBN->reorderChild(target, 3);
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
        auto dragTarger = (Sprite*)this->getChildByTag(STAR_BG_TAG);
        long selIndex = (long)target->getUserData();
        bool isCorrect = false;
        
        target->setScale(1.0f);
        m_pSBN->reorderChild(target, 2);
        
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        Rect rect = dragTarger->getBoundingBox();
        if (rect.containsPoint(touchPoint))
        {
            if (selIndex == CORRECT_ANSWER) /*答对了*/
            {
                isCorrect = true;
                this->removeChildByTag(STAR_BG_TAG);
                auto sp_star = Sprite::createWithSpriteFrameName(STAR_FRAMENAME);
                sp_star->setPosition(Vec2(1380.0f, 900.0f));
                m_pOverActorNode->addChild(sp_star, 2);
                
                target->setPosition(Vec2(-10000.0f, 0.0f));
                // remove listener
                auto eventDisptacher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 4; i++) {
                    eventDisptacher->removeEventListenersForTarget(m_pQuestionSps[i]);
                } /*for*/
                
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
                celebrate(1.5f);
            }
        }
        
        if (!isCorrect)
        {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1)
            {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB)
            {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[selIndex])),CallFunc::create([this,target,selIndex]{
                target->setPosition(m_originalPoints[selIndex]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
        target->setScale(1.0f);
        m_pSBN->reorderChild(target, 2);
    };
    auto eventDisptacher = Director::getInstance()->getEventDispatcher();
    eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pQuestionSps[0]);
    for (unsigned int i = 1; i < 4; i++) {
        eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pQuestionSps[i]);
    } /*for*/
}

//
void Round003_04::celebrate(const float delay)
{
    if (m_celebrateCB)
    {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round003_04::endRound()
{
    // 门上升
    auto doorSp = (Sprite*)getChildByTag(DOOR_TAG);
    auto doorUpCompletedCB = CallFunc::create([this]
    {
        m_pTree7Sp->setVisible(true);
        for (unsigned int i = 0; i < 4; i++)
        {
            m_pQuestionSps[i]->runAction(FadeOut::create(0.4f));
        } /*for*/
        if (m_completeCB)
        {
            m_completeCB();
        }
    });
    doorSp->runAction(Sequence::create(MoveBy::create(3.0f, Vec2(0.0f, 550.0f)), doorUpCompletedCB, NULL));
}


//
//  Round025_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#include "Round025_03.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define BN_PNG_FILE                    "res/stage025/HY-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值


// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FadeOut_TIME                    2                                       // 数字淡出时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define TREE_ZORDER                     1                                       //树的z值
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(224,1026)
#define BOGUANG_GAF_ZORDER              1

/*
 *  constructor
 */
Round025_03::Round025_03() {
    
    m_treesPos[0] = Vec2(467, 401);
    m_treesPos[1] = Vec2(1062, 413);
    m_treesPos[2] = Vec2(1629, 421);
}

/*
 *  destructor
 */
Round025_03::~Round025_03()
{
    cleanUp();
}

//
Round025_03* Round025_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round025_03();
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
bool Round025_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK01-3-DA%02ld.png",i+1);
        m_pTrees[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pTrees[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        m_pBN->addChild(m_pTrees[i], TREE_ZORDER);
        m_pTrees[i]->setUserData((void*)i);
    }
    
    for (int i = 0; i < 3; i++) {
        sprintf(buffer, "res/stage025/HY-GK01-3-DA%02d.png",i+1);
        m_pTreesImages[i] = new Image();
        m_pTreesImages[i]->initWithImageFile(buffer);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int treeIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[treeIndex]) continue;
        selected[treeIndex] = true;
        m_pTrees[all]->setPosition(m_treesPos[treeIndex]);
        all++;
    } /*while*/
    
    return true;
}

// 进入本回合
void Round025_03::onEnterRound()
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
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round025_03::scaleChangeTreeSchedule), 0.3f, -1, 0);
}

//回合开始改变树的尺寸
void Round025_03::scaleChangeTreeSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_03::scaleChangeTreeSchedule));
        return;
    }
    m_pTrees[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//
void Round025_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    for (unsigned int i = 0; i < 3; i++) {
        CC_SAFE_DELETE(m_pTreesImages[i]);
    }
}

/* 开启触摸 */
void Round025_03::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        for (int i = 0; i < 3; i++)
        {
            if (m_pTrees[i]->getBoundingBox().containsPoint(touchPoint)) {
                unsigned char *data = m_pTreesImages[i] ->getData();
                Vec2 touchPointInSprite = m_pTrees[i]->convertTouchToNodeSpace(touch);
                
                int pa = 4 * ((m_pTreesImages[i]->getHeight() - (int)(touchPointInSprite.y) - 1) * m_pTreesImages[i]->getWidth() + (int)(touchPointInSprite.x)) + 3;
                unsigned int ap = data[pa];
                if (ap > 0) {
                    m_pTrees[i]->setScaleY(1.1f); //改变scale和改变颜色
                    m_curSelIndex = i;
                    return true;
                } else {
                    continue;
                }
            }
        } /*for*/
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        m_pTrees[m_curSelIndex]->setScaleY(1.0f);
        if (m_curSelIndex == 0) {   //答对
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            celebrate(CELEBRATE_DELAY);
            for (unsigned int i = 0; i < 3; i++) {
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pTrees[i]);
            } /*for*/
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
        }
    };

    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pTrees[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pTrees[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round025_03::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/*  回合结束回调 */
void Round025_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
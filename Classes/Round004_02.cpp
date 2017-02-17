//
//  Round004_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/3/27.
//
//

#include "Round004_02.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define TENT_BN_ZORDER                     2                                               //帐篷批处理节点Z值
#define TENT_FRONT_ZORDER                  2                                              //单个帐篷前面一排Z值
#define TENT_BEHIND_ZORDER                 1                                              //单个帐篷后面一排Z值
#define CELEBRATE_DELAY                    5.0f                                          // 庆祝延时
#define TENT_CHANGE_SCALE_TIME             0.2f                                          //帐篷放大缩小时间

/*
 *  constructor
 */
Round004_02::Round004_02():
m_curSelIndex(-1)
{
    //初始化帐篷位置
    m_pTentsPos[0] = Vec2(874, 420);
    m_pTentsPos[1] = Vec2(1124, 450);
    m_pTentsPos[2] = Vec2(1324, 410);
    m_pTentsPos[3] = Vec2(1524, 490);
    m_pTentsPos[4] = Vec2(1694, 430);
    
}

/*
 *  destructor
 */
Round004_02::~Round004_02() {
    cleanUp();
}

/*
 *  清理
 */

void Round004_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    for (unsigned int i = 0; i < 4; i++) {
        CC_SAFE_DELETE(m_pImages[i]);
    }
}

//
Round004_02* Round004_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round004_02();
    if (pRound && pRound->initWithRoundIndex(roundIndex,pStageScene,pos))
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
bool Round004_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    char zpBuffer[32];
    for (int i = 4; i >= 0; i--) {
        sprintf(zpBuffer, "res/stage004/SL-GK04-3-TM%02d.png",i+1);
        m_pImages[i] = new Image();
        m_pImages[i]->initWithImageFile(zpBuffer);
    }
    
    char nsBuffer[64];
    //被选择的数字
    for (int i = 4; i >= 0; i--) {
        sprintf(nsBuffer, "SL-GK04-3-TM%02d.png",i+1);
        m_pTents[i] = Sprite::createWithSpriteFrameName(nsBuffer);
        m_pTents[i]->setPosition(m_pTentsPos[i]);
        m_pTents[i]->setAnchorPoint(Vec2(0.5f, 0.0f));
        if (i == 3 || i == 1) {
            this->addChild(m_pTents[i], TENT_BEHIND_ZORDER);
        }else
        {
            this->addChild(m_pTents[i], TENT_FRONT_ZORDER);
        }
    }
    
    m_pTempTents[0] = m_pTents[0];
    m_pTempTents[1] = m_pTents[2];
    m_pTempTents[2] = m_pTents[4];
    m_pTempTents[3] = m_pTents[1];
    m_pTempTents[4] = m_pTents[3];
    
    m_pTempImages[0] = m_pImages[0];
    m_pTempImages[1] = m_pImages[2];
    m_pTempImages[2] = m_pImages[4];
    m_pTempImages[3] = m_pImages[1];
    m_pTempImages[4] = m_pImages[3];
    return true;
}

/* 开启触摸 */
void Round004_02::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        Vec2 touchPoint = target->convertTouchToNodeSpace(touch);
        
        for (int i = 0; i < 5; i++) {
            if (m_pTempTents[i]->getBoundingBox().containsPoint(touchPoint)) {
                unsigned char *data = m_pTempImages[i] ->getData();
                Vec2 touchPointInSprite = m_pTempTents[i]->convertTouchToNodeSpace(touch);
                
                int pa = 4 * ((m_pTempImages[i]->getHeight() - (int)(touchPointInSprite.y) - 1) * m_pTempImages[i]->getWidth() + (int)(touchPointInSprite.x)) + 3;
                unsigned int ap = data[pa];
                if (ap > 0) {
                    m_pTempTents[i]->runAction(ScaleTo::create(TENT_CHANGE_SCALE_TIME, 1.1f)); //改变scale和改变颜色
                    m_pTempTents[i]->setColor(Color3B(255, 255, 0));
                    m_curSelIndex = i;
                    return true;
                } else {
                    continue;
                }
            }
        } /*for*/
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        m_pTempTents[m_curSelIndex]->runAction(ScaleTo::create(TENT_CHANGE_SCALE_TIME, 1.0f));
        m_pTempTents[m_curSelIndex]->setColor(Color3B(255, 255, 255));
        if (m_curSelIndex == 3) { //答对
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            this->celebrate(CELEBRATE_DELAY);
            auto dispatcher = Director::getInstance()->getEventDispatcher();
            dispatcher->removeEventListenersForTarget(this);
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
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, this);
}

//开始回合
void Round004_02::onEnterRound()
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

//庆祝
void Round004_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

/*  回合结束回调 */
void Round004_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round004_02::processNpcCollect() {}


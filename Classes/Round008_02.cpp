//
//  Round008_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#include "Round008_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage008/SL-GK08-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage008/SL-GK08-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define LEFT_MOVE_OFFSETX              -1150                                     //左移距离
#define LEFT_MOVE_TIME                 1.2f                                         //左移时间
#define FOODS_ZORDER                   1                                         //食物的z值
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间
#define ANIMAL_ZORDER                  1                                       //animal的z值
#define FOODS_FADEOUT_TIME             0.5f                                    //食物到达目标区域渐变消失时间

/*
 *  constructor
 */
Round008_02::Round008_02()
:m_correctNumber(0),
m_InitAniIndex(0)
{
}

/*
 *  destructor
 */
Round008_02::~Round008_02() {
    cleanUp();
}

/*
 *  清理
 */

void Round008_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round008_02* Round008_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round008_02();
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
bool Round008_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //设置食物位置
    m_foodsPos[0] = Vec2(2165, 831);
    m_foodsPos[1] = Vec2(2427, 831);
    m_foodsPos[2] = Vec2(2710, 831);
    
    //设置动物位置
    m_animalsPos[0] = Vec2(8, 1231);
    m_animalsPos[1] = Vec2(327, 1251);
    m_animalsPos[2] = Vec2(601, 1231);
    
    //设置触摸区域
    m_animalsRects[0] = Rect(880, 330, 300, 250);
    m_animalsRects[1] = Rect(1220, 320, 230, 300);
    m_animalsRects[2] = Rect(1500, 320, 200, 300);
    
    //触摸区域中心点坐标
    m_centerRectsPos[0] = Vec2(1030, 455);
    m_centerRectsPos[1] = Vec2(1335, 470);
    m_centerRectsPos[2] = Vec2(1600, 470);
    
    appendAsyncLoadGaf("res/stage008/SL-GK08-2-MS01-D/SL-GK08-2-MS01-D.gaf");
    appendAsyncLoadGaf("res/stage008/SL-GK08-2-MS02-D/SL-GK08-2-MS02-D.gaf");
    appendAsyncLoadGaf("res/stage008/SL-GK08-2-MS03-D/SL-GK08-2-MS03-D.gaf");
    preLoadRes();
    return true;
}

//
void Round008_02::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    
    // 纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载食物
    char swBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(swBuffer, "SL-GK08-2-DA%02ld.png",i+1);
        m_pFoods[i] = Sprite::createWithSpriteFrameName(swBuffer);
        m_pFoods[i]->setUserData((void*)i);
        m_pBN->addChild(m_pFoods[i], FOODS_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pFoods[all]->setPosition(m_foodsPos[Index]);
        m_originalPoints[all] = Vec2(m_foodsPos[Index].x-1150, m_foodsPos[Index].y);
        all++;
    } /*while*/
    
    
    //加载动物
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(swBuffer, "res/stage008/SL-GK08-2-MS%02d-D/SL-GK08-2-MS%02d-D.gaf",i+1,i+1);
        m_pAnimalsActor[i] = Actor::createWithGafFile(swBuffer);
        m_pAnimalsActor[i]->setPosition(m_animalsPos[i]);
        addChild(m_pAnimalsActor[i], ANIMAL_ZORDER);
    }
}

//开始回合
void Round008_02::onEnterRound()
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
    //渐变出现钱币答案
    this->schedule(CC_SCHEDULE_SELECTOR(Round008_02::leftMoveSchedule), LEFT_MOVE_TIME - 0.5f, -1, 0.0f);
}

//
void Round008_02::leftMoveSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_02::leftMoveSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    MoveBy* move = MoveBy::create(LEFT_MOVE_TIME, Vec2(LEFT_MOVE_OFFSETX, 0.0f));
    m_pFoods[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//庆祝
void Round008_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(true);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}


/* 开启触摸 */
void Round008_02::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
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
    listenerRight->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号
        if (m_animalsRects[0].containsPoint(touchPoint)) {
            if (!isCorrect && clicknumber == 0) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 0;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        if (m_animalsRects[1].containsPoint(touchPoint)) {
            
            if (!isCorrect && clicknumber == 2) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 1;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        if (m_animalsRects[2].containsPoint(touchPoint)) {
            
            if (!isCorrect && clicknumber == 1) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 2;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            target->setPosition(m_centerRectsPos[curAreaNumber]);       //将食物放置在目标区中点
            target->runAction(FadeOut::create(FOODS_FADEOUT_TIME));
            if (m_correctNumber == 3) {   //全部答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
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
                this->celebrate(CELEBRATE_DELAY);
            } else {
                // 播放答对音效
                playAnswerCorrectEffect();
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    //答对时主角说话
                    playLeadAnsCorEff();
                }else
                {
                    //答对时主角说话
                    playLeadAnsCorEff();
                }
                if (m_celebrateCB) {            //答对庆祝
                    m_celebrateCB(false);
                }
            }
        } else {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pFoods[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pFoods[i]);
    }
}

/*  回合结束回调 */
void Round008_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


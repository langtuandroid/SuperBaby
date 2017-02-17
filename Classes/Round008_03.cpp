//
//  Round008_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/7.
//
//

#include "Round008_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage008/SL-GK08-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage008/SL-GK08-IT.png"             //BN的png文件
#define BN_ZORDER                      1                                         //BN的z值
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间
#define DOWN_MOVE_OFFSETY             -820                                     //下移距离
#define DOWN_MOVE_TIME                1.0f                                     //下移时间
#define UP_MOVE_OFFSETY               550                                     //上移距离
#define UP_MOVE_TIME                  1.0f                                     //上移时间
#define DOWN_ARROWS_ZORDER            2                                        //掉落箭头z值
#define ARROWS_ZORDER                 1                                        //箭头z值
#define YELLOW_FRAME_ZORDER           3                                        //黄色目标选框z值
#define BUBBLE_FROG_ZORDER            4                                        //青蛙气泡z值
#define Bubble_UP_DOWN_MOVE_TIME      0.3f                                     //气泡上下浮动一次时间
#define Bubble_DOWN_MOVE_OFFSETY      -5                                      //气泡向上浮动距离
#define Bubble_UP_MOVE_OFFSETY        5                                       //气泡向下浮动距离
#define YELLOW_FRAMS_SCALE_CHANGE_TIME             0.3f                        //黄色框放大缩小时间
#define ARROWS_MOVE_TIME              0.3f                                     //箭头移动时间
#define FROG_FADEOUT_TIME             0.5f                                    //青蛙气泡到达目标区域渐变消失时间

/*
 *  constructor
 */
Round008_03::Round008_03()
:m_correctNumber(0),
m_InitAniIndex(0),
m_yellowFrameIndex(0),
m_arrowIndex(0)
{}

/*
 *  destructor
 */
Round008_03::~Round008_03()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round008_03* Round008_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round008_03();
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
bool Round008_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //青蛙各个阶段坐标
    m_frogsPos[0] = Vec2(857, -166);
    m_frogsPos[1] = Vec2(1107, -324);
    m_frogsPos[2] = Vec2(1383, -166);
    m_frogsPos[3] = Vec2(1628, -321);
    
    //黄色目标框坐标
    m_yellowFramesPos[0] = Vec2(595, 1796);
    m_yellowFramesPos[1] = Vec2(935, 1796);
    m_yellowFramesPos[2] = Vec2(1267, 1796);
    m_yellowFramesPos[3] = Vec2(1578, 1796);
    
    //答对后黄色目标框坐标
    m_finishYellowFramesPos[0] = Vec2(595, 976);
    m_finishYellowFramesPos[1] = Vec2(935, 976);
    m_finishYellowFramesPos[2] = Vec2(1267, 976);
    m_finishYellowFramesPos[3] = Vec2(1578, 976);
    
    //下降箭头坐标（上层）
    m_downArrowsPos[0] = Vec2(768, 1796);
    m_downArrowsPos[1] = Vec2(1101, 1796);
    m_downArrowsPos[2] = Vec2(1423, 1796);
    
    //上层箭头坐标
    m_arrowsPos[0] = Vec2(768, 976);
    m_arrowsPos[1] = Vec2(1101, 976);
    m_arrowsPos[2] = Vec2(1423, 976);
    
    preLoadRes();
    return true;
}

//
void Round008_03::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    
    //纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //黄色选框
    char buffer[64];
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(buffer, "SL-GK08-3-TM01-1.png");
        m_pYellowFrames[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pYellowFrames[i]->setPosition(m_yellowFramesPos[i]);
        m_pBN->addChild(m_pYellowFrames[i], YELLOW_FRAME_ZORDER);
    }
    //答对后黄色选框
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(buffer, "SL-GK08-3-TM%02d.png",i+1);
        m_pFinishFrogs[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pFinishFrogs[i]->setPosition(m_finishYellowFramesPos[i]);
        m_pBN->addChild(m_pFinishFrogs[i], YELLOW_FRAME_ZORDER);
        m_pFinishFrogs[i]->setVisible(false);
    }
    
    //掉落箭头
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "SL-GK08-3-IT01-jiantou.png");
        m_pDownArrows[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pDownArrows[i]->setPosition(m_downArrowsPos[i]);
        m_pBN->addChild(m_pDownArrows[i], DOWN_ARROWS_ZORDER);
    }
    
    //箭头
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "SL-GK08-3-IT01-jiantou.png");
        m_pArrows[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pArrows[i]->setPosition(m_arrowsPos[i]);
        m_pBN->addChild(m_pArrows[i], ARROWS_ZORDER);
        m_pArrows[i]->setVisible(false);
    }
    
    //青蛙气泡
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "SL-GK08-3-DA%02ld.png",i+1);
        m_pFrogs[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pFrogs[i]->setUserData((void*)i);
        m_pBN->addChild(m_pFrogs[i], BUBBLE_FROG_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int Index = static_cast<unsigned int>(rand() % 4);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pFrogs[all]->setPosition(m_frogsPos[Index]);
        m_originalPoints[all] = Vec2(m_frogsPos[Index].x, m_frogsPos[Index].y+550);
        all++;
    } /*while*/
}

//开始回合
void Round008_03::onEnterRound()
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

    //青蛙气泡上移
    this->schedule(CC_SCHEDULE_SELECTOR(Round008_03::frogUpSchedule), UP_MOVE_TIME - 0.8f, -1, 0.0f);
}

//回合开始，黄色框左边进入
void Round008_03::downMoveSchedule(float dt)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_03::downMoveSchedule));
        m_InitAniIndex = 0;  //重置为0
        //箭头下移
        this->schedule(CC_SCHEDULE_SELECTOR(Round008_03::arrowsDownSchedule), DOWN_MOVE_TIME - 0.8f, -1, 0.0f);
        return;
    }
    MoveBy* move = MoveBy::create(DOWN_MOVE_TIME, Vec2(0.0f, DOWN_MOVE_OFFSETY));
    m_pYellowFrames[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}
//回合开始，箭头上面掉下
void Round008_03::arrowsDownSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_03::arrowsDownSchedule));
        m_InitAniIndex = 0;  //重置为0
        //黄色框和箭头一次放大缩小
        this->schedule(CC_SCHEDULE_SELECTOR(Round008_03::yellowFramesChangeScaleSchedule), UP_MOVE_TIME - 0.3f, -1, 0.0f);
        return;
    }
    MoveBy* move = MoveBy::create(DOWN_MOVE_TIME, Vec2(0.0f, DOWN_MOVE_OFFSETY));
    m_pDownArrows[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//黄色框和箭头依次放大缩小
void Round008_03::yellowFramesChangeScaleSchedule(float dt)
{
    if (m_InitAniIndex >= 7) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_03::yellowFramesChangeScaleSchedule));
        m_InitAniIndex = 0;  //重置为0
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    if (m_InitAniIndex%2 == 0) {
        m_pYellowFrames[m_yellowFrameIndex]->setColor(Color3B(255, 102, 204));
        m_pYellowFrames[m_yellowFrameIndex]->runAction(Sequence::create(ScaleTo::create(YELLOW_FRAMS_SCALE_CHANGE_TIME, 1.1f),ScaleTo::create(YELLOW_FRAMS_SCALE_CHANGE_TIME, 1.0f),CallFunc::create([this]{
            m_pYellowFrames[m_yellowFrameIndex]->setColor(Color3B(255, 255, 255));
            m_yellowFrameIndex++;
            m_InitAniIndex++;
        }), NULL));
        
    }else if (m_InitAniIndex%2 == 1)
    {
        m_pArrows[m_arrowIndex]->setVisible(true);
        m_pDownArrows[m_arrowIndex]->setColor(Color3B(255, 102, 204));
        m_pDownArrows[m_arrowIndex]->runAction(Sequence::create(MoveBy::create(ARROWS_MOVE_TIME, Vec2(10.0f, 0.0f)),MoveBy::create(ARROWS_MOVE_TIME, Vec2(-10.0f, 0.0f)),CallFunc::create([this]{
            m_pDownArrows[m_arrowIndex]->setColor(Color3B(255, 255, 255));
            m_pArrows[m_arrowIndex]->setVisible(false);
            m_arrowIndex++;
            m_InitAniIndex++;
        }), NULL));
    }
}

//回合开始，青蛙气泡升起
void Round008_03::frogUpSchedule(float dt)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_03::frogUpSchedule));
        m_InitAniIndex = 0;  //重置为0

        //青蛙气泡上下浮动
        this->schedule(CC_SCHEDULE_SELECTOR(Round008_03::moveFrogBubbleSchedule), UP_MOVE_TIME - 0.8f, -1, 0.0f);
        return;
    }
    MoveBy* move = MoveBy::create(UP_MOVE_TIME, Vec2(0.0f, UP_MOVE_OFFSETY));
    m_pFrogs[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//青蛙气泡上下浮动
void Round008_03::moveFrogBubbleSchedule(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round008_03::moveFrogBubbleSchedule));
        m_InitAniIndex = 0;
        //黄色选择框下移
        this->schedule(CC_SCHEDULE_SELECTOR(Round008_03::downMoveSchedule),  DOWN_MOVE_TIME - 0.8f, -1, 0.0f);
        return;
    }
    Sequence* seq = Sequence::create(EaseBackOut::create(MoveBy::create(Bubble_UP_DOWN_MOVE_TIME, Vec2(0.0f,Bubble_DOWN_MOVE_OFFSETY))),
                                     MoveBy::create(Bubble_UP_DOWN_MOVE_TIME, Vec2(0.0f, Bubble_UP_MOVE_OFFSETY)),
                                     MoveBy::create(Bubble_UP_DOWN_MOVE_TIME, Vec2(0.0f, Bubble_UP_MOVE_OFFSETY)),
                                     MoveBy::create(Bubble_UP_DOWN_MOVE_TIME, Vec2(0.0f, Bubble_DOWN_MOVE_OFFSETY)), NULL);
    m_pFrogs[m_InitAniIndex++]->runAction(RepeatForever::create(seq));
}

//庆祝
void Round008_03::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

/* 开启触摸 */
void Round008_03::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f, m_pYellowFrames[0]->getContentSize().width, m_pYellowFrames[0]->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号
        if (rect.containsPoint(m_pYellowFrames[0]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && clicknumber == 3) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 0;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pYellowFrames[1]->getContentSize().width, m_pYellowFrames[1]->getContentSize().height);
        if (rect.containsPoint(m_pYellowFrames[1]->convertTouchToNodeSpace(touch))) {
            
            if (!isCorrect && clicknumber == 1) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 1;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pYellowFrames[2]->getContentSize().width, m_pYellowFrames[2]->getContentSize().height);
        if (rect.containsPoint(m_pYellowFrames[2]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && clicknumber == 2) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 2;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        rect = Rect(0.0f, 0.0f, m_pYellowFrames[3]->getContentSize().width, m_pYellowFrames[3]->getContentSize().height);
        if (rect.containsPoint(m_pYellowFrames[3]->convertTouchToNodeSpace(touch))) {
            
            if (!isCorrect && clicknumber == 0) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 3;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            target->runAction(FadeOut::create(FROG_FADEOUT_TIME));     //渐变消失青蛙气泡
            m_pYellowFrames[curAreaNumber]->setVisible(false);         //隐藏黄色框
            m_pFinishFrogs[curAreaNumber]->setVisible(true);           //显示完成的黄色框
            if (m_correctNumber == 4) {   //全部答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
            } else {
                if (m_celebrateCB) {            //答对庆祝
                    m_celebrateCB(1);
                }
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pFrogs[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pFrogs[i]);
    }
}

/*  回合结束回调 */
void Round008_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

//
//  Round007_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round007_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define FOX_GAF_FILE                   "config/stage007/fox.json"                //狐狸gaf配置文件
#define FOX_GAF_POS                    Vec2(570,1386)                            //狐狸actor位置
#define FOX_GAF_ZORDER                 1                                         //狐狸actor的z值
#define BN_PNG_FILE                    "res/stage007/SL-GK07-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define YELLOWGRAPH_ZORDER             3                                         //黄色图形z值
#define YELLOWGRAPH_FINSH_ZORDER       2                                         //黄色完成状态图形z值
#define BLUEGRAPH_ZORDER               1                                         //蓝色图形z值
#define INIT_UP_TIME                   0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY                560.0f                                  // 上升动画偏移值
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间
#define FadeIn_TIME                    1                                       //目标蓝色图形淡入时间
#define WATERFALL_GAF_FILE             "res/stage007/SL-GK07pubu/SL-GK07pubu.gaf"
#define WATERFALL_TAG                  200
#define WATERFALL_POS                  Vec2(7108,1738)
/*
 *  constructor
 */
Round007_02::Round007_02()
:m_correctNumber(0),
m_InitAniIndex(0)
{
}

/*
 *  destructor
 */
Round007_02::~Round007_02() {
    cleanUp();
}

/*
 *  清理
 */
void Round007_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round007_02* Round007_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round007_02();
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
bool Round007_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //黄色图形坐标
    m_yellowGraphPos[0] = Vec2(914, -260);
    m_yellowGraphPos[1] = Vec2(1135, -260);
    m_yellowGraphPos[2] = Vec2(1343, -260);
    
    //蓝色图形坐标
    m_blueGraphPos[0] = Vec2(1125, 931);
    m_blueGraphPos[1] = Vec2(1125, 802);
    m_blueGraphPos[2] = Vec2(1125, 665);
    
    //完成答题状态黄色图形坐标
    m_yellowFinshGraphPos[0] = Vec2(1125, 931);
    m_yellowFinshGraphPos[1] = Vec2(1125, 802);
    m_yellowFinshGraphPos[2] = Vec2(1125, 665);
    
    appendAsyncLoadGaf(FOX_GAF_FILE);
    preLoadRes();
    return true;
}

void Round007_02::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    
    //加载狐狸actor
    m_pFoxActor = Actor::createWithGafConfigFile(FOX_GAF_FILE);
    m_pFoxActor->setPosition(FOX_GAF_POS);
    m_pFoxActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pFoxActor, FOX_GAF_ZORDER);
    
    //加载BN
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载完成状态黄色图形
    char txBuffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(txBuffer, "SL-GK07-2-DA%02d-1.png",i+1);
        m_pYellowFinshGraphs[i] = Sprite::createWithSpriteFrameName(txBuffer);
        m_pYellowFinshGraphs[i]->setPosition(m_yellowFinshGraphPos[i]);
        m_pYellowFinshGraphs[i]->setVisible(false);
        m_pBN->addChild(m_pYellowFinshGraphs[i], YELLOWGRAPH_FINSH_ZORDER);
    }
    //加载蓝色图形
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(txBuffer, "SL-GK07-2-TM%02d.png",i+1);
        m_pBlueGraphs[i] = Sprite::createWithSpriteFrameName(txBuffer);
        m_pBlueGraphs[i]->setPosition(m_blueGraphPos[i]);
        m_pBlueGraphs[i]->setOpacity(0);
        m_pBN->addChild(m_pBlueGraphs[i], BLUEGRAPH_ZORDER);
    }
    //加载黄色图形
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(txBuffer, "SL-GK07-2-DA%02ld.png",i+1);
        m_pYellowGraphs[i] = Sprite::createWithSpriteFrameName(txBuffer);
        m_pYellowGraphs[i]->setUserData((void*)i);
        m_pBN->addChild(m_pYellowGraphs[i], YELLOWGRAPH_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pYellowGraphs[all]->setPosition(m_yellowGraphPos[Index]);
        m_originalPoints[all] = Vec2(m_yellowGraphPos[Index].x, m_yellowGraphPos[Index].y+560);
        all++;
    } /*while*/
    
    m_pWaterfallActor = Actor::createWithGafFile(WATERFALL_GAF_FILE);
    m_pStageScene->addBelowLeaderNode(m_pWaterfallActor, WATERFALL_POS, WATERFALL_TAG);
}

//开始回合
void Round007_02::onEnterRound()
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
    fadeInBlueGraphs();    //淡入蓝色目标图形
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_02::graphsUpSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
}

//回合开场渐变出现蓝色目标图形
void Round007_02::fadeInBlueGraphs()
{
    for (unsigned int  i = 0; i < 3; i++) {
        m_pBlueGraphs[i]->runAction(FadeIn::create(FadeIn_TIME));
    }
}

//回合开场动画，图形上升动画调度
void Round007_02::graphsUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_02::graphsUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pYellowGraphs[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//全部答对后改变狐狸动作
void Round007_02::changeFoxActor()
{
    m_pFoxActor->playWithSequenceAliasName(TRIGGER,true);
}

//庆祝
void Round007_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
    
}

/* 开启触摸 */
void Round007_02::openTouch()
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
        if (rect.containsPoint(touchPoint)) {
            
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
        Rect rect = Rect(0.0f, 0.0f, m_pBlueGraphs[0]->getContentSize().width, m_pBlueGraphs[0]->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        Sprite* pSp = nullptr;
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号
        if (rect.containsPoint(m_pBlueGraphs[0]->convertTouchToNodeSpace(touch))) {
            pSp = m_pBlueGraphs[0];
            if (!isCorrect && clicknumber == 0) {
                isCorrect = true;
                curAreaNumber = 0;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBlueGraphs[1]->getContentSize().width, m_pBlueGraphs[1]->getContentSize().height);
        if (rect.containsPoint(m_pBlueGraphs[1]->convertTouchToNodeSpace(touch))) {
            pSp = m_pBlueGraphs[1];
            if (!isCorrect && clicknumber == 1) {
                isCorrect = true;
                curAreaNumber = 1;
            }
        }
        rect = Rect(0.0f, 0.0f, m_pBlueGraphs[2]->getContentSize().width, m_pBlueGraphs[2]->getContentSize().height);
        if (rect.containsPoint(m_pBlueGraphs[2]->convertTouchToNodeSpace(touch))) {
            pSp = m_pBlueGraphs[2];
            if (!isCorrect && clicknumber == 2) {
                isCorrect = true;
                curAreaNumber = 2;
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            m_correctNumber++;
            Vec2 point = pSp->getPosition();      //获得目标盒子的位置
            target->setPosition(point);       //将动物阴影放置在目标区
            m_pBlueGraphs[curAreaNumber]->setVisible(false);   //隐藏蓝色目标图形
            target->setVisible(false);                         //隐藏目标图形
            m_pYellowFinshGraphs[curAreaNumber]->setVisible(true);  //显示完成状态黄色图形
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            if (m_correctNumber == 3) {   //全部答对
                changeFoxActor();       //改变狐狸动作
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
            } else {
                if (m_celebrateCB) {            //答对庆祝
                    m_celebrateCB(1);
                }
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pYellowGraphs[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pYellowGraphs[i]);
    }
}

/*  回合结束回调 */
void Round007_02::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}
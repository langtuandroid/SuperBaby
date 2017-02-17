//
//  Round006_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/2.
//
//

#include "Round006_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define WORM_GAF_CONFIG_FILE                  "config/stage006/worm.json"        //虫的配置文件
#define WORM_GAF_POS                   Vec2(300.0f,1250.0f)                      //虫的初始位置
#define WORM_GAF_ZORDER                1                                         //虫的z值
#define COLOR_PNG_FILE                 "res/stage006/SL-GK06-IT.png"             //颜色纹理png图
#define COLOR_BN_ZORDER                2                                         //颜色纹理Z值
#define BOARD_ZORDER                   1                                         //木板z值
#define COLOR_ZORDER                   2                                         //颜色z值
#define BOARD_POS                      Vec2(1534,2242)                            //木板位置
#define DOWN_OFFSETY                   -1000.0f                                    //木板和颜色下降高度
#define BOARD_PNG_NAME                 "SL-GK06-3-IT01.png"                      //木板png名
#define INIT_DOWN_TIME                  2.0f                                    // 下降动画时间
#define INIT_UP_TIME                    3.0f                                    // 上升动画时间
#define INIT_UP_OFFSETY                 1000.0f                                  // 上升动画偏移值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间

/*
 *  constructor
 */
Round006_03::Round006_03()
{}

/*
 *  destructor
 */
Round006_03::~Round006_03() {
    cleanUp();
}

/*
 *  清理
 */

void Round006_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round006_03* Round006_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round006_03();
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
bool Round006_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;

    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //设置颜色初始位置
    m_colorPos[0] = Vec2(1320, 1954);
    m_colorPos[1] = Vec2(1534, 1954);
    m_colorPos[2] = Vec2(1740, 1954);
    
    appendAsyncLoadGaf("res/stage006/SL-GK06-3-MS-D+C/SL-GK06-3-MS-D+C.gaf");
    preLoadRes();
    return true;
}

void Round006_03::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    //创建BN
    m_pColorAndBoardBN = SpriteBatchNode::create(COLOR_PNG_FILE);
    addChild(m_pColorAndBoardBN,COLOR_BN_ZORDER);
    
    //创建颜色板
    m_pBoard = Sprite::createWithSpriteFrameName("SL-GK06-3-IT01.png");
    m_pBoard->setPosition(BOARD_POS);
    m_pColorAndBoardBN->addChild(m_pBoard, BOARD_ZORDER);
    
    char ysBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(ysBuffer, "SL-GK06-3-DA%02ld.png",i+1);
        m_pColors[i] = Sprite::createWithSpriteFrameName(ysBuffer);
        m_pColors[i]->setUserData((void*)i);
        m_pColorAndBoardBN->addChild(m_pColors[i], COLOR_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pColors[all]->setPosition(m_colorPos[Index]);
        m_colorEndPos[all] = Vec2(m_colorPos[Index].x, m_colorPos[Index].y-1000);
        all++;
    } /*while*/
    
    //加载gaf
    m_pWormActor = Actor::createWithGafConfigFile(WORM_GAF_CONFIG_FILE);
    m_pWormActor->setPosition(WORM_GAF_POS);
    m_pWormActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pWormActor, WORM_GAF_ZORDER);
    
    m_pHintSp = Sprite::createWithSpriteFrameName("SL-GK06-3-TS.png");
    m_pHintSp->setPosition(Vec2(1578, 375));
    m_pColorAndBoardBN->addChild(m_pHintSp, WORM_GAF_ZORDER);
    m_pHintSp->setOpacity(0);
}

// 开始回合
void Round006_03::onEnterRound()
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
    //颜色板下降
    colorBoardDown();
}

//颜色板下降
void Round006_03::colorBoardDown()
{
    m_pBoard->runAction(Sequence::create(MoveBy::create(INIT_DOWN_TIME, Vec2(0, DOWN_OFFSETY)),CallFunc::create([this]{
        this->schedule(CC_SCHEDULE_SELECTOR(Round006_03::changeScaleColorsSchedule), 0.6f, -1, 0);
    }), NULL));
    
    for (unsigned int i = 0; i < 3; i++) {
        m_pColors[i]->runAction(MoveBy::create(INIT_DOWN_TIME, Vec2(0, DOWN_OFFSETY)));
    }
}

//依次放大颜色
void Round006_03::changeScaleColorsSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round006_03::changeScaleColorsSchedule));
        fadeHint();
        return;
    }
    m_pColors[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.3f, 1.1f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//显示提示
void Round006_03::fadeHint()
{
    m_pHintSp->runAction(Sequence::create(FadeIn::create(0.3f),FadeOut::create(0.3f),FadeIn::create(0.3f),FadeOut::create(0.3f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round006_03::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintSp->getContentSize().width,
                         m_pHintSp->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        bool isCorrect = false;
        
        if (rect.containsPoint(m_pHintSp->convertTouchToNodeSpace(touch)))
        {
            if (clicknumber == 1) { //答对问题
                isCorrect = true;
                target->runAction(FadeOut::create(0.2f));
                colorBoardUp();        //颜色板上升
                changeWorm();          //改变虫动作
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pColors[i]);
                } /*for*/
            }
        }
        if (!isCorrect)
        {
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_colorEndPos[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_colorEndPos[clicknumber]);
            }), NULL));
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pColors[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pColors[i]);
    }
}

//选对后颜色板上升
void Round006_03::colorBoardUp()
{
    m_pBoard->runAction(Sequence::create(MoveBy::create(INIT_UP_TIME, Vec2(0, INIT_UP_OFFSETY)), NULL));
    
    for (unsigned int i = 0; i < 3; i++) {
        if (i != 1) {
            m_pColors[i]->runAction(MoveBy::create(INIT_UP_TIME, Vec2(0, INIT_UP_OFFSETY)));
        }
    }
}

//答对后改变虫的动作
void Round006_03::changeWorm()
{
    m_pWormActor->playWithSequenceAliasName(TRIGGER);
    m_pWormActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pWormActor->setVisible(false);         //播放完后隐藏
    });
}

//庆祝
void Round006_03::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/*  回合结束回调 */
void Round006_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

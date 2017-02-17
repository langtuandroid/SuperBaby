//
//  Round007_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round007_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage007/SL-GK07-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage007/SL-GK07-IT.png"             //BN的png文件
#define BN_ZORDER                      1                                         //BN的z值
#define BLACKANIMALS_ZORDER            5                                         //阴影动物z值
#define ANIMALS_ZORDER                 2                                         //动物z值
#define CYAN_FRAMES_ZORDER             1                                         //青色背景框z值
#define RED_SHINE_AREAS_ZORDER         3                                         //红色发光区z值
#define MARKS_ZORDER                   4                                         //问号z值
#define CYAN_FRAMES_PNG_NAME           "SL-GK07-1-IT-01.png"                     //青色背景框png
#define MARKS_PNG_NAME                 "HY-GK07-1-kuang.png"                     //问号png
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间
#define BLINK_TIME                     1                                        //闪烁时间
#define BLINK_NUMBER                   1                                        //闪烁次数
#define GAF_ZORDER                     3                                        //gaf的z值
#define BLACK_ANIMAL_SCALE             0.7f                                     //阴影动物缩放系数
#define INIT_DOWN_OFFSETY              -1120.0f                                // 下坠动画偏移值
#define INIT_DOWN_TIME                  0.8f                                    // 下坠动画时间

/*
 *  constructor
 */
Round007_01::Round007_01():
m_correctNumber(0)
{}

/*
 *  destructor
 */
Round007_01::~Round007_01() {
    cleanUp();
}

/*
 *  清理
 */
void Round007_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round007_01* Round007_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round007_01();
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
bool Round007_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //动物阴影坐标
    m_blackAnimalsPos[0] = Vec2(1145, 827);
    m_blackAnimalsPos[1] = Vec2(1424, 850);
    m_blackAnimalsPos[2] = Vec2(1709, 850);
    
    //动物坐标
    m_animalsPos[0] = Vec2(861, 504);
    m_animalsPos[1] = Vec2(1232, 504);
    m_animalsPos[2] = Vec2(1611, 504);
    
    //青色背景框坐标
    m_cyanFramesPos[0] = Vec2(950, 504);
    m_cyanFramesPos[1] = Vec2(1320, 504);
    m_cyanFramesPos[2] = Vec2(1690, 504);
    
    //问号坐标
    m_marksPos[0] = Vec2(1032, 500);
    m_marksPos[1] = Vec2(1398, 500);
    m_marksPos[2] = Vec2(1768, 500);
    
    //gaf坐标组
    m_gafPos[0] = Vec2(-165, 1270);
    m_gafPos[1] = Vec2(210, 1270);
    m_gafPos[2] = Vec2(585, 1270);
    
    appendAsyncLoadTexture(BN_PNG_FILE);
    appendAsyncLoadGaf("res/stage007/SL-GK07-1-TM01-C/SL-GK07-1-TM01-C.gaf");
    appendAsyncLoadGaf("res/stage007/SL-GK07-1-TM02-C/SL-GK07-1-TM02-C.gaf");
    appendAsyncLoadGaf("res/stage007/SL-GK07-1-TM03-C/SL-GK07-1-TM03-C.gaf");
    preLoadRes();
    return true;
}

void Round007_01::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    
    //纹理加载
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    
    //添加动物阴影
    char yyBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(yyBuffer, "SL-GK07-1-DA%02ld.png",i+1);
        m_pBlackAnimals[i] = Sprite::createWithSpriteFrameName(yyBuffer);
        m_pBlackAnimals[i]->setUserData((void*)i);
        m_pBN->addChild(m_pBlackAnimals[i], BLACKANIMALS_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pBlackAnimals[all]->setPosition(m_blackAnimalsPos[Index]);
        m_originalPoints[all] = m_blackAnimalsPos[Index];
        all++;
    } /*while*/
    
    //动物
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(yyBuffer, "SL-GK07-1-TM%02d-D.png",i+1);
        m_pAnimals[i] = Sprite::createWithSpriteFrameName(yyBuffer);
        m_pAnimals[i]->setPosition(m_animalsPos[i]);
        m_pBN->addChild(m_pAnimals[i], ANIMALS_ZORDER);
    }
    
    //青色背景框
    for (unsigned int i = 0; i < 3; i++) {
        m_pCyanFrames[i] = Sprite::createWithSpriteFrameName(CYAN_FRAMES_PNG_NAME);
        m_pCyanFrames[i]->setPosition(m_cyanFramesPos[i]);
        m_pBN->addChild(m_pCyanFrames[i], CYAN_FRAMES_ZORDER);
    }
    
    //问号
    for (unsigned int i = 0; i < 3; i++) {
        m_pMarks[i] = Sprite::createWithSpriteFrameName(MARKS_PNG_NAME);
        m_pMarks[i]->setPosition(m_marksPos[i]);
        m_pBN->addChild(m_pMarks[i], MARKS_ZORDER);
        m_pMarks[i]->setScale(0.9f);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
}

//开始回合
void Round007_01::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_01::scaleChangeSchedule), 0.4, -1, 0);
}

//
void Round007_01::scaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_01::scaleChangeSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pMarks[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.2f, 1.0f),ScaleTo::create(0.2f, 0.9f), NULL));
}

//庆祝
void Round007_01::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

//答对后下移
void Round007_01::areasDown()
{
    int offSetTime = 1.0f;
    for (unsigned int i = 0; i < 3; i++) {
        m_pCyanFrames[i]->runAction(MoveBy::create(INIT_DOWN_TIME+offSetTime*i, Vec2(0, INIT_DOWN_OFFSETY)));
        m_pAnimalActors[i]->runAction(MoveBy::create(INIT_DOWN_TIME+offSetTime*i, Vec2(0, INIT_DOWN_OFFSETY)));
        if (i == 0) {
            m_pBlackAnimals[2]->runAction(MoveBy::create(INIT_DOWN_TIME+offSetTime*i, Vec2(0, INIT_DOWN_OFFSETY)));
        }else if (i == 1)
        {
            m_pBlackAnimals[0]->runAction(MoveBy::create(INIT_DOWN_TIME+offSetTime*i, Vec2(0, INIT_DOWN_OFFSETY)));
        }else if (i == 2)
        {
            m_pBlackAnimals[1]->runAction(MoveBy::create(INIT_DOWN_TIME+offSetTime*i, Vec2(0, INIT_DOWN_OFFSETY)));
        }
        
    }
}

/* 开启触摸 */
void Round007_01::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f, m_pMarks[0]->getContentSize().width, m_pMarks[0]->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        Sprite* pSp = nullptr;
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号
        if (rect.containsPoint(m_pMarks[0]->convertTouchToNodeSpace(touch))) {
            pSp = m_pMarks[0];
            if (!isCorrect && clicknumber == 2) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 0;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pMarks[1]->getContentSize().width, m_pMarks[1]->getContentSize().height);
        if (rect.containsPoint(m_pMarks[1]->convertTouchToNodeSpace(touch))) {
            pSp = m_pMarks[1];
            if (!isCorrect && clicknumber == 0) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 1;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pMarks[2]->getContentSize().width, m_pMarks[2]->getContentSize().height);
        if (rect.containsPoint(m_pMarks[2]->convertTouchToNodeSpace(touch))) {
            pSp = m_pMarks[2];
            if (!isCorrect && clicknumber == 1) {
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 2;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            Vec2 point = pSp->getPosition();      //获得目标盒子的位置
            pSp->setVisible(false);            //隐藏红色闪烁区
            m_pMarks[curAreaNumber]->setVisible(false);
            target->setPosition(point);       //将动物阴影放置在目标区
            target->setScale(BLACK_ANIMAL_SCALE);           //阴影缩小
            m_pMarks[curAreaNumber]->setVisible(false);
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            //加载相应gaf
            char dwBuffer[64];
            if (curAreaNumber == 0) {
                sprintf(dwBuffer, "res/stage007/SL-GK07-1-TM01-C/SL-GK07-1-TM01-C.gaf");   //猫的gaf
            }else if (curAreaNumber == 1)
            {
                sprintf(dwBuffer, "res/stage007/SL-GK07-1-TM02-C/SL-GK07-1-TM02-C.gaf");   //兔子gaf
            }else if (curAreaNumber == 2)
            {
                sprintf(dwBuffer, "res/stage007/SL-GK07-1-TM03-C/SL-GK07-1-TM03-C.gaf");  //熊gaf
            }
            m_pAnimalActors[curAreaNumber] = Actor::createWithGafFile(dwBuffer);
            m_pAnimalActors[curAreaNumber]->setPosition(m_gafPos[curAreaNumber]);
            addChild(m_pAnimalActors[curAreaNumber],GAF_ZORDER);
            
            //隐藏相应动物图
            m_pAnimals[curAreaNumber]->setVisible(false);
            
            if (m_correctNumber == 3) {   //全部答对
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pBlackAnimals[i]);
                } /*for*/
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
    listenerRight->onTouchCancelled = [this](Touch* touch, Event* event) {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pBlackAnimals[0]);   //注册绑定监听动物黑色阴影
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pBlackAnimals[i]);
    }
}

/*  回合结束回调 */
void Round007_01::endRound()
{
    areasDown();         //目标区域下移退出回合
    
    scheduleOnce([this](float){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 3.0f, "delaytime");
}

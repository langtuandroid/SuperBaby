//
//  Round007_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/3.
//
//

#include "Round007_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_GAF_BG_FILE          "res/common/guanghuanxiaoguo/guanghuanxiaoguo.gaf"
#define CELEBRATE_GAF_FILE             "res/common/wujiaoxing01/wujiaoxing01.gaf"
#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage007/SL-GK07-IT.png"             //BN的png文件
#define BN_ZORDER                      1                                         //BN的z值
#define ANSWER_MARK_ZORDER             4                                         //答案符号z值
#define MARK_ZORDER                    3                                         //目标符号z值
#define FRUIT_ZORDER                   2                                         //水果z值
#define VINE_ZORDER                    1                                         //藤条z值
#define BEGAN_UP_TIME                  0.5f                                    // 上升动画时间
#define BEGAN_UP_OFFSETY               569.0f                                  // 上升动画偏移值
#define BEGAN_DOWN_TIME                0.5f                                    // 下降动画时间
#define BEGAN_DOWN_OFFSETY             -1200.0f                                  // 下降动画偏移值
#define END_UP_TIME                    1.0f                                    // 上升动画时间
#define END_UP_OFFSETY                 1200.0f                                  // 上升动画偏移值
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间
#define BACK_SCENE_TIME               2.5f                              //返回关卡选择场景延时时间
#define MOVE_ACTOR_TIME               3.0f                              //分享界面ok过后主角移动时间
#define TREE_PNG_FILE                 "res/stage007/SL-GK07-BG01-qj2.png"    //树png路径
#define TREE_POS                      Vec2(12615.0f,768.0f)                     //树的坐标
#define TREE_TAG                      1000                                   //树的tag值
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round007_04::Round007_04()
:m_correctNumber(0),
m_upIndex(0),
m_downIndex(0)
{}

/*
 *  destructor
 */
Round007_04::~Round007_04() {
    cleanUp();
}

/*
 *  清理
 */

void Round007_04::cleanUp()
{
    m_pStageScene->removeOBLeaderNode(TREE_TAG);
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    Director::getInstance()->getTextureCache()->removeTextureForKey(TREE_PNG_FILE);
}

//
Round007_04* Round007_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round007_04();
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
bool Round007_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //设置答案符号坐标
    m_answerMarksPos[0] = Vec2(760, -300);
    m_answerMarksPos[1] = Vec2(988, -300);
    m_answerMarksPos[2] = Vec2(1218, -300);
    m_answerMarksPos[3] = Vec2(1445, -300);
    
    //设置目标答案符号坐标
    m_marksPos[0] = Vec2(812, 1821);
    m_marksPos[1] = Vec2(1114, 1821);
    m_marksPos[2] = Vec2(1401, 1821);
    m_marksPos[3] = Vec2(1687, 1801);
    
    //设置藤的坐标
    m_vinesPos[0] = Vec2(812, 2385);
    m_vinesPos[1] = Vec2(1114, 2385);
    m_vinesPos[2] = Vec2(1401, 2385);
    m_vinesPos[3] = Vec2(1687, 2385);
    
    //设置水果坐标
    m_fruitsPos[0] = Vec2(812, 1841);
    m_fruitsPos[1] = Vec2(1114, 1841);
    m_fruitsPos[2] = Vec2(1401, 1841);
    m_fruitsPos[3] = Vec2(1687, 1841);
    
    appendAsyncLoadTexture(TREE_PNG_FILE);
    
    preLoadRes();
    return true;
}

void Round007_04::onPreLoadedRes()
{
    //////
    RoundBase::onPreLoadedRes();
    
    //加载BN
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    // 加载答案符号
    char fhBuffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(fhBuffer, "SL-GK07-4-DA%02ld.png",i+1);
        m_pAnswerMarks[i] = Sprite::createWithSpriteFrameName(fhBuffer);
        m_pAnswerMarks[i]->setUserData((void*)i);
        m_pBN->addChild(m_pAnswerMarks[i], ANSWER_MARK_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int Index = static_cast<unsigned int>(rand() % 4);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pAnswerMarks[all]->setPosition(m_answerMarksPos[Index]);
        m_originalPoints[all] = Vec2(m_answerMarksPos[Index].x, m_answerMarksPos[Index].y+569);
        all++;
    } /*while*/
    
    // 加载目标符号
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(fhBuffer, "SL-GK07-4-DA%02d-1.png",i+1);
        m_pMarks[i] = Sprite::createWithSpriteFrameName(fhBuffer);
        m_pMarks[i]->setPosition(m_marksPos[i]);
        m_pBN->addChild(m_pMarks[i], MARK_ZORDER);
    }
    // 加载藤
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(fhBuffer, "SL-GK07-4-TM06.png");
        m_pVine[i] = Sprite::createWithSpriteFrameName(fhBuffer);
        m_pVine[i]->setPosition(m_vinesPos[i]);
        m_pBN->addChild(m_pVine[i], VINE_ZORDER);
    }
    // 加载水果
    for (unsigned int i = 0; i < 4; i++) {
        sprintf(fhBuffer, "SL-GK07-4-TM%02d.png",i+1);
        m_pFruits[i] = Sprite::createWithSpriteFrameName(fhBuffer);
        m_pFruits[i]->setPosition(m_fruitsPos[i]);
        m_pBN->addChild(m_pFruits[i], FRUIT_ZORDER);
    }
    
    //加载树
    m_pTree = Sprite::create(TREE_PNG_FILE);
    m_pStageScene->addOverLeaderNode(m_pTree,TREE_POS,TREE_TAG);
    
}

//开始回合
void Round007_04::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });

    //答案符号上升
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_04::BeganMarksUpSchedule), BEGAN_UP_TIME - 0.3f, -1, 0.0f);
    //藤条下降
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_04::BeganVinesDownSchedule), BEGAN_DOWN_TIME - 0.3f, -1, 0.0f);
}

// 回合开场动画，符号上升动画调度
void Round007_04::BeganMarksUpSchedule(float dt)
{
    if (m_upIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_04::BeganMarksUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(BEGAN_UP_TIME, Vec2(0.0f, BEGAN_UP_OFFSETY));
    m_pAnswerMarks[m_upIndex++]->runAction(EaseBackOut::create(move));
}

// 回合开场动画，藤条下降动画调度
void Round007_04::BeganVinesDownSchedule(float dt)
{
    if (m_downIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_04::BeganVinesDownSchedule));
        return;
    }
    MoveBy* moveMark = MoveBy::create(BEGAN_DOWN_TIME, Vec2(0.0f, BEGAN_DOWN_OFFSETY));
    m_pMarks[m_downIndex]->runAction(EaseBackOut::create(moveMark));
    
    MoveBy* moveVine = MoveBy::create(BEGAN_DOWN_TIME, Vec2(0.0f, BEGAN_DOWN_OFFSETY));
    m_pVine[m_downIndex]->runAction(EaseBackOut::create(moveVine));
    
    MoveBy* moveFruit = MoveBy::create(BEGAN_DOWN_TIME, Vec2(0.0f, BEGAN_DOWN_OFFSETY));
    m_pFruits[m_downIndex]->runAction(EaseBackOut::create(moveFruit));
    
    m_downIndex++;
}

// 庆祝
void Round007_04::celebrate(const float celebrateDelay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
    
    m_upIndex = 0;                      //重置为0
    //藤条上升
    this->schedule(CC_SCHEDULE_SELECTOR(Round007_04::EndVineUpSchedule), END_UP_TIME - 0.3f, -1, 0.0f);
}

//回合结束分享调用
void Round007_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }else
    {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    moveActor();                        //移动主角
    
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round007_04::fadeOutLayer), MOVE_ACTOR_TIME);
}

// 回合结束动画，符号上升动画调度
void Round007_04::EndVineUpSchedule(float dt)
{
    if (m_upIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round007_04::BeganVinesDownSchedule));
        return;
    }
    MoveBy* move = MoveBy::create(END_UP_TIME, Vec2(0.0f, END_UP_OFFSETY));
    if (m_upIndex == 0) {
        m_pAnswerMarks[3]->runAction(EaseBackOut::create(move));
    }else if (m_upIndex == 1)
    {
        m_pAnswerMarks[2]->runAction(EaseBackOut::create(move));
    }else if (m_upIndex == 2)
    {
        m_pAnswerMarks[0]->runAction(EaseBackOut::create(move));
    }else if (m_upIndex == 3)
    {
        m_pAnswerMarks[1]->runAction(EaseBackOut::create(move));
    }
    
    MoveBy* moveVine = MoveBy::create(END_UP_TIME, Vec2(0.0f, END_UP_OFFSETY));
    m_pVine[m_upIndex]->runAction(EaseBackOut::create(moveVine));
    
    MoveBy* moveFruit = MoveBy::create(END_UP_TIME, Vec2(0.0f, END_UP_OFFSETY));
    m_pFruits[m_upIndex]->runAction(EaseBackOut::create(moveFruit));
    
    m_upIndex++;
}

//移动主角退出回合
void Round007_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));
}

/* 开启触摸 */
void Round007_04::openTouch()
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
            target->setScale(1.1f);
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
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
        Rect rect = Rect(0.0f, 0.0f, m_pFruits[0]->getContentSize().width, m_pFruits[0]->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        Sprite* pSp = nullptr;
        bool isCorrect = false;
        int curAreaNumber = -1;   //当前区域号
        if (rect.containsPoint(m_pFruits[0]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect && clicknumber == 3) {
                pSp = m_pMarks[0];
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 0;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pFruits[1]->getContentSize().width, m_pFruits[1]->getContentSize().height);
        if (rect.containsPoint(m_pFruits[1]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect  && clicknumber == 2) {
                pSp = m_pMarks[1];
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 1;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pFruits[2]->getContentSize().width, m_pFruits[2]->getContentSize().height);
        if (rect.containsPoint(m_pFruits[2]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect  && clicknumber == 0) {
                pSp = m_pMarks[2];
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 2;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = Rect(0.0f, 0.0f, m_pFruits[3]->getContentSize().width, m_pFruits[3]->getContentSize().height);
        if (rect.containsPoint(m_pFruits[3]->convertTouchToNodeSpace(touch))) {
            if (!isCorrect  && clicknumber == 1) {
                pSp = m_pMarks[3];
                isCorrect = true;
                m_correctNumber++;
                curAreaNumber = 3;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        
        if (isCorrect) {
            Vec2 point = pSp->getPosition();
            target->setPosition(point);                     //将拖动符号坐标设置为目标符号坐标
            m_pMarks[curAreaNumber]->setVisible(false);    //隐藏目标符号
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            
            if (m_correctNumber == 4) {
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
            
            if (m_anwerErrorCB) {    //答错
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
        target->setScale(1.0f);
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pAnswerMarks[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pAnswerMarks[i]);
    }
}

/*  回合结束回调 */
void Round007_04::endRound() {}

//渐变变黑退出
void Round007_04::fadeOutLayer(float)
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
    // 过关
    m_pStageScene->stageClear();
}

//分享回调
void Round007_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round007_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round007_04::roundEndShareCallBack, this,true));
}

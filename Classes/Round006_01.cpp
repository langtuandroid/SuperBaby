//
//  Round006_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/2.
//
//

#include "Round006_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define GOODS_PLIST_FILE               "res/stage006/SL-GK06-IT.plist"           //物品纹理plist文件
#define GOODS_PNG_FILE                 "res/stage006/SL-GK06-IT.png"             //物品纹理png图
#define GOODS_BN_ZORDER                2                                         //物品批节点z值
#define CLOUD_GAF_CONFIG_FILE           "config/stage006/cloud.json"              //背景云gaf配置文件
// 云 gaf 文件
#define CLOUD_GAF_FILE                  "res/stage006/SL-GK06-1-IT-D+C/SL-GK06-1-IT-D+C.gaf"
#define CLOUD_GAF_POS                  Vec2(120,1700)                            //背景云位置
#define CLOUD_GAF_ZORDER               1                                         //背景云z值
#define LEFT_BOX_BACK_PNG_FILE         "SL-GK06-1-TM02-BACK.png"                 //左边拼接盒子后面png
#define LEFT_BOX_PNG_FILE              "SL-GK06-1-TM02.png"                      //左边拼接盒子前面png
#define RIGHT_BOX_BACK_PNG_FILE        "SL-GK06-1-TM01-BACK.png"                 //右边拼接盒子后面png
#define RIGHT_BOX_PNG_FILE             "SL-GK06-1-TM01.png"                      //右边拼接盒子前面png
#define BOX_BACK_ZORDER                3                                         //盒子后面部分z值
#define BOX_FRONT_ZORDER               1                                         //盒子前面部分z值
#define LEFT_BOX_POS                    Vec2(970.0f,370.0f)                       //左边盒子位置
#define RIGHT_BOX_POS                   Vec2(1289.0f,370.0f)                       //右边盒子位置
#define GOODS_ZORDER                   2                                        //物品z值
#define FadeIn_TIME                    1                                         //盒子淡入时间
#define FadeOut_TIME                   1                                         //盒子淡出时间
#define LEFT_BOX_DELAY_TIME            0                                         //左边盒子淡入延迟
#define RIGHT_BOX_DELAY_TIME           1                                         //右边盒子淡入延迟
#define INIT_DOWN_OFFSETY              -1120.0f                                // 下坠动画偏移值
#define INIT_DOWN_TIME                  0.8f                                    // 下坠动画时间
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间
#define FOODS_ZORDER                   4                                        //食物被正确放置之前的z值

/*
 *  constructor
 */
Round006_01::Round006_01() {}

/*
 *  destructor
 */
Round006_01::~Round006_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round006_01* Round006_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round006_01();
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
bool Round006_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //设置物品位置
    m_goodsPos[0] = Vec2(840,2060);
    m_goodsPos[1] = Vec2(1097,2060);
    m_goodsPos[2] = Vec2(1396,2060);
    
    appendAsyncLoadTexture(GOODS_PNG_FILE);
    appendAsyncLoadGaf(CLOUD_GAF_FILE);
    preLoadRes();
    return true;
}

void Round006_01::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    //物品纹理加载
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(GOODS_PLIST_FILE);
    m_pGoodsBN = SpriteBatchNode::create(GOODS_PNG_FILE);
    addChild(m_pGoodsBN, GOODS_BN_ZORDER);
    
    //左边盒子后面
    m_pLeftBoxsBack = Sprite::createWithSpriteFrameName(LEFT_BOX_BACK_PNG_FILE);
    m_pLeftBoxsBack->setPosition(LEFT_BOX_POS);
    m_pGoodsBN->addChild(m_pLeftBoxsBack,BOX_BACK_ZORDER);
    m_pLeftBoxsBack->setOpacity(0);
    
    //左边盒子前面
    m_pLeftBoxs = Sprite::createWithSpriteFrameName(LEFT_BOX_PNG_FILE);
    m_pLeftBoxs->setPosition(LEFT_BOX_POS);
    m_pGoodsBN->addChild(m_pLeftBoxs,BOX_FRONT_ZORDER);
    m_pLeftBoxs->setOpacity(0);
    
    //右边盒子后面
    m_pRightBoxsBack = Sprite::createWithSpriteFrameName(RIGHT_BOX_BACK_PNG_FILE);
    m_pRightBoxsBack->setPosition(RIGHT_BOX_POS);
    m_pGoodsBN->addChild(m_pRightBoxsBack,BOX_BACK_ZORDER);
    m_pRightBoxsBack->setOpacity(0);

    //右边盒子前面
    m_pRightBoxs = Sprite::createWithSpriteFrameName(RIGHT_BOX_PNG_FILE);
    m_pRightBoxs->setPosition(RIGHT_BOX_POS);
    m_pGoodsBN->addChild(m_pRightBoxs,BOX_FRONT_ZORDER);
    m_pRightBoxs->setOpacity(0);
    
    //物品
    char wpBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(wpBuffer,"SL-GK06-1-DA%02ld.png",i+1);
        m_pGoods[i] = Sprite::createWithSpriteFrameName(wpBuffer);
        m_pGoodsBN->addChild(m_pGoods[i], FOODS_ZORDER);
        m_pGoods[i]->setUserData((void*)i);
        
        m_pEndGoods[i] = Sprite::createWithSpriteFrameName(wpBuffer);
        m_pGoodsBN->addChild(m_pEndGoods[i],GOODS_ZORDER);
        m_pEndGoods[i]->setVisible(false);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pGoods[all]->setPosition(m_goodsPos[Index]);
        m_originalPoints[all] = Vec2(m_goodsPos[Index].x, m_goodsPos[Index].y-1120);
        all++;
    } /*while*/
    
    // 背景云gaf
    m_pCloudActor = Actor::createWithGafConfigFile(CLOUD_GAF_CONFIG_FILE);
    m_pCloudActor->setAutoReleaseAsset(true);
    m_pCloudActor->setPosition(CLOUD_GAF_POS);
    m_pCloudActor->playWithSequenceAliasName(STANDBY, true);
    addChild(m_pCloudActor,CLOUD_GAF_ZORDER);
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(GOODS_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(GOODS_PNG_FILE);
    }
}

//开始回合
void Round006_01::onEnterRound()
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
    fadeInBoxs();        //盒子淡入
    // 开始物品下落动画
    this->schedule(CC_SCHEDULE_SELECTOR(Round006_01::goodsDonwSchedule), INIT_DOWN_TIME - 0.5f, -1, 0.0f);
}

//盒子渐变出现
void Round006_01::fadeInBoxs()
{
    //左边盒子淡入
    m_pLeftBoxs->runAction(Sequence::create(DelayTime::create(LEFT_BOX_DELAY_TIME),FadeIn::create(FadeIn_TIME), NULL));
    m_pLeftBoxsBack->runAction(Sequence::create(DelayTime::create(LEFT_BOX_DELAY_TIME),FadeIn::create(FadeIn_TIME), NULL));
    //右边盒子淡入
    m_pRightBoxs->runAction(Sequence::create(DelayTime::create(RIGHT_BOX_DELAY_TIME),FadeIn::create(FadeOut_TIME), NULL));
    m_pRightBoxsBack->runAction(Sequence::create(DelayTime::create(RIGHT_BOX_DELAY_TIME),FadeIn::create(FadeOut_TIME), NULL));
    
}

//物品掉落
void Round006_01::goodsDonwSchedule(float dt)
{
    if (m_curGoodsIndex >= 3) {
        this->unschedule(CC_SCHEDULE_SELECTOR(Round006_01::goodsDonwSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    EaseBackOut* ease = EaseBackOut::create(move);
    m_pGoods[m_curGoodsIndex++]->runAction(ease);
}

/* 开启触摸 */
void Round006_01::openTouch()
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
            m_pGoodsBN->reorderChild(target, FOODS_ZORDER + 1);
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
        m_pGoodsBN->reorderChild(target, FOODS_ZORDER);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        Sprite* pSp = nullptr;
        bool isCorrect = false;
        Rect rect = Rect(0.0f, 0.0f, m_pLeftBoxsBack->getContentSize().width, m_pLeftBoxsBack->getContentSize().height);
        if (rect.containsPoint(m_pLeftBoxsBack->convertTouchToNodeSpace(touch))) {  //判断是否在左边盒子
            pSp = nullptr;
            pSp = m_pLeftBoxsBack;
            if (!isCorrect && clicknumber == 0) {          //判断类型是否是左边盒子所属类型
                isCorrect = true;
                m_correctNumber++;
                auto dispatcher = Director::getInstance()->getEventDispatcher();
                dispatcher->removeEventListenersForTarget(target);
            }
        }
        rect = m_pRightBoxsBack->getBoundingBox();
        if (rect.containsPoint(this->convertTouchToNodeSpace(touch))) {  //判断是否在右边盒子
            pSp = nullptr;
            pSp = m_pRightBoxsBack;
            if (!isCorrect) {           //判断类型是否是右边盒子所属类型
                if (clicknumber == 1 || clicknumber == 2) {
                    isCorrect = true;
                    m_correctNumber++;
                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                    dispatcher->removeEventListenersForTarget(target);
                }
            }
        }
        
        if (isCorrect) {     //处理回答正确情况
            Vec2 point = pSp->getPosition();      //获得目标盒子的位置
            target->setPosition(Vec2(point.x, point.y+75));                 //将物品放置在盒子里
            target->removeFromParent();
            
            m_pEndGoods[clicknumber]->setVisible(true);
            m_pEndGoods[clicknumber]->setPosition(Vec2(point.x, point.y+100));
            
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
            // 播放答对音效
            playAnswerCorrectEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答对时主角说话
            playLeadAnsCorEff();
            if (m_celebrateCB) {            //答对庆祝
                m_celebrateCB(1);
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
        
        if (m_correctNumber == 3) {   //全部答对
            m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
            m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
            fadeOutCloud();
            this->celebrate(CELEBRATE_DELAY);
        }
    };
    listenerRight->onTouchCancelled = [this](Touch* touch, Event* event) {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        m_pGoodsBN->reorderChild(target, FOODS_ZORDER);
        target->setPosition(m_originalPoint);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pGoods[0]);   //注册绑定监听盒子
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pGoods[i]);
    }
}

void Round006_01::fadeOutCloud()
{
    m_pCloudActor->playWithSequenceAliasName(TRIGGER);
}

//庆祝
void Round006_01::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

//盒子渐变消失
void Round006_01::fadeOutBoxs()
{
    m_pLeftBoxs->runAction(FadeOut::create(FadeOut_TIME));
    m_pLeftBoxsBack->runAction(FadeOut::create(FadeOut_TIME));
    m_pRightBoxs->runAction(FadeOut::create(FadeOut_TIME));
    m_pRightBoxsBack->runAction(FadeOut::create(FadeOut_TIME));
    for (unsigned int i = 0; i < 3; i++) {
        m_pEndGoods[i]->runAction(FadeOut::create(FadeOut_TIME));
    } /*for*/
}

/*  回合结束回调 */
void Round006_01::endRound()
{
    fadeOutBoxs();    //盒子渐变消失
    if (m_completeCB) {
        m_completeCB();
    }
}


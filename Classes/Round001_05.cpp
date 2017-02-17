//
//  Round001_05.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "Round001_05.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

using namespace gaf;

#define BIRD_ONE_HAIR_GAF_CONFIG_FILE         "config/stage001/bird_one_hair.json"    // 一根毛的鸟配置文件
#define BIRD_TWO_HAIR_GAF_CONFIG_FILE         "config/stage001/bird_two_hair.json"    // 两根毛的鸟配置文件
#define BIRD_STANDBY                          "standby"                               // 待机动画
#define BIRD_TRIGGER                          "trigger"                               // 触发动画

#define TREE_PNG_FILE                         "res/stage001/SL-GK01-3-IT01.png"       // 树
#define TREE_POS                              Vec2(1120.0f, -1220.0f)                 // 树坐标

// 庆祝延时
#define CELEBRATE_DELAY                       7.0f

// 上升时间
#define UP_TIME                               2.0f

#define SCALEBY_TIME                          0.2f                                 //鸟放大缩小时间

#define SCALEBY_DELAY_TIME                    0.5f                           //鸟放大缩小间距时间

#define BIRD_BLINK_DELAY_TIME                 0.2f                           //每只鸟开始眨眼间隔时间

#define BIRD_FLY_MOV_TIME                     2.0f                           //鸟飞移动时间

#define ACTOR_MOV_TIME                       2.0f                           //主角移动时间

#define PASS_GAF_FILE                   "res/common/yanshi02/yanshi02.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3

/*
 *  constructor
 */
Round001_05::Round001_05()
{
    // 添加鸟的初始位置
    m_birdInitPos[0].x = -60;
    m_birdInitPos[0].y = 1800;
    m_birdInitPos[1].x = 210;
    m_birdInitPos[1].y = 1815;
    m_birdInitPos[2].x = 460;
    m_birdInitPos[2].y = 1840;
    m_birdInitPos[3].x = 660;
    m_birdInitPos[3].y = 2230;
    m_birdInitPos[4].x = 880;
    m_birdInitPos[4].y = 2120;
    
    // 设置五只鸟触摸响应区域
    m_AllRect[0] = Rect(580, 265, 200, 200);
    m_AllRect[1] = Rect(850, 280, 200, 200);
    m_AllRect[2] = Rect(1100, 305, 200, 200);
    m_AllRect[3] = Rect(1300, 705, 200, 200);
    m_AllRect[4] = Rect(1520, 585, 200, 200);
    
    // 设置鸟飞的目的坐标
    m_MovePos[0] = Vec2(-500, 3200);
    m_MovePos[1] = Vec2(-200, 3200);
    m_MovePos[2] = Vec2(800, 3200);
    m_MovePos[3] = Vec2(200, 3200);
    m_MovePos[4] = Vec2(-1200,2800);
}

/*
 *  destructor
 */
Round001_05::~Round001_05()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    GafAssetCache::getInstance()->removeGafAssetForKey(PASS_GAF_FILE);
}

//
Round001_05* Round001_05::createWithRoundIndex(const int roundIndex,
                                               StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round001_05();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene, pos))
    {
        pRound->autorelease();
    } else {
        CC_SAFE_DELETE(pRound);
    }
    return pRound;
}

bool Round001_05::initWithRoundIndex(const int roundIndex,
                                     StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(TREE_PNG_FILE);
    appendRemovedTexture(TREE_PNG_FILE);
    appendAsyncLoadGaf("res/stage001/SL-GK01-3-MS02-D+C/SL-GK01-3-MS02-D+C.gaf");
    appendAsyncLoadGaf("res/stage001/SL-GK01-3-MS01-D+C/SL-GK01-3-MS01-D+C.gaf");
    
    preLoadRes();
    return true;
}

//
void Round001_05::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    m_pBirdTreeNode = Sprite::create(TREE_PNG_FILE);
    this->addChild(m_pBirdTreeNode, 2);
    m_pBirdTreeNode->setPosition(TREE_POS);
    
    /* 加载鸟 Actor */
    for (unsigned int i = 0 ; i < 5; i++)
    {
        if (i == 1) { //第二只鸟不一样
            m_pBirdActors[i] = Actor::createWithGafConfigFile(BIRD_ONE_HAIR_GAF_CONFIG_FILE);
        } else {
            m_pBirdActors[i] = Actor::createWithGafConfigFile(BIRD_TWO_HAIR_GAF_CONFIG_FILE);
        }
        m_pBirdTreeNode->addChild(m_pBirdActors[i], 3);
        birdBlinkAction(); // 鸟不同步眨眼
    }/*for*/
    
    //随机
    unsigned int all = 0;
    bool selected[5] = {false, false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 5)
    {
        unsigned int Index = static_cast<unsigned int>(rand() % 5);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pBirdActors[all]->setPosition(m_birdInitPos[Index]);
        if (all == 1) {
            m_trueAnswerIndex = Index;
        }
        all++;
    } /*while*/
}

void Round001_05::onEnterRound()
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
    upTreeAndBirds(); // 树和鸟上升
}

/*
 * 树和鸟上升
 */
void Round001_05::upTreeAndBirds()
{
    m_pBirdTreeNode->runAction(Sequence::create(MoveBy::create(UP_TIME, Vec2(0, 1000)),CallFunc::create([this]{
        if (!UserDefault::getInstance()->getBoolForKey("clickone", false)) {
            playPassActor();        //播放引导
            UserDefault::getInstance()->setBoolForKey("clickone", true);
            UserDefault::getInstance()->flush();
        }
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

void Round001_05::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->addChild(colorLayer,PASS_GAF_ZORDER);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, colorLayer);
    dispatcher->setPriority(listenerDrag, -130);
    Actor* passActor = Actor::createWithGafFile(PASS_GAF_FILE,false);
    passActor->setPosAtNode(Vec2(300, 500));
    passActor->setPosition(Vec2(350, 650));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]
    {
        colorLayer->removeFromParent();
        passActor->setVisible(false);
    });
}

void Round001_05::openTouch()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        for (unsigned int i = 0; i < 5; i++)
        {
            if (m_AllRect[i].containsPoint(touchPoint))
            {
                return true;
            }
        } /*for*/
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        for (unsigned int i = 0; i < 5; i++)
        {
            if (m_AllRect[i].containsPoint(touchPoint))
            {
                if (m_trueAnswerIndex == i) { // 答题正确
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
                    
                    this->celebrate(CELEBRATE_DELAY);
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
                } else {
                    // 播放答错音效
                    playAnswerErrorEffect();
                    if (getEffIndex() != -1) {
                        MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    }
                    //答错时主角说话
                    playLeadAnsErrEff();
                    if (m_anwerErrorCB) {
                        m_anwerErrorCB();
                    }
                }
                break;
            }
        } /*for*/
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, this);
}

/*
 *  鸟不同步眨眼
 */
void Round001_05::birdBlinkAction()
{
    if (m_CurBirdIndex < 5)
    {
        auto callFunc = CallFunc::create([this]
        {
            m_pBirdActors[m_DelayTimeBirdIndex]->playWithSequenceAliasName(BIRD_TRIGGER,true);
            m_DelayTimeBirdIndex++;
        });
        m_pBirdActors[m_CurBirdIndex]->runAction(Sequence::create(DelayTime::create(BIRD_BLINK_DELAY_TIME*m_CurBirdIndex), callFunc, NULL));
    }
    m_CurBirdIndex++;
}

/*
 *  选择正确后鸟的动作改变
 */
void Round001_05::chooseTrueActionBird()
{
    if (m_CurBirdIndex < 5)
    {
        auto callFunc = CallFunc::create([this]{
            m_pBirdActors[m_DelayTimeBirdIndex]->playWithSequenceAliasName(BIRD_STANDBY,true);
            birdFlyAction(m_DelayTimeBirdIndex);
            m_DelayTimeBirdIndex++;
        });
        m_pBirdActors[m_CurBirdIndex]->runAction(Sequence::create(DelayTime::create(BIRD_BLINK_DELAY_TIME*m_CurBirdIndex), callFunc, NULL));
    }
    m_CurBirdIndex++;
}

/*
 *  鸟的动作改变后飞走
 */
void Round001_05::birdFlyAction(int birdIndex)
{
    m_pBirdActors[birdIndex]->runAction(MoveTo::create(BIRD_FLY_MOV_TIME, m_MovePos[birdIndex]));
}

void Round001_05::celebrate(const float celebrateDelay)
{
    m_CurBirdIndex = 0; //当前鸟index重置为0，指向第一个
    m_DelayTimeBirdIndex = 0; //当前鸟index重置为0，指向第一个（用于改变鸟的动作）
    for (unsigned int i = 0; i < 5; i++) {
        chooseTrueActionBird(); //选择正确后鸟的动作
    } /*for*/
    
    if (m_celebrateCB)
    {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->setCloseTouch();    //提前关闭ui层的触摸
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

void Round001_05::endRound()
{
    m_pStageScene->moveLeadingActorCB(ACTOR_MOV_TIME, Vec2(600.0f, 0.0f), [this]
    {
        if (m_completeCB)
        {
            m_completeCB();
        }
    });
}

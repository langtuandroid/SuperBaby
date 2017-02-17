//
//  Round010_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/8.
//
//

#include "Round010_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage010/SL-GK10-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define UP_BACKGROUND_PNG             "res/stage010/SL-GK010-QJ_002.png"      //上部背景图
#define UP_BACKGROUND_TAG             1000                                     //上部背景tag值
#define UP_BACKGROUND_POS             Vec2(4500.0f,1300.0f)                     //上部背景的坐标
#define INIT_UP_TIME                  0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY               300.0f                                  // 上升动画偏移值
#define INIT_DOWN_TIME                0.5f                                    // 下降动画时间
#define INIT_DOWN_OFFSETY             -300.0f                                  // 下降动画偏移值
#define NUMBER_ZORDER                 1                                       //数字Z值
#define ROPE_ZORDER                   2                                       //绳子z值
#define ROPE_PASS_ZORDER              3                                       //过关状态绳子z值
#define ROPE_POS                      Vec2(1124,2068)                          //绳子的坐标
#define ROPE_PASS_POS                 Vec2(1124,968)                          //绳子的坐标
#define ROPE_PNG_NAME                 "SL-GK010-2-TM01.png"                   //绳子png名
#define ROPE_PASS_PNG_NAME            "SL-GK010-2-TM02.png"                   //过关状态的绳子png名
#define ROPE_MOVE_TIME                1                                      //绳子淡入时间
#define ROPE_MOVE_DOWN_OFFSETY        -1100                                   //绳子下降高度
#define ROPE_MOVE_UP_OFFSETY          1100                                   //绳子上升高度
#define SCALE_CHANGE_TIME             0.5f                                   //绳子改变尺寸时间
#define FadeIn_TIME                   1                                      //绳子淡入时间
#define FadeOut_TIME                  1                                      //绳子淡出时间

/*
 *  constructor
 */
Round010_02::Round010_02()
:m_InitAniIndex(0)
{}

/*
 *  destructor
 */
Round010_02::~Round010_02() {
    cleanUp();
}

/*
 *  清理
 */
void Round010_02::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
}

//
Round010_02* Round010_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round010_02();
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
bool Round010_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(UP_BACKGROUND_PNG);
    preLoadRes();
    
    return true;
}

void Round010_02::onPreLoadedRes()
{
    RoundBase::onPreLoadedRes();
    
    //加载上部背景
    m_pUpBackground = Sprite::create(UP_BACKGROUND_PNG);
    m_pStageScene->addOverLeaderNode(m_pUpBackground,UP_BACKGROUND_POS,UP_BACKGROUND_TAG);
    
    //纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载绳子
    m_pRope = Sprite::createWithSpriteFrameName(ROPE_PNG_NAME);
    m_pRope->setPosition(ROPE_POS);
    m_pBN->addChild(m_pRope, ROPE_ZORDER);
    
    m_pWhiteRope = Sprite::createWithSpriteFrameName("SL-GK010-2-TM01-2.png");
    m_pWhiteRope->setPosition(Vec2(ROPE_POS.x, ROPE_POS.y-1100));
    m_pBN->addChild(m_pWhiteRope, ROPE_PASS_ZORDER);
    m_pWhiteRope->setOpacity(0);
    
    //加载过关状态绳子
    m_pPassRope = Sprite::createWithSpriteFrameName(ROPE_PASS_PNG_NAME);
    m_pPassRope->setPosition(ROPE_PASS_POS);
    m_pBN->addChild(m_pPassRope, ROPE_PASS_ZORDER);
    m_pPassRope->setOpacity(0);
    
    char nsBuffer[64];
    float nx = 800.0f; float offsetX = 250.0f; float ny = -150.0f;
    //被选择的数字
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 0) {
            sprintf(nsBuffer, "SZ3-%02ld.png",i+2);
        }else if (i == 1)
        {
            sprintf(nsBuffer, "SZ3-%02ld.png",i+2);
        }else
        {
            sprintf(nsBuffer, "SZ3-%02ld.png",i+3);
        }
        m_pNumbers[i] = Sprite::createWithSpriteFrameName(nsBuffer);
        m_pNumbers[i]->setUserData((void*)i);
        addChild(m_pNumbers[i], NUMBER_ZORDER);
    }
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pNumbers[all]->setPosition(Vec2(nx+offsetX*Index, ny));
        all++;
    } /*while*/
}

//开始回合
void Round010_02::onEnterRound()
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
    //绳子淡入
    ropeInAction();
}

//绳子出现时动作
void Round010_02::ropeInAction()
{
    m_pRope->runAction(Sequence::create(MoveBy::create(ROPE_MOVE_TIME, Vec2(0.0f, ROPE_MOVE_DOWN_OFFSETY)),CallFunc::create([this]{
        m_pWhiteRope->runAction(Sequence::create(FadeIn::create(0.5f),CallFunc::create([this]{
            m_pRope->setVisible(false);
        }),FadeOut::create(0.5f),FadeIn::create(0.5f),FadeOut::create(0.5f),CallFunc::create([this]{
            m_pRope->setVisible(true);
        }), NULL));
    }),CallFunc::create([this]{
        this->schedule(CC_SCHEDULE_SELECTOR(Round010_02::upNumberSchedule), INIT_UP_TIME - 0.3f, -1, 2.0f);  //上升数字
    }), NULL));
}

//回合开始，依次上升数字
void Round010_02::upNumberSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round010_02::upNumberSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pNumbers[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//答对时绳子状态改变
void Round010_02::changeRope()
{
    m_pRope->runAction(FadeOut::create(FadeOut_TIME));
    m_pPassRope->runAction(Sequence::create(FadeIn::create(FadeIn_TIME), NULL));

}

//庆祝
void Round010_02::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

//离开回合，绳子消失
void Round010_02::ropeOutAction()
{
    m_pPassRope->runAction(MoveBy::create(ROPE_MOVE_TIME, Vec2(0.0f, ROPE_MOVE_UP_OFFSETY)));
}

//回合结束，依次下降数字
void Round010_02::downNumberSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round010_02::downNumberSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    m_pNumbers[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

/* 开启触摸 */
void Round010_02::openTouch()
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
            target->setScale(1.1f);
            return true;
        }
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (clicknumber == 1) { //答对问题
                changeRope();                   //改变绳子状态
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 3; i++) {
                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                    dispatcher->removeEventListenersForTarget(m_pNumbers[i]);
                }
            } else /* 答错*/  {
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
        }
        
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pNumbers[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pNumbers[i]);
    } /*for*/
}

/*  回合结束回调 */
void Round010_02::endRound()
{
    ropeOutAction();                     //上升绳子
    m_InitAniIndex = 0;                 //重置为0，第一个数字
    this->schedule(CC_SCHEDULE_SELECTOR(Round010_02::downNumberSchedule), INIT_DOWN_TIME - 0.3f, -1, 0.0f);
}


//
//  Round005_06.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/27.
//
//

#include "Round005_06.h"
#include "SocialShareNode.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"

#define CELEBRATE_DELAY               3.0f                                    // 庆祝延时
#define MOVE_ACTOR_TIME               3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME               2.5f                              //返回关卡选择场景延时时间
#define STANDBY                       "standby"                               // 待机动画
#define TRIGGER                       "trigger"                               // 触发动画
#define FadeIn_TIME                      1                                   //电话淡入时间
#define FadeOut_TIME                     1                                   //电话淡出时间
#define NUMBER_ZORDER                    3                                       //数字Z值
#define INIT_UP_TIME                    0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY                 300.0f                                  // 上升动画偏移值
#define INIT_DOWN_TIME                   0.5f                                    // 下降动画时间
#define INIT_DOWN_OFFSETY               -300.0f                                  // 下降动画偏移值
#define TOYBLOCK_BN_ZORDER              2                                     //积木BN的Z值
#define TOYBLOCK_POS                    Vec2(1000.0f,580.0f)                  //积木位置
#define TOYBLOCK_NAME_PNG               "SL-GK05-4-TM01-1.png"               //积木在plist里的名字
#define TOYBLOCK_ZORDER                 2                                       //积木节点Z值
#define MONKEY_GAF_CONFIG_FILE        "config/stage005/monkey.json"             //猴子配置文件
#define MONKEY_GAF_POS                Vec2(1050.0f,695.0f)                   //猴子位置
#define MONKEY_ZORDER                 1                                     //猴子z值
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round005_06::Round005_06()
:m_InitAniIndex(0)
{}

/*
 *  destructor
 */
Round005_06::~Round005_06() {
    cleanUp();
}

/*
 *  清理
 */

void Round005_06::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage005/SL-GK05-4-MS01-D+C/SL-GK05-4-MS01-D+C.gaf");
}

//
Round005_06* Round005_06::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round005_06();
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
bool Round005_06::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //猴子
    m_pMonkey = Actor::createWithGafConfigFile(MONKEY_GAF_CONFIG_FILE);
    m_pMonkey->playWithSequenceAliasName(STANDBY,true);
    m_pMonkey->setPosition(MONKEY_GAF_POS);
    addChild(m_pMonkey, MONKEY_ZORDER);
    
    m_PToyblock = Sprite::createWithSpriteFrameName(TOYBLOCK_NAME_PNG);
    m_PToyblock->setPosition(TOYBLOCK_POS);
    this->addChild(m_PToyblock, TOYBLOCK_ZORDER);
    m_PToyblock->setOpacity(0);
    
    char nsBuffer[64];
    float nx = 800.0f; float offsetX = 250.0f; float ny = -150.0f;
    //被选择的数字
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(nsBuffer, "SZ3-%02ld.png",i+3);
        m_pChooseNumber[i] = Sprite::createWithSpriteFrameName(nsBuffer);
        m_pChooseNumber[i]->setUserData((void*)(i+3));
        addChild(m_pChooseNumber[i], NUMBER_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pChooseNumber[all]->setPosition(Vec2(nx+offsetX*Index, ny));
        all++;
    } /*while*/
    return true;
}

//开始回合
void Round005_06::onEnterRound()
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
    //积木渐变出现
    fadeInToyblock();
    
    this->schedule(CC_SCHEDULE_SELECTOR(Round005_06::numberUpSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
}

//积木渐变出现
void Round005_06::fadeInToyblock()
{
    m_PToyblock->runAction(FadeIn::create(FadeIn_TIME));
}

/* 开启触摸 */
void Round005_06::openTouch()
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
            return true;
        }
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        if (rect.containsPoint(touchPoint)) {
            if (clicknumber == 5) { //答对问题
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pChooseNumber[i]);
                } /*for*/
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
        }
        
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pChooseNumber[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pChooseNumber[i]);
    }
}

/*
 *  回合开场动画，小数字上升动画调度
 */
void Round005_06::numberUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round005_06::numberUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pChooseNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//庆祝
void Round005_06::celebrate(const float celebrateDelay) {
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    changeMonkey();               //改变猴子动作
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

//改变猴子动作
void Round005_06::changeMonkey()
{
    m_pMonkey->playWithSequenceAliasName(TRIGGER,true);
}

//回合结束分享调用
void Round005_06::roundEndShareCallBack(bool isShare)
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
    fadeOutToyblock();                  //积木渐变消失
    moveActor();                        //移动主角
    
    m_InitAniIndex = 0;                 //重置为0，第一个数字
    this->schedule(CC_SCHEDULE_SELECTOR(Round005_06::numberDownSchedule), INIT_UP_TIME - 0.3f, -1, 0.0f);
    
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round005_06::fadeOutLayer), BACK_SCENE_TIME-1.5f);
}

//离场数字下降
void Round005_06::numberDownSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round005_06::numberDownSchedule));
        
        return;
    }
    MoveBy* move = MoveBy::create(INIT_DOWN_TIME, Vec2(0.0f, INIT_DOWN_OFFSETY));
    m_pChooseNumber[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

/*
 * 积木渐变隐藏
 */
void Round005_06::fadeOutToyblock()
{
    m_PToyblock->runAction(FadeOut::create(FadeOut_TIME));
}

/*
 *  分享界面点击ok后移动主角
 */

void Round005_06::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));
}

/*  回合结束回调 */
void Round005_06::endRound() {}

//渐变变黑退出
void Round005_06::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor,COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            // 过关
            m_pStageScene->stageClear();
            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round005_06::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round005_06::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round005_06::roundEndShareCallBack, this,true));
}

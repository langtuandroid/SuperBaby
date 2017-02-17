//
//  Round004_05.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/24.
//
//

#include "Round004_05.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"

#define MONKEY_GAF_CONFIG_FILE           "config/stage004/monkey.json"             //猴子配置文件
#define MONKEY_STANDBY                   "standby"                               // 待机动画
#define MONKEY_TRIGGER                   "trigger"                               // 触发动画
#define MONKEY_ZORDER                    1                                       //猴子Z值
#define MONKEY_POS                       Vec2(600,1600)                          //猴子位置

#define FadeIn_TIME                      1.0f                                       //树桩淡入时间
#define FadeOut_TIME                     0.5f                                       //树桩淡出时间
#define CELEBRATE_DELAY                  3.0f                                    // 庆祝延时
#define TREE_BN_ZORDER                   1                                      //树桩组Z值
#define MOVE_ACTOR_TIME               3.0f                              //分享界面ok过后主角移动时间
#define MOVE_ACTOR_DELAY_TIME         9.0f                              //返回关卡选择场景延时时间
#define BACK_SCENE_TIME               2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300


/*
 *  constructor
 */
Round004_05::Round004_05(){}

/*
 *  destructor
 */
Round004_05::~Round004_05() {
    cleanUp();
}

/*
 *  清理
 */
void Round004_05::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage004/SL-GK04-4-MS01-D+C/SL-GK04-4-MS01-D+C.gaf");
}

//
Round004_05* Round004_05::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round004_05();
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
bool Round004_05::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //加载猴子gaf
    m_pMonkey = Actor::createWithGafConfigFile(MONKEY_GAF_CONFIG_FILE);
    m_pMonkey->setPosition(MONKEY_POS);
    addChild(m_pMonkey, MONKEY_ZORDER);
    m_pMonkey->playWithSequenceAliasName(MONKEY_TRIGGER,true);
    
    char szBuffer[64];
    float szX = 850.0f; float offsetX = 350.0f; float szY = 400.0f;
    //被选择的树桩
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK04-4-DA%02ld.png",i+1);
        m_pTrees[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pTrees[i]->setUserData((void*)(i+1));
        m_pTrees[i]->setOpacity(0);
        this->addChild(m_pTrees[i]);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pTrees[all]->setPosition(Vec2(szX+offsetX*Index, szY));
        all++;
    } /*while*/
    
    return true;
}

/* 开启触摸 */
void Round004_05::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
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
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        if (rect.containsPoint(touchPoint)) {
            if (clicknumber == 3) { //答对问题
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
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
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pTrees[i]);
                } /*for*/
            }else //答错
            {
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
            }
        }
        
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pTrees[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pTrees[i]);
    }
}

//开始回合
void Round004_05::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
    });
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    fadeInTrees(); //渐变显示树桩
}

/*
 * 渐变显示树桩
 */

void Round004_05::fadeInTrees()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pTrees[i]->runAction(FadeIn::create(FadeIn_TIME));
    }
}

//庆祝
void Round004_05::celebrate(const float celebrateDelay) {
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    changeMonkey();      //改变猴子动作
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
    
}

/*
 * 改变猴子动作
 */
void Round004_05::changeMonkey()
{
    m_pMonkey->playWithSequenceAliasName(MONKEY_STANDBY,true);
}

//回合结束分享调用
void Round004_05::roundEndShareCallBack(bool isShare)
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
    fadeOutTrees();                    //渐变隐藏树桩
    moveActor();                      //移动主角
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round004_05::fadeOutLayer), BACK_SCENE_TIME-0.5f);
}

//渐变隐藏树桩
void Round004_05::fadeOutTrees()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pTrees[i]->runAction(FadeOut::create(FadeOut_TIME));
    }
}

/*
 *  分享界面点击ok后移动主角
 */
void Round004_05::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));
}

//渐变变黑退出
void Round004_05::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
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
void Round004_05::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round004_05::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round004_05::roundEndShareCallBack, this,true));
}

/*  回合结束回调 */
void Round004_05::endRound()
{

}

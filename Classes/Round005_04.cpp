//
//  Round005_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/1.
//
//

#include "Round005_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define TENT_GAF_STANDBY_FILE            "res/stage005/SL-GK05-3-BGA01-D/SL-GK05-3-BGA01-D.gaf"      //帐篷待机gaf
#define TENT_GAF_TRIGGER_FILE            "res/stage005/SL-GK05-3-BGA01-C/SL-GK05-3-BGA01-C.gaf"      //帐篷触发gaf
#define FIREMAN_GAF_CONFIG_FILE          "config/stage005/fireman.json"          //消防员配置文件
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define TENT_GAF_STANDBY_POS             Vec2(800.0f,1850.0f)                   //帐篷待机位置
#define TENT_GAF_TRIGGER_POS             Vec2(880.0f,1837.0f)                   //帐篷触发位置
#define TENT_GAF_ZORDER                  1                                       //帐篷Z值
#define FIREMAN_GAF_POS                  Vec2(-200.0f,2000.0f)                    //消防鸟位置
#define FIREMAN_GAF_ZORDER               3                                       //消防鸟z值
#define TELEPHONE_BN_ZORDER              2                                     //电话BN的Z值
#define FadeIn_TIME                      1                                   //电话淡入时间
#define FadeOut_TIME                     1                                   //电话淡出时间

/*
 *  constructor
 */
Round005_04::Round005_04()
{
}

/*
 *  destructor
 */
Round005_04::~Round005_04() {
    cleanUp();
}

/*
 *  清理
 */

void Round005_04::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey("res/stage005/SL-GK05-3-MS01-D+C/SL-GK05-3-MS01-D+C.gaf");
    GafAssetCache::getInstance()->removeGafAssetForKey(TENT_GAF_STANDBY_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(TENT_GAF_TRIGGER_FILE);
}

//
Round005_04* Round005_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round005_04();
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
bool Round005_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //帐篷待机
    m_pTentStandbyActor = Actor::createWithGafFile(TENT_GAF_STANDBY_FILE);
    m_pTentStandbyActor->setPosition(TENT_GAF_STANDBY_POS);
    addChild(m_pTentStandbyActor,TENT_GAF_ZORDER);
    
    float dhX = 800.0f; float offSetX = 450.0f; float dhY = 350.0f;
    char dhBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(dhBuffer, "SL-GK05-3-DA%02ld.png",i+1);
        m_pTelephones[i] = Sprite::createWithSpriteFrameName(dhBuffer);
        m_pTelephones[i]->setUserData((void*)i);
        this->addChild(m_pTelephones[i]);
        m_pTelephones[i]->setOpacity(0);
    }
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pTelephones[all]->setPosition(Vec2(dhX+offSetX*Index, dhY));
        all++;
    } /*while*/
    return true;
}

void Round005_04::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
}

//开始回合
void Round005_04::onEnterRound()
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
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        processNpcCollect();
    });
    dispatcher->addCustomEventListener("endRound", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("endRound");
        endRound();
    });
    fadeInTelephones();         //电话渐变出现
}

/*
 * 电话渐变出现
 */
void Round005_04::fadeInTelephones()
{
    int offSetTime = 2.0f;
    for (unsigned int i = 0; i < 3; i++) {
        if (i == 2) {
            m_pTelephones[i]->runAction(Sequence::create(DelayTime::create(offSetTime*i),FadeIn::create(FadeIn_TIME),CallFunc::create(CC_CALLBACK_0(Round005_04::fadeInTelephonesComplete, this)), NULL));
        }else
        {
            m_pTelephones[i]->runAction(Sequence::create(DelayTime::create(offSetTime*i),FadeIn::create(FadeIn_TIME), NULL));
        }
        
    }
}

/* 开启触摸 */
void Round005_04::openTouch()
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
            if (clicknumber == 1) { //答对问题
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pTelephones[i]);
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
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pTelephones[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pTelephones[i]);
    }
}

/*
 * 电话渐变出现完成
 */
void Round005_04::fadeInTelephonesComplete()
{
    m_jumped = true;
    if (m_opentouched) {
        openTouch();
    }
}

//庆祝
void Round005_04::celebrate(const float celebrateDelay) {
    
    addFiremanActor();      //添加消防员
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
}

//加载消防员
void Round005_04::addFiremanActor()
{
    m_PFiremanActor = Actor::createWithGafConfigFile(FIREMAN_GAF_CONFIG_FILE);
    m_PFiremanActor->setPosition(FIREMAN_GAF_POS);
    addChild(m_PFiremanActor, FIREMAN_GAF_ZORDER);
    m_PFiremanActor->playWithSequenceAliasName(STANDBY,true);
    
    ccBezierConfig points;
    points.controlPoint_1 = Vec2(200, 1700);
    points.controlPoint_2 = Vec2(500, 1250);
    points.endPosition = Vec2(800, 1500);
    
    m_PFiremanActor->runAction(Sequence::create(BezierTo::create(2.0f, points),CallFunc::create([this]{
        m_PFiremanActor->playWithSequenceAliasName(TRIGGER);     //将消防员切换到喷水动作
        m_PFiremanActor->setAnimationFinishedPlayDelegate([this]
        {
            m_PFiremanActor->playWithSequenceAliasName(STANDBY,true);  //消防员动作切换为不喷水状态
            //隐藏清除待机帐篷
            m_pTentStandbyActor->setVisible(false);
            m_pTentStandbyActor->removeFromParentAndCleanup(true);
            m_pTentStandbyActor = nullptr;
            //帐篷触发
            m_pTentTriggerActor = Actor::createWithGafFile(TENT_GAF_TRIGGER_FILE,false);
            m_pTentTriggerActor->setPosition(TENT_GAF_TRIGGER_POS);
            addChild(m_pTentTriggerActor,TENT_GAF_ZORDER);
            
            ccBezierConfig points;
            points.controlPoint_1 = Vec2(800, 1500);
            points.controlPoint_2 = Vec2(1000, 1250);
            points.endPosition = Vec2(1900, 2150);
            
            m_PFiremanActor->runAction(Sequence::create(BezierTo::create(2.0f, points), NULL));

        });
    }), NULL));
}

//电话淡出
void Round005_04::fadeOutTelephone()
{
    int offSetTime = 1.0f;
    for (unsigned int i = 0; i < 3; i++) {
        if (i == 1) {
            m_pTelephones[i]->runAction(Sequence::create(DelayTime::create(offSetTime*(i+1)),FadeOut::create(FadeOut_TIME), NULL));
        }else
        {
            if (i == 2) {
                m_pTelephones[i]->runAction(Sequence::create(DelayTime::create(offSetTime*(i-1)),FadeOut::create(FadeOut_TIME), NULL));
            }else
            {
                m_pTelephones[i]->runAction(Sequence::create(DelayTime::create(offSetTime*i),FadeOut::create(FadeOut_TIME), NULL));
            }
            
        }
    }
}

/*  回合结束回调 */
void Round005_04::endRound()
{
    fadeOutTelephone();
    scheduleOnce([this](float dt){
        if (m_completeCB) {
            m_completeCB();
        }
    }, 3.0f, "end");
}

/*  处理npc收集 */
void Round005_04::processNpcCollect() {}


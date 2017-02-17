//
//  Round010_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/8.
//
//

#include "Round010_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "NpcActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "CustomMessage.h"
#include "AppConfig.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage010/SL-GK10-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage010/SL-GK10-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define INIT_FADE_TIME                 1.0f                                    // 渐变动画时间
#define CAMERA_GAF_CONFIG_FILE         "res/npc/NPC-034-D+C.json"             //照相机配置文件
#define CAMERA_GAF_ZORDER              1                                       //照相机gaf的z值
#define PHOTOS_ZORDER                  1                                       //照片的z值
#define RED_PHOTOS_ZORDER              2                                       //红色模板照片的z值
#define FadeIn_TIME                    0.5                                       //照片淡入时间
#define FadeOut_TIME                   0.5                                       //照片淡出时间
#define PHOTO_SCALE_TIME               0.3f                                    //照片改变尺寸时间
#define LEFT_BACKGROUDN_PNG            "res/stage010/SL-GK010-QJ_001.png"      //左边背景图
#define LEFT_BACKGROUND_TAG            1000                                     //左边背景tag值
#define LEFT_BACKGROUND_POS            Vec2(0.0f,0.0f)                              //左边背景的坐标

#define NPC_POS                     Vec2(940.0f, 162.0f)
#define NPC_KEY                     "NPC-034"
#define NPC_COLLECT_DURATION        1.5f
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round010_01::Round010_01()
{
    //设置照片坐标
    m_photosPos[0] = Vec2(704, 839);
    m_photosPos[1] = Vec2(1085, 839);
    m_photosPos[2] = Vec2(1566, 839);
}

/*
 *  destructor
 */
Round010_01::~Round010_01()
{
    m_pStageScene->removeOBLeaderNode(LEFT_BACKGROUND_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
    Director::getInstance()->getTextureCache()->removeTextureForKey(LEFT_BACKGROUDN_PNG);
}

//
Round010_01* Round010_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round010_01();
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
bool Round010_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //加载左背景
    m_pLeftBackground = Sprite::create(LEFT_BACKGROUDN_PNG);
    m_pLeftBackground->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addOverLeaderNode(m_pLeftBackground,LEFT_BACKGROUND_POS,LEFT_BACKGROUND_TAG);
    
    appendAsyncLoadTexture(BN_PNG_FILE);
    appendAsyncLoadTexture(LEFT_BACKGROUDN_PNG);
    appendAsyncLoadGaf("res/npc/NPC-034-D+C/NPC-034-D+C.gaf");
    preLoadRes();
    return true;
}

//
void Round010_01::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    // 纹理加载
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);

    // 加载gaf
    m_pNpcActor = NpcActor::createWithGafConfigFile(CAMERA_GAF_CONFIG_FILE);
    m_pNpcActor->setPosition(NPC_POS);
    m_pNpcActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pNpcActor, CAMERA_GAF_ZORDER);
    
    // 加载照片
    char zpBuffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(zpBuffer, "SL-GK010-1-DA%02ld.png",i+1);
        m_pPhotos[i] = Sprite::createWithSpriteFrameName(zpBuffer);
        m_pPhotos[i]->setPosition(m_photosPos[i]);
        m_pPhotos[i]->setUserData((void*)i);
        m_pBN->addChild(m_pPhotos[i], PHOTOS_ZORDER);
        m_pPhotos[i]->setOpacity(0);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    
}

//开始回合
void Round010_01::onEnterRound()
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
    dispatcher->addCustomEventListener("collectNpc", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("collectNpc");
        MyAudioEngine::getInstance()->playAudio(NPC_ENTER_BAG_MUSIC_FILE);
        
        processNpcCollect();
        std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
        MyAudioEngine::getInstance()->playAudio(effStr.c_str());
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
    this->schedule(CC_SCHEDULE_SELECTOR(Round010_01::fadeInPhotosUpSchedule), INIT_FADE_TIME, -1, 0.0f);
}

// 回合开场动画，渐变出现照片动画调度
void Round010_01::fadeInPhotosUpSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round010_01::fadeInPhotosUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pPhotos[m_InitAniIndex++]->runAction(FadeIn::create(FadeIn_TIME));
}

//庆祝
void Round010_01::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

// 回合结束动画，渐变隐藏照片动画调度
void Round010_01::fadeOutPhotosUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round010_01::fadeOutPhotosUpSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pPhotos[m_InitAniIndex++]->runAction(FadeOut::create(FadeOut_TIME));
}

/* 开启触摸 */
void Round010_01::openTouch()
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
            target->setScale(0.9f);
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
            if (clicknumber == 2)
            {
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
                this->celebrate(CELEBRATE_DELAY);   //答对庆祝
                for (unsigned int i = 0; i < 3; i++) {   //移除监听
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPhotos[i]);
                } /*for*/
            } else {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {   //答错
                    m_anwerErrorCB();
                }
            }
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pPhotos[0]);
    for (unsigned int i = 1; i < 3; i++)
    {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pPhotos[i]);
    } /*for*/
}

/*  回合结束回调 */
void Round010_01::endRound()
{
    m_InitAniIndex = 0;    //重置为0
    this->schedule(CC_SCHEDULE_SELECTOR(Round010_01::fadeOutPhotosUpSchedule), 0.5f, -1, 0.0f);
}

/*  处理npc收集 */
void Round010_01::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);

    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(400, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pNpcActor->setVisible(false);
    }), NULL);
    m_pNpcActor->runAction(npcActions);
    
    //
    scheduleOnce([this] (float dt)
                 {
                     auto pNpcManager = NpcDataManager::getInstance();
                     auto pNpcData = pNpcManager->getNpcDataByKey(NPC_KEY);
                     if (pNpcData != nullptr ) {
                         pNpcData->collect();
                     }
                     m_pStageScene->getRoundUINode()->setCollectNpcKey(NPC_KEY);
                     m_pStageScene->getRoundUINode()->changeBagGafActor(2);
                 },
                 NPC_COLLECT_DURATION, "npcCollected");
}

//
//  Round012_03.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round012_03.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "CustomMessage.h"
#include "AppConfig.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage012/SL-GK12-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage012/SL-GK12-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                          "standby"                               // 待机动画
#define TRIGGER                          "trigger"                               // 触发动画
#define INIT_UP_TIME                  0.5f                                      // 上升动画时间
#define INIT_UP_OFFSETY               300.0f                                    // 上升动画偏移值
#define TEETERTOTTER_GAF_CONFIG_FILE  "config/stage012/teetertotter.json"       //翘翘板gaf配置文件
#define TEETERTOTTER_GAF_POS          Vec2(180,1500)                            //跷跷板gaf位置
#define TEETERTOTTER_GAF_ZORDER       1                                         //翘翘板z值
#define TOPIC_BOARD_POS               Vec2(1203,446)                            //题目板位置
#define TOPIC_BOARD_ZORDER            1                                         //题目板z值
#define ANSWER_ZORDER                 2                                         //符号z值
#define CHANGE_SCALE_TIME             0.5f                                      //题目板改变大小时间
#define FADE_OUT_TIME                 0.5f                                      //答对后剩余答案渐变消失时间
#define MOV_TIME                      0.2f                                         //答错回原位置移动时间
#define NPC_SP_FILE                   "res/stage012/NPC-066.png"
#define NPC_POS                     Vec2(740.0f, 462.0f)
#define NPC_KEY                     "NPC-066"
#define NPC_COLLECT_DURATION        1.5f
#define NPC_ENTER_BAG_MUSIC_FILE    "res/audio/npccollect/NPCjinbeibao.mp3"

/*
 *  constructor
 */
Round012_03::Round012_03()
:m_InitAniIndex(0)
{
    m_answersPos[0] = Vec2(1094, -100);
    m_answersPos[1] = Vec2(1292, -100);
}

/*
 *  destructor
 */
Round012_03::~Round012_03() {
    cleanUp();
}

/*
 *  清理
 */

void Round012_03::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners("collectNpc");
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);

}

//
Round012_03* Round012_03::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round012_03();
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
bool Round012_03::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage012/SL-GK12-3-TM01-D+C/SL-GK12-3-TM01-D+C.gaf");
    preLoadRes();
    return true;
}

//
void Round012_03::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    // 纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载actor
    m_pTeetertotterActor = Actor::createWithGafConfigFile(TEETERTOTTER_GAF_CONFIG_FILE);
    m_pTeetertotterActor->setPosition(TEETERTOTTER_GAF_POS);
    m_pTeetertotterActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pTeetertotterActor, TEETERTOTTER_GAF_ZORDER);
    
    //加载题目板
    char tmBuffer[64];

    sprintf(tmBuffer, "SL-GK012-3-TM02.png");
    m_pTopicBoard = Sprite::createWithSpriteFrameName(tmBuffer);
    m_pTopicBoard->setPosition(TOPIC_BOARD_POS);
    m_pBN->addChild(m_pTopicBoard, TOPIC_BOARD_ZORDER);
    
    
    for (unsigned int i = 0; i < 2; i++) {
        if (i == 1) {
            sprintf(tmBuffer, "SL-GK012-3-TM01.png");
            m_pAskMarks[i] = Sprite::createWithSpriteFrameName(tmBuffer);
        }else
        {
            sprintf(tmBuffer, "SL-GK012-3-TM03.png");
            m_pAskMarks[i] = Sprite::createWithSpriteFrameName(tmBuffer);
            m_pAskMarks[i]->setVisible(false);
        }
        m_pAskMarks[i]->setPosition(TOPIC_BOARD_POS);
        m_pBN->addChild(m_pAskMarks[i], TOPIC_BOARD_ZORDER);
    }
    
    //加载答案符号
    
    for (unsigned long i = 0; i < 2; i++) {
        sprintf(tmBuffer, "SL-GK012-3-DA%02ld.png",i+1);
        m_pAnswers[i] = Sprite::createWithSpriteFrameName(tmBuffer);
        m_pAnswers[i]->setUserData((void*)i);
        m_pBN->addChild(m_pAnswers[i], ANSWER_ZORDER);
    }
    //随机设置坐标
    unsigned int all = 0;
    bool selected[2] = {false, false};
    srand(unsigned(time(0)));
    while (all < 2) {
        unsigned int answerIndex = static_cast<unsigned int>(rand() % 2);
        if (selected[answerIndex]) continue;
        selected[answerIndex] = true;
        m_pAnswers[all]->setPosition(m_answersPos[answerIndex]);
        m_originalPoints[all] = Vec2(m_answersPos[answerIndex].x, m_answersPos[answerIndex].y+300);
        all++;
    } /*while*/
}

//开始回合
void Round012_03::onEnterRound()
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
        addNpcSp();
        MyAudioEngine::getInstance()->playAudio(NPC_ENTER_BAG_MUSIC_FILE);
        processNpcCollect();
        std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
        MyAudioEngine::getInstance()->playAudio(effStr.c_str());
        
        scheduleOnce([this](float){
            m_pTeetertotterActor->setVisible(false);
            m_pTopicBoard->setVisible(false);
            m_pAnswers[1]->setVisible(false);
            m_pAskMarks[0]->setVisible(false);
            m_pAskMarks[1]->setVisible(false);
        }, 5.0f, "delaytimeforfalse");
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
        dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);

    });
    fadeInActions();         //题目渐变出现
}

// 回合开场动画，题目渐变出现，以及相关动画
void Round012_03::fadeInActions()
{
    m_pTopicBoard->runAction(Sequence::create(ScaleTo::create(CHANGE_SCALE_TIME, 1.1f),ScaleTo::create(CHANGE_SCALE_TIME, 1.0f),CallFunc::create([this]{
        m_pAskMarks[0]->setVisible(true);
        m_pAskMarks[0]->runAction(Sequence::create(ScaleTo::create(CHANGE_SCALE_TIME, 1.1f),ScaleTo::create(CHANGE_SCALE_TIME, 1.0f),CallFunc::create([this]{
            m_pAskMarks[0]->setVisible(false);
            this->schedule(CC_SCHEDULE_SELECTOR(Round012_03::markUpSchedule), INIT_UP_TIME, -1, 0);  //答案上升
        }),NULL));
    }), NULL));
    
}

// 回合开场动画，符号上升动画调度
void Round012_03::markUpSchedule(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_03::markUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pAnswers[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//答对后剩余答案渐隐
void Round012_03::fadeOutAnswer()
{
    m_pAnswers[0]->runAction(Spawn::create(FadeOut::create(FADE_OUT_TIME),ScaleTo::create(FADE_OUT_TIME, 1.1f), NULL));
}

//答对改变翘翘板动作
void Round012_03::changeActor()
{
    m_pTeetertotterActor->playWithSequenceAliasName(TRIGGER);
}

//添加npc图片
void Round012_03::addNpcSp()
{
    m_pNpcSp = Sprite::create(NPC_SP_FILE);
    m_pNpcSp->setPosition(Vec2(800, 740));
    addChild(m_pNpcSp, 5);
    m_pNpcSp->setRotation(-15.0f);
}

//庆祝
void Round012_03::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(2);
    
}

/* 开启触摸 */
void Round012_03::openTouch()
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
        auto rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
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
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        if (m_pAskMarks[0]->getBoundingBox().containsPoint(touchPoint))
        {
            if (clicknumber == 1) { //答对问题
                target->setPosition(TOPIC_BOARD_POS);        //将正确符号设置为目标坐标
                m_pAskMarks[0]->setVisible(false);           //隐藏问号
                fadeOutAnswer();          //隐藏剩余答案
                changeActor();                  //改变跷跷板动作
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
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
                for (unsigned int i = 0; i < 2; i++) {
                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                    dispatcher->removeEventListenersForTarget(m_pAnswers[i]);
                }
                
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                    target->setPosition(m_originalPoints[clicknumber]);
                }), NULL));
            }
        }else
        {
            // 播放答错音效
            playAnswerErrorEffect();
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
        
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pAnswers[0]);
    for (unsigned int i = 1; i < 2; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pAnswers[i]);
    }
}

/*  回合结束回调 */
void Round012_03::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

/*  处理npc收集 */
void Round012_03::processNpcCollect()
{
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);

    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(400, 200), 100, 1),ScaleTo::create(0.5f, 1.1f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.2f), NULL),CallFunc::create([this]{
        m_pNpcSp->setVisible(false);
    }), NULL);
    m_pNpcSp->runAction(npcActions);
    
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


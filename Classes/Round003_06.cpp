//
//  Round003_06.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/24.
//
//

#include "Round003_06.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "AppConfig.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "CustomMessage.h"

#define DUYANGUAI_CONFIG_FILE       "res/stage003/SL-GK03-4-MS01-D+C.json"
#define DUYANGUAI_PNG_FILE          "res/stage003/SL-GK03-4-MS01-D+C/SL-GK03-4-MS01-D+C.png"

#define SL_MAP_BGPNG_FILE           "res/stage003/SL-MAP-BG.png"

#define DICE1_FRAMENAME             "SL-GK03-3-DA01.png"
#define DICE2_FRAMENAME             "SL-GK03-3-DA02.png"
#define DICE3_FRAMENAME             "SL-GK03-3-DA03.png"

#define MOVE_ACTION_TAG             101

#define CORRECT_ANSWER              1

#define DICE_POS                    Vec2(1000.0f, 880.0f)
#define CELEBRATE_DELAY             3.0f                                    // 庆祝延时
#define FADE_OUT_TIME               0.5f                                    //骰子渐变消失时间
#define NPC_COLLECT_DURATION        1.5f
#define NPC_KEY                     "NPC-010"
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define COLORLAYER_TAG                  300
#define HINT_POS                    Vec2(1151.0f, 886.0f)

/*
 *  constructor
 */
Round003_06::Round003_06():
m_pActor(nullptr),
m_pSSNode(nullptr)
{}

/*
 *  destructor
 */
Round003_06::~Round003_06()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
}

//
Round003_06* Round003_06::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round003_06();
    if (pRound && pRound->initWithRoundIndex(roundIndex, pStageScene,pos))
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
bool Round003_06::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadTexture(DUYANGUAI_PNG_FILE);
    appendAsyncLoadGaf("res/stage003/SL-GK03-4-MS01-D+C/SL-GK03-4-MS01-D+C.gaf");
    preLoadRes();
    
    return true;
}

//
void Round003_06::onPreLoadedRes()
{
    /////////////////
    // 必须调用基类函数
    RoundBase::onPreLoadedRes();
    
    // 添加独眼怪 Npc
    m_pActor = Actor::createWithGafConfigFile(DUYANGUAI_CONFIG_FILE);
    m_pActor->setPosition(Vec2(1250.0f, 1100.0f));
    m_pActor->playWithSequenceAliasName("standby", true);
    addChild(m_pActor);
    m_pActor->setScale(0.8f);
    
    m_pSBN = SpriteBatchNode::create(SL_MAP_BGPNG_FILE);
    addChild(m_pSBN, 2);
    //
    float posx = 0.0f, posy = 0.0f, offsetx = 0.0f;
    
    char szBuffer[32];
    posx = 780.0f;posy = -460.0f;offsetx = 240.0f;
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(szBuffer, "SL-GK03-4-DA%02ld.png", i + 1);
        m_pDiceSps[i] = Sprite::createWithSpriteFrameName(szBuffer);
        m_pSBN->addChild(m_pDiceSps[i], 2);
        m_pDiceSps[i]->setUserData((void*)i);
        m_pDiceSps[i]->runAction(EaseBackOut::create(MoveBy::create(0.2f * (i + 1),
                                                                    Vec2(0.0f, 580.0f * (i + 1)))));
    } /*for*/
    
    //随机
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int Index = static_cast<unsigned int>(rand() % 3);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pDiceSps[all]->setPosition(Vec2(posx+offsetx*Index, posy - 580.0f * all));
        m_originalPoints[all] = Vec2(posx+offsetx*Index, 120.0f);
        all++;
    } /*while*/
    
    //
    auto spDice = Sprite::createWithSpriteFrameName("SL-GK03-4-TM01.png");
    spDice->setPosition(DICE_POS);
    addChild(spDice, 1);
    
    m_pHintSp = Sprite::createWithSpriteFrameName("SL-GK03-4-TM03.png");
    m_pHintSp->setPosition(HINT_POS);
    addChild(m_pHintSp, 2);
    m_pHintSp->setOpacity(0);
    
}

void Round003_06::openTouch()
{
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            long index = (long)target->getUserData();
            m_originalPoint = m_originalPoints[index];
            target->setPosition(m_originalPoint);
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            m_pSBN->reorderChild(target, 3);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 m_movePos;
        m_movePos.x = m_originalPoint.x + (touchMovePoint.x - m_touchBeganPoint.x);
        m_movePos.y = m_originalPoint.y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(m_movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        m_pSBN->reorderChild(target, 2);
        long selIndex = (long)target->getUserData();
        auto touchPoint = m_pHintSp->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pHintSp->getContentSize().width, m_pHintSp->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (selIndex == CORRECT_ANSWER) /*答对了*/
            {
                target->setVisible(false);
                auto spDice = Sprite::createWithSpriteFrameName("SL-GK03-4-TM02.png");
                spDice->setPosition(DICE_POS);
                addChild(spDice, 2);
                
                // remove listener
                auto eventDisptacher = Director::getInstance()->getEventDispatcher();
                for (unsigned int i = 0; i < 3; i++)
                {
                    eventDisptacher->removeEventListenersForTarget(m_pDiceSps[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x-320, touch->getLocation().y+220));    //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                if (getEffIndex() != -1)
                {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答对时主角说话
                playLeadAnsCorEff();
                celebrate(CELEBRATE_DELAY);
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
                // 播放答错音效
                playAnswerErrorEffect();
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[selIndex])),CallFunc::create([this,target,selIndex]{
                    target->setPosition(m_originalPoints[selIndex]);
                }), NULL));
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[selIndex])),CallFunc::create([this,target,selIndex]{
                target->setPosition(m_originalPoints[selIndex]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setPosition(m_originalPoint);
        m_pSBN->reorderChild(target, 2);
    };
    auto eventDisptacher = Director::getInstance()->getEventDispatcher();
    eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pDiceSps[0]);
    for (unsigned int i = 1; i < 3; i++) {
        eventDisptacher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pDiceSps[i]);
    } /*for*/
}

//
void Round003_06::onEnterRound()
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
    dispatcher->addCustomEventListener(SHARE_CALLBACK, [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
        shareCallback();
    });
    dispatcher->addCustomEventListener(NPC_COLLECTED_PROMPT_END, [this] (EventCustom *)
    {
       auto dispatcher = Director::getInstance()->getEventDispatcher();
       dispatcher->removeCustomEventListeners(NPC_COLLECTED_PROMPT_END);
       dispatcher->dispatchCustomEvent(ROUND_END_CALLBACK);
    });
    fadeHint();
}

//渐现提示
void Round003_06::fadeHint()
{
    m_pHintSp->runAction(Sequence::create(FadeIn::create(0.3f),FadeOut::create(0.3f),FadeIn::create(0.3f),FadeOut::create(0.3f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

//渐隐消失骰子
void Round003_06::fadeOutDices()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pDiceSps[i]->runAction(FadeOut::create(FADE_OUT_TIME));
    }
}

//
void Round003_06::celebrate(const float delay)
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(3);
    
}

//回合结束分享调用
void Round003_06::roundEndShareCallBack(bool isShare)
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
    processNpcCollect();    //npc收集
    std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
    MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}

//分享回调
void Round003_06::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round003_06::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round003_06::roundEndShareCallBack, this,true));
}

/*  回合结束回调 */
void Round003_06::endRound()
{
    npcCollectOver();
}

//npc收集完成
void Round003_06::npcCollectOver()
{
    m_pActor->playWithSequenceAliasName("trigger");
    m_pActor->runAction(MoveBy::create(2.0f, Vec2(0.0f, 60.0f)));
    fadeOutDices();                 //渐隐骰子
    scheduleOnce([this](float dt) {
        m_pActor->playWithSequenceAliasName("standby", true);
        m_pStageScene->getBeibeiActor()->walk();
        m_pStageScene->getAimeeActor()->walk();
        m_pStageScene->moveLeadingActor(3.0f, Vec2(2000.0f, 0.0f));
    }, 2.2f, "actorJump");
    
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round003_06::fadeOutLayer), 3.0f);

}

//渐变变黑退出
void Round003_06::fadeOutLayer(float)
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

/*  处理npc收集 */
void Round003_06::processNpcCollect()
{
    auto npcCollectedCB = CallFunc::create([this]
                                           {
                                               auto pNpcManager = NpcDataManager::getInstance();
                                               auto pNpcData = pNpcManager->getNpcDataByKey(NPC_KEY);
                                               if (pNpcData != nullptr ) {
                                                   pNpcData->collect();
                                               }
                                               m_pStageScene->getRoundUINode()->setCollectNpcKey(NPC_KEY);
                                               m_pStageScene->getRoundUINode()->changeBagGafActor(2);
                                               
                                           } );
    auto npcBagPos = m_pStageScene->getRoundUINode()->getNpcBagPos();
    auto npcNewBafPos = Vec2(npcBagPos.x-118, npcBagPos.y-111);

    auto npcActions = Sequence::create(Spawn::create(JumpBy::create(0.5f, Vec2(100, 200), 100, 1),ScaleTo::create(0.5f, 0.8f), NULL),Spawn::create(MoveTo::create(0.5f, npcNewBafPos),ScaleTo::create(0.5f, 0.0f), NULL),CallFunc::create([this]{
        m_pActor->setVisible(false);
    }), NULL);
    m_pActor->runAction(Sequence::create(npcActions, npcCollectedCB, NULL));
}

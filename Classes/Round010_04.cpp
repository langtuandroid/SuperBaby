//
//  Round010_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/8.
//
//

#include "Round010_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "SocialShareNode.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_GAF_BG_FILE          "res/common/guanghuanxiaoguo/guanghuanxiaoguo.gaf"
#define CELEBRATE_GAF_FILE             "res/common/wujiaoxing03/wujiaoxing03.gaf"
#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage010/SL-GK10-IT.png"             //BN的png文件
#define BN_ZORDER                      3                                         //BN的z值
#define BACK_SCENE_TIME               4.5f                                      //返回关卡选择场景延时时间
#define MOVE_ACTOR_TIME               2.0f                                      //分享界面ok过后主角移动时间
#define LEAF_GAF_POS                  Vec2(-652,981)                             //荷叶actor的位置
#define LEAF_SP_POS                   Vec2(-130,220)                              //荷叶位置
#define LEAF_GAF_ZORDER               2                                         //荷叶actor的z值
#define LEAF_SP_FILE                  "res/stage010/SL-GK10-4-heye.png"         //荷叶的png
#define LEAF_GAF_FILE                 "res/stage010/SL-GK10-4-TM02-D/SL-GK10-4-TM02-D.gaf"      //荷叶gaf文件路径
#define OCTOPUS_GAF_CONFIG_FILE       "config/stage010/octopus.json"                            //章鱼actor配置文件
#define OCTOPUS_GAF_POS               Vec2(236,1505)                            //章鱼actor位置
#define OCTOPUS_GAF_ZORDER            1                                         //章鱼actor的z值
#define GLOVE_ZORDER                  5                                         //手套z值
#define STANDBY                        "standby"                                 // 待机动画
#define TRIGGER                        "trigger"                                 // 触发动画
#define UP_MOVE_TIME                  0.5f                                      //手套上升时间
#define UP_MOVE_OFFSETY               507                                       //手套上升高度
#define DOWN_MOVE_TIME                0.5f                                      //手套下降时间
#define DOWN_MOVE_OFFSETY             -507                                       //手套下降高度
#define TOPIC_GLOVE_POS               Vec2(1636,554)                            //题目手套位置
#define TOPIC_GLOVE_FADE_TIME         0.5f                                      //题目手套显隐时间
#define MOV_TIME                      0.2f                                    //答错回原位置移动时间
#define END_MOVE_TIME                3.0f                                     //退出回合时移动时间
#define END_MOVE_OFFSETX             2000                                     //退出回合时移动宽度
#define COLORLAYER_TAG               300
#define WAVE_GAF_FILE                "res/stage010/SL-GK10-4-IT01-D/SL-GK10-4-IT01-D.gaf"
#define WAVE_GAF_POS                 Vec2(509,448)
#define WAVE_ZORDER                  2


/*
 *  constructor
 */
Round010_04::Round010_04()
:m_InitAniIndex(0)
{
    // 设置手套坐标
    m_glovesPos[0] = Vec2(1014, -300);
    m_glovesPos[1] = Vec2(1237, -300);
    m_glovesPos[2] = Vec2(1446, -300);
    m_glovesPos[3] = Vec2(1661, -300);
}

/*
 *  destructor
 */
Round010_04::~Round010_04() {
    cleanUp();
}

//
Round010_04* Round010_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round010_04();
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
bool Round010_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage010/SL-GK10-4-TM01-D+C/SL-GK10-4-TM01-D+C.gaf");
    appendAsyncLoadGaf(WAVE_GAF_FILE);
    appendAsyncLoadGaf(LEAF_GAF_FILE);
    appendAsyncLoadTexture(LEAF_SP_FILE);
    appendRemovedTexture(LEAF_SP_FILE);
    preLoadRes();
    
    return true;
}
/*
 *  清理
 */
void Round010_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

void Round010_04::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    //荷叶sp
    m_pLeafSp = Sprite::create(LEAF_SP_FILE);
    m_pLeafSp->setPosition(LEAF_SP_POS);
    this->addChild(m_pLeafSp);
    
    //纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载荷叶
    m_pLeafActor = Actor::createWithGafFile(LEAF_GAF_FILE);
    m_pLeafActor->setPosition(LEAF_GAF_POS);
    addChild(m_pLeafActor, LEAF_GAF_ZORDER);
    
    //加载章鱼
    m_pOctopusActor = Actor::createWithGafConfigFile(OCTOPUS_GAF_CONFIG_FILE);
    m_pOctopusActor->setPosition(OCTOPUS_GAF_POS);
    m_pOctopusActor->playWithSequenceAliasName(STANDBY,true);
    addChild(m_pOctopusActor, OCTOPUS_GAF_ZORDER);
    
    m_pWaveActor = Actor::createWithGafFile(WAVE_GAF_FILE);
    m_pWaveActor->setPosAtNode(Vec2(-300, 600));
    m_pWaveActor->setPosition(WAVE_GAF_POS);
    this->addChild(m_pWaveActor, WAVE_ZORDER);
    
    //加载手套
    char stBuffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(stBuffer, "SL-GK010-4-DA%02ld.png",i+1);
        m_pGlove[i] = Sprite::createWithSpriteFrameName(stBuffer);
        m_pGlove[i]->setUserData((void*)i);
        m_pBN->addChild(m_pGlove[i], GLOVE_ZORDER);
    }
    
    //加载题目手套
    for (unsigned int i = 0; i < 2; i++) {
        sprintf(stBuffer, "SL-GK010-4-TM%02d.png",i+1);
        m_pTopicGloves[i] = Sprite::createWithSpriteFrameName(stBuffer);
        m_pTopicGloves[i]->setPosition(TOPIC_GLOVE_POS);
        m_pBN->addChild(m_pTopicGloves[i], i+3);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int Index = static_cast<unsigned int>(rand() % 4);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pGlove[all]->setPosition(m_glovesPos[Index]);
        m_originalPoints[all] = Vec2(m_glovesPos[Index].x, m_glovesPos[Index].y+507);
        all++;
    } /*while*/
}

//开始回合
void Round010_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round010_04::upGlove), UP_MOVE_TIME, -1, 0.0f);  //上升手套
    this->schedule(CC_SCHEDULE_SELECTOR(Round010_04::gloveFadeSchedule), TOPIC_GLOVE_FADE_TIME+0.5f, -1, 0.0f);  //题目手套显隐
}

//回合开始，手套上升
void Round010_04::upGlove(float)
{
    if (m_InitAniIndex >= 4) {
        unschedule(CC_SCHEDULE_SELECTOR(Round010_04::upGlove));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pGlove[m_InitAniIndex++]->runAction(MoveBy::create(UP_MOVE_TIME, Vec2(0.0f, UP_MOVE_OFFSETY)));
}

//设置题目手套渐变显隐
void Round010_04::gloveFadeSchedule(float dt)
{
    m_pTopicGloves[1]->setOpacity(0);
    m_pTopicGloves[1]->runAction(Sequence::create(FadeIn::create(TOPIC_GLOVE_FADE_TIME),FadeOut::create(TOPIC_GLOVE_FADE_TIME), NULL));
    
}

//庆祝
void Round010_04::celebrate(const float celebrateDelay) {
    
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
void Round010_04::roundEndShareCallBack(bool isShare)
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
    changeOctopusActor();                   //改变章鱼动作
}

//回合结束，手套下降
void Round010_04::downGlove()
{
    for (unsigned int i = 0; i < 4; i++) {
        if (i != 1) {
            m_pGlove[i]->runAction(FadeOut::create(0.3f));
        }
    } /*for*/
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round010_04::fadeOutLayer), END_MOVE_TIME);
    moveActor();
}

//移动主角退出回合
void Round010_04::moveActor()
{
    m_pStageScene->moveLeadingActor(END_MOVE_TIME, Vec2(END_MOVE_OFFSETX, 0.0f));
    m_pLeafActor->runAction(MoveBy::create(END_MOVE_TIME, Vec2(END_MOVE_OFFSETX, 0.0f)));
}

//回合结束，章鱼actor改变动作
void Round010_04::changeOctopusActor()
{
    m_pGlove[1]->setVisible(false);
    m_pOctopusActor->playWithSequenceAliasName(TRIGGER);
    m_pOctopusActor->setAnimationFinishedPlayDelegate([this]{
        downGlove();
        m_pWaveActor->setVisible(false);
    });
}

//渐变变黑退出
void Round010_04::fadeOutLayer(float)
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
    // 过关
    m_pStageScene->stageClear();
}

/* 开启触摸 */
void Round010_04::openTouch()
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
            m_pBN->reorderChild(target, GLOVE_ZORDER + 1);
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
        target->setScale(1.0f);
        m_pBN->reorderChild(target, GLOVE_ZORDER);
        
        unsigned long clicknumber = (unsigned long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, m_pTopicGloves[0]->getContentSize().width, m_pTopicGloves[0]->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            if (clicknumber == 1) { // 答对
                target->setPosition(m_pTopicGloves[0]->getPosition()); //将答对的手套坐标设置为题目手套坐标
                for (unsigned int i = 0; i < 2; i++) {                  //隐藏题目手套
                    m_pTopicGloves[i]->setVisible(false);
                }
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
                this->celebrate(CELEBRATE_DELAY);   //庆祝
                
                for (unsigned int i = 0; i < 4; i++) {
                    auto dispatcher = Director::getInstance()->getEventDispatcher();
                    dispatcher->removeEventListenersForTarget(m_pGlove[i]);
                }
            } else   //答错
            {
                // 播放答错音效
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {            //答对庆祝
                    m_anwerErrorCB();
                }
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                    target->setPosition(m_originalPoints[clicknumber]);
                }), NULL));
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {            //答对庆祝
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_originalPoints[clicknumber]);
            }), NULL));
        }
    };
    listenerRight->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        target->setScale(1.0f);
        m_pBN->reorderChild(target, GLOVE_ZORDER);
    };
    
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pGlove[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pGlove[i]);
    } /*for*/
}

/*  回合结束回调 */
void Round010_04::endRound() {}

//分享回调
void Round010_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round010_04::roundEndShareCallBack, this, false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round010_04::roundEndShareCallBack, this, true));
}

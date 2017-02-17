//
//  Round011_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round012_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_DELAY                7.0f                                    // 庆祝延时
#define BN_PNG_FILE                    "res/stage012/SL-GK12-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                        "standby"                               // 待机动画
#define TRIGGER                        "trigger"                               // 触发动画
#define MOVE_ACTOR_TIME               3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME               2.5f                              //返回关卡选择场景延时时间
#define INIT_UP_TIME                  0.5f                                    // 上升动画时间
#define INIT_UP_OFFSETY               300.0f                                  // 上升动画偏移值
#define MUSHROOM_LEFT_MOVE_TIME       1.0f                                    //蘑菇左移时间
#define MUSHROOM_LEFT_MOVE_OFFSETX    -1500                                   //蘑菇左移宽度
#define MUSHROOM_CHANGE_SCALE_TIME    0.5f                                    //蘑菇改变大小时间
#define MUSHROOM_ZORDER               2                                       //蘑菇z值
#define MUSHROOM_SHADOW_ZORDER        1                                       //蘑菇阴影z值
#define ANSWER_ZORDER                 3                                       //答案z值
#define SPRITE_GAF_CONFIG_FILE        "config/stage012/bean.json"             //精灵gaf配置文件
#define SPRITE_GAF_POS                Vec2(300,1150)                          //精灵gaf位置
#define SPRITE_GAF_ZORDER             1                                       //精灵gaf的z值
#define FADE_OUT_TIME                 1.0f                                    //渐隐时间
#define COLORLAYER_TAG                  300

/*
 *  constructor
 */
Round012_04::Round012_04()
:m_InitAniIndex(0),
m_correctNumber(0)
{
    //蘑菇坐标
    m_mushroomsPos[0] = Vec2(2355, 396);
    m_mushroomsPos[1] = Vec2(2786, 396);
    m_mushroomsPos[2] = Vec2(3214, 396);
    
    //答案坐标
    m_answersPos[0] = Vec2(993, -150);
    m_answersPos[1] = Vec2(1253, -150);
    m_answersPos[2] = Vec2(1518, -150);
}

/*
 *  destructor
 */
Round012_04::~Round012_04() {
    cleanUp();
}

/*
 *  清理
 */
void Round012_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round012_04* Round012_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round012_04();
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
bool Round012_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    appendAsyncLoadGaf("res/stage012/SL-GK12-4-MS01-D+C/SL-GK12-4-MS01-D+C.gaf");
    preLoadRes();
    return true;
}

//
void Round012_04::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    // 纹理加载
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    //加载蘑菇
    char mgBuffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(mgBuffer, "SL-GK012-4-TM%02d.png",i+1);
        m_pMushrooms[i] = Sprite::createWithSpriteFrameName(mgBuffer);
        m_pBN->addChild(m_pMushrooms[i], MUSHROOM_ZORDER);
    }
    
    //加载答案
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(mgBuffer, "SZ3-%02ld.png",i+5);
        m_pAnswerNumbers[i] = Sprite::createWithSpriteFrameName(mgBuffer);
        m_pAnswerNumbers[i]->setUserData((void*)i);
        addChild(m_pAnswerNumbers[i], ANSWER_ZORDER);
    }
    
    //随机设置坐标
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int answerIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[answerIndex]) continue;
        selected[answerIndex] = true;
        m_pMushrooms[all]->setPosition(m_mushroomsPos[answerIndex]);
        all++;
    } /*while*/
    
    //重置
    all = 0;
    for (unsigned i = 0; i < 3; i++) {
        selected[i] = false;
    }
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int answerIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[answerIndex]) continue;
        selected[answerIndex] = true;
        m_pAnswerNumbers[all]->setPosition(m_answersPos[answerIndex]);
        all++;
    } /*while*/
}

//开始回合
void Round012_04::onEnterRound()
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round012_04::mushroomsLeftMoveSchedule), MUSHROOM_LEFT_MOVE_TIME-0.4f, -1, 0);
}

// 回合开场动画，蘑菇左移动画调度
void Round012_04::mushroomsLeftMoveSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_04::mushroomsLeftMoveSchedule));
        m_InitAniIndex = 0;                 //重置为0
        this->schedule(CC_SCHEDULE_SELECTOR(Round012_04::mushroomsChangeScaleSchedule), MUSHROOM_CHANGE_SCALE_TIME*2+0.1f, -1, MUSHROOM_LEFT_MOVE_TIME);      //蘑菇放大缩小
        return;
    }
    m_pMushrooms[m_InitAniIndex++]->runAction(MoveBy::create(MUSHROOM_LEFT_MOVE_TIME, Vec2(MUSHROOM_LEFT_MOVE_OFFSETX, 0.0f)));
}

// 回合开场动画，蘑菇放大缩小动画调度
void Round012_04::mushroomsChangeScaleSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_04::mushroomsChangeScaleSchedule));
        m_InitAniIndex = 0;                 //重置为0
        this->schedule(CC_SCHEDULE_SELECTOR(Round012_04::numberUpSchedule), INIT_UP_TIME, -1, MUSHROOM_CHANGE_SCALE_TIME*2);      //答案上升
        return;
    }
    m_pMushrooms[m_InitAniIndex]->setOpacity(200);
    m_pMushrooms[m_InitAniIndex]->runAction(Sequence::create(ScaleTo::create(MUSHROOM_CHANGE_SCALE_TIME, 1.1f),ScaleTo::create(MUSHROOM_CHANGE_SCALE_TIME, 1.0f),CallFunc::create([this]{
        m_pMushrooms[m_InitAniIndex]->setOpacity(255);
        m_InitAniIndex++;
    }), NULL));
}

/*
 *  回合开场动画，小数字上升动画调度
 */
void Round012_04::numberUpSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_04::numberUpSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    
    MoveBy* move = MoveBy::create(INIT_UP_TIME, Vec2(0.0f, INIT_UP_OFFSETY));
    m_pAnswerNumbers[m_InitAniIndex++]->runAction(EaseBackOut::create(move));
}

//回合结束，渐隐正确的答案数字
void Round012_04::fadeOutTrueNumber(float)
{
    m_pAnswerNumbers[2]->runAction(Sequence::create(FadeOut::create(FADE_OUT_TIME),CallFunc::create([this]{
        m_InitAniIndex = 0;     //重置为0
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round012_04::fadeOutTrueMushroom), 0.0f);
    }), NULL));
}

//回合结束，渐隐正确数字的蘑菇
void Round012_04::fadeOutTrueMushroom(float)
{
    m_pMushrooms[2]->setOpacity(200);
    m_pMushrooms[2]->runAction(Sequence::create(ScaleTo::create(MUSHROOM_CHANGE_SCALE_TIME, 1.05f),Spawn::create(ScaleTo::create(MUSHROOM_CHANGE_SCALE_TIME, 1.0f),FadeOut::create(MUSHROOM_CHANGE_SCALE_TIME),CallFunc::create([this]{
        m_InitAniIndex = 0;     //重置为0
        this->schedule(CC_SCHEDULE_SELECTOR(Round012_04::fadeOutRemainMushrroomAndNumbers), FADE_OUT_TIME, -1, 0);
    }), NULL), NULL));
}

//回合结束，渐隐剩下的数字和蘑菇
void Round012_04::fadeOutRemainMushrroomAndNumbers(float)
{
    if (m_InitAniIndex >= 2) {
        unschedule(CC_SCHEDULE_SELECTOR(Round012_04::fadeOutRemainMushrroomAndNumbers));
        addSpriteGafActor();                        //添加精灵actor
        return;
    }

    m_pMushrooms[m_InitAniIndex]->runAction(FadeOut::create(FADE_OUT_TIME));
    
    m_pAnswerNumbers[m_InitAniIndex]->runAction(FadeOut::create(FADE_OUT_TIME));
    m_InitAniIndex++;
}

//添加精灵gaf
void Round012_04::addSpriteGafActor()
{
    m_pSpriteActor = Actor::createWithGafConfigFile(SPRITE_GAF_CONFIG_FILE);
    m_pSpriteActor->setPosition(SPRITE_GAF_POS);
    addChild(m_pSpriteActor, SPRITE_GAF_ZORDER);
    m_pSpriteActor->playWithSequenceAliasName(STANDBY);
}

//庆祝
void Round012_04::celebrate(const float celebrateDelay) {
    
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
void Round012_04::roundEndShareCallBack(bool isShare)
{
    if (isShare) {
        auto shareBackGround = m_pStageScene->getRoundUINode()->getSocialShareNode()->getShareBackGround();
        shareBackGround->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    } else {
        auto layer = m_pStageScene->getRoundUINode()->getSocialShareNode()->getLayer();
        layer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            m_pStageScene->getRoundUINode()->getSocialShareNode()->removeFromParent();
        }), NULL));
    }
    m_InitAniIndex = 1;                 //重置为1
    m_pSpriteActor->playWithSequenceAliasName(TRIGGER);
    m_pSpriteActor->setAnimationFinishedPlayDelegate([this]
    {
        m_pSpriteActor->setVisible(false);
        moveActor();
    });
}

//移动主角退出回合
void Round012_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round012_04::fadeOutLayer), MOVE_ACTOR_TIME);    //退出
}

/* 开启触摸 */
void Round012_04::openTouch()
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
            if (clicknumber == 2) { //答对问题
                this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round012_04::fadeOutTrueNumber), 0.3f);   //隐藏答案
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
                    dispatcher->removeEventListenersForTarget(m_pAnswerNumbers[i]);
                } /*for*/
            } else //答错
            {
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
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pAnswerNumbers[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pAnswerNumbers[i]);
    }
}

//主角移动完成
void Round012_04::moveActorComplete(float dt)
{
    MyAudioEngine::getInstance()->stopAudio(m_effectId);          //停止分享音效
    // 返回魔法森林大地图
    m_pStageScene->stageClear();
}

/*  回合结束回调 */
void Round012_04::endRound()
{
    
}

//渐变变黑退出
void Round012_04::fadeOutLayer(float)
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
    }, 0.005f, "add");
}

//分享回调
void Round012_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round012_04::roundEndShareCallBack, this, false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round012_04::roundEndShareCallBack, this, true));
}

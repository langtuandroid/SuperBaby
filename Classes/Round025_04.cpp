//
//  Round025_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/6.
//
//

#include "Round025_04.h"
#include "Player.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "ThemeSelScene.h"
#include "GafAssetCache.h"

#define BN_PNG_FILE                    "res/stage025/HY-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define STARTFISH_ZORDER                2                                           //海星z值
#define TARGET_STARTFISH_ZORDER         1                                           //目标海星z值

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define Fade_TIME                       0.5f                                       // 海星渐变时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define SCALE_TIME                      1.0f                                    //海星变大时间

#define NUMBER_OF_ANSWER_RIGHT          3                                       // 答题正确数量达到该数字怎本回合结束
#define MOVE_ACTOR_TIME               3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME               3.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300
#define PASS_GAF_FILE                   "res/common/yanshi01/yanshi01.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3
#define BOGUANG_GAF_FILE                "res/stage025/HY-GK-boguang/HY-GK-boguang.gaf"
#define BOGUANG_GAF_POS                 Vec2(224,1126)
#define BOGUANG_GAF_ZORDER              1

/*
 *  constructor
 */
Round025_04::Round025_04()
:m_InitAniIndex(0)
{
    m_startfishsPos[0] = Vec2(833, 937);
    m_startfishsPos[1] = Vec2(1198, 1129);
    m_startfishsPos[2] = Vec2(1533, 937);
    
    m_targetStartfishPos[0] = Vec2(817, 293);
    m_targetStartfishPos[1] = Vec2(1206, 293);
    m_targetStartfishPos[2] = Vec2(1605, 293);
    
    m_nameStr[0] = "HY-GK01-4-TM02-2.png";
    m_nameStr[1] = "HY-GK01-4-TM03-2.png";
    m_nameStr[2] = "HY-GK01-4-TM01-2.png";
}

/*
 *  destructor
 */
Round025_04::~Round025_04()
{
    cleanUp();
}

//
Round025_04* Round025_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round025_04();
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
bool Round025_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBoguangActor = Actor::createWithGafFile(BOGUANG_GAF_FILE);
    m_pBoguangActor->setPosition(BOGUANG_GAF_POS);
    this->addChild(m_pBoguangActor, BOGUANG_GAF_ZORDER);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK01-4-DA%02ld.png",i+1);
        m_pStarfishs[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBN->addChild(m_pStarfishs[i], STARTFISH_ZORDER);
        m_pStarfishs[i]->setUserData((void*)i);
        m_pStarfishs[i]->setScale(0.0f);
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(buffer, "HY-GK01-4-TM%02d-1.png",i+1);
        m_pTargetStarfishs[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        this->addChild(m_pTargetStarfishs[i], TARGET_STARTFISH_ZORDER);
        m_pTargetStarfishs[i]->setOpacity(0);
    }
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int startfishIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[startfishIndex]) continue;
        selected[startfishIndex] = true;
        m_pStarfishs[all]->setPosition(m_startfishsPos[startfishIndex]);
        m_startfishEndPos[all] = m_startfishsPos[startfishIndex];
        all++;
    } /*while*/
    
    all = 0;
    selected[0] = false;
    selected[1] = false;
    selected[2] = false;
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int startfishIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[startfishIndex]) continue;
        selected[startfishIndex] = true;
        m_pTargetStarfishs[all]->setPosition(m_targetStartfishPos[startfishIndex]);
        all++;
    } /*while*/
    
    return true;
}

void Round025_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
}

// 进入本回合
void Round025_04::onEnterRound()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
//    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
//        auto dispatcher = Director::getInstance()->getEventDispatcher();
//        dispatcher->removeCustomEventListeners("openTouch");
//        openTouch();
//    });
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round025_04::scaleStarfishSchedule), SCALE_TIME+0.2f, -1, 0);
}

//海星变大出现
void Round025_04::scaleStarfishSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_04::scaleStarfishSchedule));
        m_InitAniIndex = 0;
        this->schedule(CC_SCHEDULE_SELECTOR(Round025_04::fadeOutStarfishSchedule), Fade_TIME, -1, 0);
        if (!UserDefault::getInstance()->getBoolForKey("slidefour", false)) {
            UserDefault::getInstance()->setBoolForKey("slidefour", true);
            playPassActor();
        }
        return;
    }
    m_pStarfishs[m_InitAniIndex]->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(SCALE_TIME, 1.0f)),CallFunc::create([this]{
        m_pStarfishs[m_InitAniIndex]->runAction(RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -10.0f)),MoveBy::create(0.5f, Vec2(0.0f, 10.0f)), NULL)));
        m_InitAniIndex++;
    }), NULL));
}

//目标海星渐变出现
void Round025_04::fadeOutStarfishSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_04::fadeOutStarfishSchedule));
        m_InitAniIndex = 0;   //重置为0
        this->schedule(CC_SCHEDULE_SELECTOR(Round025_04::scaleChangeStarfishSchedule), 0.2f, -1, 0);
        return;
    }
    m_pTargetStarfishs[m_InitAniIndex++]->runAction(FadeIn::create(Fade_TIME));
}

//海星依次变大变小
void Round025_04::scaleChangeStarfishSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_04::scaleChangeStarfishSchedule));
        m_InitAniIndex = 0;   //重置为0
        this->schedule(CC_SCHEDULE_SELECTOR(Round025_04::scaleChangeTargetStarfishSchedule), 0.2f, -1, 0);
        return;
    }
    m_pStarfishs[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.2f, 1.2f),ScaleTo::create(0.2f, 1.0f), NULL));
}

//目标海星依次变大变小
void Round025_04::scaleChangeTargetStarfishSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round025_04::scaleChangeTargetStarfishSchedule));
        if (!UserDefault::getInstance()->getBoolForKey("slidetwo", false)) {
            playPassActor();
            UserDefault::getInstance()->setBoolForKey("slidetwo", true);
        }else
        {
            openTouch();    //开启触摸
        }
        return;
    }
    m_pTargetStarfishs[m_InitAniIndex++]->runAction(Sequence::create(ScaleTo::create(0.2f, 1.2f),ScaleTo::create(0.2f, 1.0f), NULL));
}

/* 播放过关引导 */
void Round025_04::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->addChild(colorLayer,PASS_GAF_ZORDER);
    // 事件派发
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
    passActor->setPosition(Vec2(570, 300));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]{
        colorLayer->removeFromParent();
        passActor->setVisible(false);
        openTouch();        //开启触摸
    });
}

//
void Round025_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
//    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
    GafAssetCache::getInstance()->removeGafAssetForKey(PASS_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(BOGUANG_GAF_FILE);

}

/* 开启触摸 */
void Round025_04::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
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
        long index = (long)target->getUserData();
        target->setScale(1.0f);
        
        ImageView* pSp = nullptr;
        bool isCorrect = false;
        
        Rect rect = Rect(0.0f, 0.0f,
                         m_pTargetStarfishs[0]->getContentSize().width,
                         m_pTargetStarfishs[0]->getContentSize().height);
        if (rect.containsPoint(m_pTargetStarfishs[0]->convertTouchToNodeSpace(touch)) )
        {
            if (!isCorrect && 2 == index)
            {
                pSp = m_pTargetStarfishs[0];
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pTargetStarfishs[1]->getContentSize().width,
                    m_pTargetStarfishs[1]->getContentSize().height);
        if (rect.containsPoint(m_pTargetStarfishs[1]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && 0 == index)
            {
                pSp = m_pTargetStarfishs[1];
                isCorrect = true;
            }
        }
        rect = Rect(0.0f, 0.0f,
                    m_pTargetStarfishs[2]->getContentSize().width,
                    m_pTargetStarfishs[2]->getContentSize().height);
        if (rect.containsPoint(m_pTargetStarfishs[2]->convertTouchToNodeSpace(touch)))
        {
            if (!isCorrect && 1 == index)
            {
                pSp = m_pTargetStarfishs[2];
                isCorrect = true;
            }
        }
        if (isCorrect) /* 答题正确 */
        {
            target->setVisible(false);
            m_numberOfdragComplete++;
            pSp->loadTexture(m_nameStr[index],ui::Widget::TextureResType::PLIST);
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
            // 播放答对音效
            playAnswerCorrectEffect();
            
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            //答对时主角说话
            playLeadAnsCorEff();
            
            if (NUMBER_OF_ANSWER_RIGHT == m_numberOfdragComplete) /* 全部答对，过关 */
            {
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pStarfishs[i]);
                } /*for*/
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            } else {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(target);
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_startfishEndPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_startfishEndPos[index]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        target->setPosition(m_startfishsPos[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pStarfishs[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pStarfishs[i]);
    } /*for*/
    
}

/*
 *  庆祝
 */
void Round025_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    //播放关卡结束分享音效
    auto pStageData = m_pStageScene->getStageData();
    m_effectId = MyAudioEngine::getInstance()->playAudio(pStageData->getShareAudio().c_str());
    
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(3);
}

/*  回合结束回调 */
void Round025_04::endRound() {}

/*
 *  分享界面点击ok后移动主角
 */
void Round025_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk(); m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));
    
}

//回合结束分享调用
void Round025_04::roundEndShareCallBack(bool isShare)
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
    fadeOutStarfishs();               //渐变隐藏海星
    moveActor();                      //移动主角
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round025_04::fadeOutLayer), BACK_SCENE_TIME-1.5f);
}

//渐变隐藏海星
void Round025_04::fadeOutStarfishs()
{
    for (unsigned int i = 0; i < 3; i++) {
        m_pTargetStarfishs[i]->runAction(FadeOut::create(Fade_TIME));
    }
}

//渐变变黑退出
void Round025_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt){
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            }else
            {
                // 过关
                m_pStageScene->stageClear();
                //试玩时停止播放背景音乐
                if (m_pStageScene->isTrial())
                {
                    MyAudioEngine::getInstance()->stopAudio(0,true);
                }
            }

            return;
        }
        m_LayerColor->setOpacity(m_opacity++);
    }, 0.001f, "add");
}

//分享回调
void Round025_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round025_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round025_04::roundEndShareCallBack, this,true));
}

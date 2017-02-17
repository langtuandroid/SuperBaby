//
//  Round037_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/08.
//
//

#include "Round037_04.h"
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

#define BN_PNG_FILE                    "res/stage037/GC-GK01-IT.png"               // BN的 png 文件
#define BN_ZORDER                       2                                           // BN的z值
#define GRAPHIC_ZORDER                  6                                           //图形z值

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间
#define SCALE_TIME                      1.0f                                    //图形变大时间

#define MOVE_ACTOR_TIME                3.0f                              //分享界面ok过后主角移动时间
#define BACK_SCENE_TIME                2.5f                              //返回关卡选择场景延时时间
#define COLORLAYER_TAG                  300

#define HOLE_ZORDER                     3
#define DOOR_ZORDER                     4
#define WALL_TAG                        500
#define WALL_POS                        Vect(1905,590)
#define MOVE_TIME                       1.0f
#define MOVE_OFFSETX                    300
#define WENHAO_ZORDER                   3

/*
 *  constructor
 */
Round037_04::Round037_04()
:m_InitAniIndex(0)
{
    m_graphicsPos[0] = Vec2(733, 134);
    m_graphicsPos[1] = Vec2(962, 134);
    m_graphicsPos[2] = Vec2(1162, 134);
    
    m_touchRect = Rect(937, 733, 162, 140);
    m_touchCentrePos = Vec2(1018, 803);
    
    m_targetPos = Vec2(1018, 803);
    
    m_holePos = Vec2(1651, 718);
    
    m_doorPos = Vec2(1779, 501);
}

/*
 *  destructor
 */
Round037_04::~Round037_04()
{
    cleanUp();
}

//
Round037_04* Round037_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round037_04();
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
bool Round037_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;

    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    char buffer[64];
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(buffer, "GC-GK01-4-DA%02ld.png",i+1);
        m_pGraphics[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pBN->addChild(m_pGraphics[i], GRAPHIC_ZORDER);
        m_pGraphics[i]->setUserData((void*)i);
        m_pGraphics[i]->setScale(0.0f);
    }
    
    m_pTargetSp = Sprite::createWithSpriteFrameName("GC-GK01-4-TM.png");
    m_pTargetSp->setPosition(m_targetPos);
    m_pBN->addChild(m_pTargetSp);
    
    m_pMark = Sprite::createWithSpriteFrameName("GC-GK01-4-wenhao.png");
    m_pMark->setPosition(m_touchCentrePos);
    m_pBN->addChild(m_pMark, WENHAO_ZORDER);
    
    m_pHole = Sprite::createWithSpriteFrameName("GC-GK01-bj03-back.png");
    m_pHole->setPosition(m_holePos);
    m_pBN->addChild(m_pHole, HOLE_ZORDER);
    
    m_pDoor = Sprite::createWithSpriteFrameName("GC-GK01-bj03-m.png");
    m_pDoor->setPosition(m_doorPos);
    m_pBN->addChild(m_pDoor, DOOR_ZORDER);
    
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int graphicIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[graphicIndex]) continue;
        selected[graphicIndex] = true;
        m_pGraphics[all]->setPosition(m_graphicsPos[graphicIndex]);
        m_randGraphicPos[all] = m_graphicsPos[graphicIndex];
        all++;
    } /*while*/
    
    return true;
}

void Round037_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
}

// 进入本回合
void Round037_04::onEnterRound()
{
    m_pOverActorNode = Node::create();
    m_pStageScene->addOverLeaderNode(m_pOverActorNode, this->getPosition(), WALL_TAG);
    
    m_pWall = Sprite::createWithSpriteFrameName("GC-GK01-bj03-f.png");
    m_pWall->setPosition(WALL_POS);
    m_pOverActorNode->addChild(m_pWall,7);
    m_pWall->setVisible(false);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener("openTouch", [this] (EventCustom *) {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
        openTouch();
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
    this->schedule(CC_SCHEDULE_SELECTOR(Round037_04::graphicScaleChangeSchedule), SCALE_TIME, -1, 0);
}

//回合开始改变提示问号的大小
void Round037_04::scaleChangeMark()
{
    m_pMark->runAction(Sequence::create(ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f),ScaleTo::create(0.3f, 1.2f),ScaleTo::create(0.3f, 1.0f), NULL));
}

//图案变大
void Round037_04::graphicScaleChangeSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round037_04::graphicScaleChangeSchedule));
        scaleChangeMark();
        return;
    }
    m_pGraphics[m_InitAniIndex++]->runAction(EaseBackOut::create(ScaleTo::create(SCALE_TIME, 1.0f)));
}

//过关移动门
void Round037_04::rightMoveDoor()
{
    m_pWall->setVisible(true);
    m_pDoor->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(600.0f, 0.0f)),CallFunc::create([this]{
        moveActor();
    }), NULL));
}

//
void Round037_04::cleanUp()
{
    m_pStageScene->removeOBLeaderNode(WALL_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);

    m_pStageScene->removeOBLeaderNode(WALL_TAG);
}

/* 开启触摸 */
void Round037_04::openTouch()
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
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        target->setScale(1.0f);
        if (m_touchRect.containsPoint(touchPoint))
        {
            if (index == 2) {   //答对
                m_pMark->setVisible(false);
                target->setPosition(m_touchCentrePos);
                target->setScale(0.8f);
                playAnswerCorrectEffect();
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                //答对时主角说话
                playLeadAnsCorEff();
                
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());
                celebrate(CELEBRATE_DELAY);
                scaleChangeGraphic();
                for (unsigned int i = 0; i < 3; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pGraphics[i]);
                } /*for*/
            } else {
                playAnswerErrorEffect();
                if (getEffIndex() != -1) {
                    MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                }
                // 答错时主角说话
                playLeadAnsErrEff();
                if (m_anwerErrorCB) {
                    m_anwerErrorCB();
                }
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_randGraphicPos[index])),CallFunc::create([this,target,index]{
                    target->setPosition(m_randGraphicPos[index]);
                }), NULL));
            }
        } else {
            playAnswerErrorEffect();
            if (getEffIndex() != -1) {
                MyAudioEngine::getInstance()->stopAudio(getEffIndex());
            }
            // 答错时主角说话
            playLeadAnsErrEff();
            if (m_anwerErrorCB) {
                m_anwerErrorCB();
            }
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_randGraphicPos[index])),CallFunc::create([this,target,index]{
                target->setPosition(m_randGraphicPos[index]);
            }), NULL));
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pGraphics[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_pGraphics[i]);
    } /*for*/
}

/*
 *  庆祝
 */
void Round037_04::celebrate(const float celebrateDelay /*庆祝延时*/ )
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
void Round037_04::endRound() {}

//缩小剩余图形
void Round037_04::scaleChangeGraphic()
{
    for (unsigned int i = 0; i < 2; i++) {
        m_pGraphics[i]->runAction(ScaleTo::create(SCALE_TIME, 0.0f));
    }
}

/*
 *  分享界面点击ok后移动主角
 */
void Round037_04::moveActor()
{
    m_pStageScene->getBeibeiActor()->walk();
    m_pStageScene->getAimeeActor()->walk();
    m_pStageScene->moveLeadingActor(MOVE_ACTOR_TIME, Vec2(2000, 0));
}

//回合结束分享调用
void Round037_04::roundEndShareCallBack(bool isShare)
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
    rightMoveDoor();                //移动门
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round037_04::fadeOutLayer), BACK_SCENE_TIME-1.5f);
}

//渐变变黑退出
void Round037_04::fadeOutLayer(float)
{
    auto size = Director::getInstance()->getVisibleSize();
    m_LayerColor = LayerColor::create(Color4B(0, 0, 0, 0), size.width, size.height);
    m_pStageScene->addOverAllNode(m_LayerColor, COLORLAYER_TAG);
    this->schedule([this](float dt)
    {
        if (m_opacity == 200) {
            unschedule("add");
            MyAudioEngine::getInstance()->stopAudio(m_effectId);    //停止分享音效
            if (m_pStageScene->isTrial())
            {
                Director::getInstance()->replaceScene(ThemeSelScene::create());
            } else {
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
void Round037_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round037_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round037_04::roundEndShareCallBack, this,true));
}

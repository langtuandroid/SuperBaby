//
//  Round006_04.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/2.
//
//

#include "Round006_04.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "SocialShareNode.h"
#include "LeadingActor.h"
#include "Player.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "CustomMessage.h"

#define CELEBRATE_DELAY                5.0f                                      // 庆祝延时
#define BACK_SCENE_TIME                2.5f                                      //返回关卡选择场景延时时间
#define INIT_UP_TIME                   1.0f                                      // 上升动画时间
#define INIT_UP_OFFSETX                400                                       //上升高度
#define BN_PNG_FILE                    "res/stage006/SL-GK06-IT.png"             //纹理png图
#define WINDMILL_BN_ZORDER             1                                         //风车纹理Z值
#define COLOR_BN_ZORDER                2                                         //颜色纹理Z值
#define BOARD_ZORDER                   7                                         //木板z值
#define COLOR_ZORDER                   8                                         //颜色z值
#define BOARD_POS                      Vec2(1218.0f,-241.0f)                      //木板位置
#define BASKET_POS                     Vec2(1201.0f,351.0f)                      //篮子位置
#define BOARD_PNE_NAME                 "SL-GK06-4-IT01.png"                      //颜色板png
#define BASKET_BACK_PNG_NAME           "SL-GK06-4-IT03.png"                      //篮子后部png
#define BASKET_FRONT_PNG_NAME          "SL-GK06-4-IT04.png"                      //篮子前部png
#define STICK_PNG_NAME                 "SL-GK06-4-IT05.png"                      //风车棒
#define BASKET_BACK_ZORDER             3                                         //篮子后部分z值
#define BASKET_FRONT_ZORDER            6                                         //篮子前部分z值
#define STICK_BACK_ZORDER              1                                         //中间风车棒z值
#define STICK_FRONT_ZORDER             4                                         //左右风车棒z值
#define WINDMILL_BACK_ZORDER           2                                         //中间风车z值
#define WINDMILL_FRONT_ZORDER          5                                         //左右风车z值
#define COLOR_SCALE_CHANGE_TIME        0.5f                                      //颜色节点改变尺寸时间
#define ROTATE_TIME                    1.0f                                      //旋转时间
#define ROTATE_ANGLE                   -360.0f                                    //旋转角度
#define NODE_TAG                       4000                                      //上层节点tag值（在主角之上的）
#define MOVE_TIME                      2.5f                                      //篮子移动时间
#define MOVE_POS                       Vec2(1000,1400)                             //篮子移动的相对距离
#define COLORLAYER_TAG                  300
#define MOV_TIME                        0.2f                                    //答错回原位置移动时间

/*
 *  constructor
 */
Round006_04::Round006_04()
{}

/*
 *  destructor
 */
Round006_04::~Round006_04() {
    cleanUp();
}

/*
 *  清理
 */
void Round006_04::cleanUp()
{
    m_pStageScene->removeOBAllNode(COLORLAYER_TAG);
    m_pStageScene->removeOBLeaderNode(NODE_TAG);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners(SHARE_CALLBACK);
}

//
Round006_04* Round006_04::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round006_04();
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
bool Round006_04::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    //设置颜色位置
    m_colorsPos[0] = Vec2(900, -212);
    m_colorsPos[1] = Vec2(1118, -212);
    m_colorsPos[2] = Vec2(1321, -212);
    m_colorsPos[3] = Vec2(1523, -212);
    
    //设置风车位置
    m_windmillsPos[0] = Vec2(785, 735);
    m_windmillsPos[1] = Vec2(1218, 735);
    m_windmillsPos[2] = Vec2(1629, 735);
    
    //设置风车棒位置
    m_sticksPos[0] = Vec2(778, 563);
    m_sticksPos[1] = Vec2(1190, 563);
    m_sticksPos[2] = Vec2(1649, 563);
    
    //创建风车BN
    m_pWindmillBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pWindmillBN,WINDMILL_BN_ZORDER);
    
    
    m_pNode = Node::create();
    m_pStageScene->addOverLeaderNode(m_pNode, Vec2(11000, 0), NODE_TAG);   //将节点添加到主角所在节点
    //创建颜色BN
    m_pColorBN = SpriteBatchNode::create(BN_PNG_FILE);
    m_pNode->addChild(m_pColorBN,WINDMILL_BN_ZORDER);
    
    
    
    //加载板
    m_pColorBoard = Sprite::createWithSpriteFrameName(BOARD_PNE_NAME);
    m_pColorBoard->setPosition(BOARD_POS);
    m_pColorBN->addChild(m_pColorBoard, BOARD_ZORDER);
    
    //加载颜色
    char buffer[64];
    for (unsigned long i = 0; i < 4; i++) {
        sprintf(buffer, "SL-GK06-3-DA%02ld.png",i+1);
        m_pColors[i] = Sprite::createWithSpriteFrameName(buffer);
        m_pColors[i]->setUserData((void*)i);
        m_pColorBN->addChild(m_pColors[i], COLOR_ZORDER);
    }
    
    //随机
    unsigned int all = 0;
    bool selected[4] = {false, false, false, false};
    srand(unsigned(time(0)));
    while (all < 4) {
        unsigned int Index = static_cast<unsigned int>(rand() % 4);
        if (selected[Index]) continue;
        selected[Index] = true;
        m_pColors[all]->setPosition(m_colorsPos[Index]);
        m_colorsEndPos[all] = Vec2(m_colorsPos[Index].x, m_colorsPos[Index].y+400);
        all++;
    } /*while*/
    
    //加载篮子
    m_pBasketBack = Sprite::createWithSpriteFrameName(BASKET_BACK_PNG_NAME);
    m_pBasketBack->setPosition(BASKET_POS);
    m_pWindmillBN->addChild(m_pBasketBack, BASKET_BACK_ZORDER);
    
    m_PBasketFront = Sprite::createWithSpriteFrameName(BASKET_FRONT_PNG_NAME);
    m_PBasketFront->setPosition(BASKET_POS);
    m_pColorBN->addChild(m_PBasketFront, BASKET_FRONT_ZORDER);
    
    for (unsigned long i = 0; i < 3; i++) {
        if (i == 2) {
            sprintf(buffer, "SL-GK06-4-TM02.png");
        }else
        {
            sprintf(buffer, "SL-GK06-4-TM01.png");
        }
        m_pWindmill[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
        this->addChild(m_pWindmill[i], WINDMILL_BACK_ZORDER);
        m_pWindmill[i]->setPosition(m_windmillsPos[i]);
    }
    
    m_pHintSp = Sprite::createWithSpriteFrameName("SL-GK06-4-IT02.png");
    m_pHintSp->setPosition(Vec2(1685, 821));
    this->addChild(m_pHintSp, WINDMILL_FRONT_ZORDER);
    m_pHintSp->setOpacity(0);
    
    //加载风车棒
    for (unsigned int i = 0; i < 3; i++) {
        m_pSticks[i] = Sprite::createWithSpriteFrameName(STICK_PNG_NAME);
        m_pSticks[i]->setPosition(m_sticksPos[i]);
        if (i == 1) {
            m_pWindmillBN->addChild(m_pSticks[i], STICK_BACK_ZORDER);
        }else
        {
            m_pWindmillBN->addChild(m_pSticks[i], STICK_FRONT_ZORDER);
        }
    }
    
    return true;
}

void Round006_04::onPreLoadedRes()
{
    /////
    RoundBase::onPreLoadedRes();
    
    
}

//开始回合
void Round006_04::onEnterRound()
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
    moveBoardUp();   //颜色板上升
}

//出场时向上移动颜色板
void Round006_04::moveBoardUp()
{
    m_pColorBoard->runAction(Sequence::create(MoveBy::create(INIT_UP_TIME, Vec2(0, INIT_UP_OFFSETX)),CallFunc::create(CC_CALLBACK_0(Round006_04::changeScaleColorNodes, this)), NULL));
    for (unsigned int i = 0; i < 4; i++) {
        m_pColors[i]->runAction(MoveBy::create(INIT_UP_TIME, Vec2(0, INIT_UP_OFFSETX)));
    }
}

//颜色节点依次放大缩小
void Round006_04::changeScaleColorNodes()
{
    int offSetTime = 1.0f;
    for (unsigned int i = 0; i < 4; i++) {
        m_pColors[i]->runAction(Sequence::create(DelayTime::create(offSetTime*i),ScaleTo::create(COLOR_SCALE_CHANGE_TIME, 1.1f),ScaleTo::create(COLOR_SCALE_CHANGE_TIME, 1.0f), NULL));        
    }
    scheduleOnce(CC_SCHEDULE_SELECTOR(Round006_04::fadeHint), 4.0f);
}

//显示提示区域
void Round006_04::fadeHint(float)
{
    m_pHintSp->runAction(Sequence::create(FadeIn::create(0.3f),FadeOut::create(0.3f),FadeIn::create(0.3f),FadeOut::create(0.3f),CallFunc::create([this]{
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
    }), NULL));
}

/* 开启触摸 */
void Round006_04::openTouch()
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
        Rect rect = Rect(0.0f, 0.0f,
                         m_pHintSp->getContentSize().width,
                         m_pHintSp->getContentSize().height);
        target->setScale(1.0f);
        unsigned long clicknumber = (unsigned long)target->getUserData();
        if (rect.containsPoint(m_pHintSp->convertTouchToNodeSpace(touch))) {
            if (clicknumber == 3) { //答对问题
                target->runAction(FadeOut::create(0.3f));
                m_pWindmill[2]->loadTexture("SL-GK06-4-TM01.png",ui::Widget::TextureResType::PLIST);
                m_pStageScene->playAnswerCorrectEffect(touch->getLocation());     //播放答对特效
                // 播放答对音效
                playAnswerCorrectEffect();
                scheduleOnce(CC_SCHEDULE_SELECTOR(Round006_04::answerComplete), 0.5f);       //转动风车
                m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                this->celebrate(CELEBRATE_DELAY);
                for (unsigned int i = 0; i < 4; i++) {
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pColors[i]);
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
                target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_colorsEndPos[clicknumber])),CallFunc::create([this,target,clicknumber]{
                    target->setPosition(m_colorsEndPos[clicknumber]);
                }), NULL));
            }
        }else
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
            target->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_colorsEndPos[clicknumber])),CallFunc::create([this,target,clicknumber]{
                target->setPosition(m_colorsEndPos[clicknumber]);
            }), NULL));
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pColors[0]);
    for (unsigned int i = 1; i < 4; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pColors[i]);
    }
}

//全部回答完成
void Round006_04::answerComplete(float)
{
    m_pWindmill[0]->runAction(RepeatForever::create(RotateBy::create(ROTATE_TIME, ROTATE_ANGLE)));
    m_pWindmill[1]->runAction(RepeatForever::create(RotateBy::create(ROTATE_TIME, ROTATE_ANGLE)));
    m_pWindmill[2]->runAction(RepeatForever::create(RotateBy::create(ROTATE_TIME, ROTATE_ANGLE)));
}

//庆祝
void Round006_04::celebrate(const float celebrateDelay) {
    
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
void Round006_04::roundEndShareCallBack(bool isShare)
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
    changeActor();                        //主角跳跃到篮子
}

/*
 *  分享界面点击ok后移动主角
 */

void Round006_04::changeActor()
{
    m_pStageScene->getAimeeActor()->jump([this]{
        Vec2 pos = m_pStageScene->getAimeeActor()->getPosition();
        m_pStageScene->getAimeeActor()->setPosition(Vec2(pos.x+758, pos.y+60));
        m_pStageScene->getAimeeActor()->wait();             //艾米回到等待状态
        m_pStageScene->getBeibeiActor()->jump([this]{
            Vec2 pos = m_pStageScene->getBeibeiActor()->getPosition();
            m_pStageScene->getBeibeiActor()->setPosition(Vec2(pos.x+650, pos.y+70));
            m_pStageScene->getBeibeiActor()->wait();           //蓓蓓回到等待状态
            changeActorComplete();            //主角跳跃完成
        });                                     //蓓蓓后跳

    });          //艾米先跳

}

void Round006_04::changeActorComplete()
{
    moveBasket();      //移动篮子
}

void Round006_04::moveBasket()
{
    m_pBasketBack->runAction(MoveBy::create(MOVE_TIME, MOVE_POS));
    m_PBasketFront->runAction(MoveBy::create(MOVE_TIME, MOVE_POS));
    for (unsigned int i = 0; i < 3; i++) {
        m_pWindmill[i]->runAction(MoveBy::create(MOVE_TIME, MOVE_POS));
    }
    
    for (unsigned int i = 0; i < 3; i++) {
        m_pSticks[i]->runAction(MoveBy::create(MOVE_TIME, MOVE_POS));
    }
    m_pStageScene->moveLeadingActor(MOVE_TIME,MOVE_POS);
    
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round006_04::fadeOutLayer), BACK_SCENE_TIME-1.5f);
}

/*  回合结束回调 */
void Round006_04::endRound()
{
}

//渐变变黑退出
void Round006_04::fadeOutLayer(float)
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
void Round006_04::shareCallback()
{
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setOKCb(std::bind(&Round006_04::roundEndShareCallBack, this,false));
    m_pStageScene->getRoundUINode()->getSocialShareNode()->setCloseCb(std::bind(&Round006_04::roundEndShareCallBack, this,true));
}
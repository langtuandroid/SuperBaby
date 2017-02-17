//
//  Round011_02.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round011_02.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "AppConfig.h"
#include "GafAssetCache.h"
#include "CustomMessage.h"

#define CELEBRATE_DELAY                 8.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage011/SL-GK11-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage011/SL-GK11-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define STANDBY                        "standby"                                // 待机动画
#define TRIGGER                        "trigger"                                // 触发动画
#define BRIDGE_PNG_FILE                "res/stage011/SL-GK11-2-IT.png"           //木桥png
#define PUZZLE_PIECE_ZORDER            2                                         //拼图块z值
#define PUZZLE_BOARD_ZORDER            1                                         //拼图板z值
#define BRIDGE_DOWN_TIME               2.0f                                      //木桥下降时间
#define BRIDGE_FIRST_DOWN_OFFSETY      -1100                                      //木桥第一次下降高度
#define BRIDGE_SECOND_DOWN_OFFSETY     -1050                                      //木桥第二次下降高度
#define PUZZLE_BOARD_POS               Vec2(1142,1901)                             //拼图板的坐标
#define FULL_PUZZLE_BOARD_POS          Vec2(1142,801)                              //完整拼图板的坐标
#define HINT_PUZZLE_BOARD_POS          Vec2(1168,773)                              //提示拼图板的坐标
#define BRIDGE_POS                     Vec2(1310,3300)                            //木桥的坐标
#define BRIDGE_ZORDER                  1                                          //木桥z值
#define PUZZLE_PIECE_UP_TIME           0.5f                                      //拼图块上升时间
#define PUZZLE_PIECE_UP_OFFSETY        450                                       //拼图块上升高度
#define PUZZLE_PIECE_DOWN_TIME         0.5f                                      //拼图块下降时间
#define PUZZLE_PIECE_DOWN_OFFSETY      -450                                      //拼图块下降高度
#define MOV_TIME                       0.2f                                    //答错回原位置移动时间
#define ACTOR_MOV_TIME                 2.5f                                    //主角移动时间
#define FadeOut_TIME                   2                                       //拼图板淡出时间
#define WATERFALL_GAF_FILE             "res/stage011/SL-GK11-3-pubu/SL-GK11-3-pubu.gaf"
#define WATERFALL_POS                  Vec2(719,372)

/*
 *  constructor
 */
Round011_02::Round011_02() {}

/*
 *  destructor
 */
Round011_02::~Round011_02()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    GafAssetCache::getInstance()->removeGafAssetForKey(WATERFALL_GAF_FILE);
}

//
Round011_02* Round011_02::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round011_02();
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
bool Round011_02::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    m_pWaterfallActor = Actor::createWithGafFile(WATERFALL_GAF_FILE);
    m_pWaterfallActor->setScaleX(0.8f);
    m_pWaterfallActor->setPosition(WATERFALL_POS);
    this->addChild(m_pWaterfallActor);
    
    appendAsyncLoadTexture(BRIDGE_PNG_FILE);
    appendRemovedTexture(BRIDGE_PNG_FILE);
    preLoadRes();
    return true;
}

//
void Round011_02::onPreLoadedRes()
{
    ///////
    RoundBase::onPreLoadedRes();
    
    // 纹理加载
    m_pSBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pSBN, BN_ZORDER);
    
    // 加载木桥和拼图板
    m_pBridge = Sprite::create(BRIDGE_PNG_FILE);
    m_pBridge->setPosition(BRIDGE_POS);
    addChild(m_pBridge, BRIDGE_ZORDER);
    
    char ptBuffer[64];
    for (unsigned int i = 0; i < 3; i++) {
        sprintf(ptBuffer, "SL-GK11-2-TM%02d.png",i+1);
        m_pPuzzleBoard[i] = Sprite::createWithSpriteFrameName(ptBuffer);
        m_pSBN->addChild(m_pPuzzleBoard[i], PUZZLE_BOARD_ZORDER);
        if (i == 1) {
            m_pPuzzleBoard[i]->setPosition(FULL_PUZZLE_BOARD_POS);
            m_pPuzzleBoard[i]->setVisible(false);
        }else if(i == 2)
        {
            m_pPuzzleBoard[i]->setPosition(HINT_PUZZLE_BOARD_POS);
            m_pPuzzleBoard[i]->setVisible(false);
        }else
        {
            m_pPuzzleBoard[i]->setPosition(PUZZLE_BOARD_POS);
        }
    }
    
    //加载拼图块
    float ptX = 825; float offSetX = 284; float ptY = -150;
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(ptBuffer, "SL-GK11-2-DA%02ld.png",i+1);
        m_pPuzzles[i] = Sprite::createWithSpriteFrameName(ptBuffer);
        auto index = getRandomNumber(m_pCreatedIndexs, 0, 2);
        m_pPuzzles[i]->setPosition(Vec2(ptX+offSetX*index, ptY));
        m_originalPoints[i] = Vec2(ptX+offSetX*index, ptY+450);
        m_pPuzzles[i]->setUserData((void*)i);
        m_pSBN->addChild(m_pPuzzles[i], PUZZLE_PIECE_ZORDER);
    }
    m_pCreatedIndexs.clear();    // 清理
    
}

int Round011_02::randomIndex(int min,int max)
{
    srand(unsigned(time(0)));
    return random(min, max);
}

bool Round011_02::numberIsRepeat(std::vector<int>& indexs,int number)
{
    while (true)
    {
        int cursize = 0;
        if (indexs.size() != 0) {
            for (auto &it : indexs)
            {
                cursize++;
                if (it == number) {
                    return false;    //表示重复
                } else {
                    if (cursize == indexs.size()) {
                        return true;  //表示不重复
                    }
                }
            } /*for*/
        } else {
            return true;          //表示为随机的第一个数，没有重复
        }
    } /*while*/
}

int Round011_02::getRandomNumber(std::vector<int>& indexs,int min, int max)
{
    while (true) {
        auto number = randomIndex(min,max);  //获得随机数
        auto isrepeat = numberIsRepeat(indexs,number);  //判断是否重复
        
        if (isrepeat) {
            indexs.push_back(number);
            return number;
        }
    }
    
}

//设置拼图板红色提示块循环显隐
void Round011_02::redHintPuzzleVisible(float dt)
{
    if (m_bIsVisible) {
        m_pPuzzleBoard[2]->setVisible(false);
        m_bIsVisible = false;
    }else
    {
        m_pPuzzleBoard[2]->setVisible(true);
        m_bIsVisible = true;
    }
}

//开始回合
void Round011_02::onEnterRound()
{
    log("this posX === %f",this->getPosition().x);
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
    downBridgeAndPuzzleBoard();        //下降木桥和拼图板
}

//回合开始，下降木桥和拼图板
void Round011_02::downBridgeAndPuzzleBoard()
{
    m_pPuzzleBoard[0]->runAction(MoveBy::create(BRIDGE_DOWN_TIME, Vec2(0.0f, BRIDGE_FIRST_DOWN_OFFSETY)));
    m_pBridge->runAction(Sequence::create(MoveBy::create(BRIDGE_DOWN_TIME, Vec2(0.0f, BRIDGE_FIRST_DOWN_OFFSETY)),CallFunc::create([this]{
        m_pPuzzleBoard[2]->setVisible(true);
        this->schedule(CC_SCHEDULE_SELECTOR(Round011_02::redHintPuzzleVisible), 0.3f, 3, 0.3f);   //设置红色提示块显隐
        this->schedule(CC_SCHEDULE_SELECTOR(Round011_02::upPuzzlePieceSchedule), PUZZLE_PIECE_UP_TIME, -1, 0);
    }), NULL));
    
}

void Round011_02::upPuzzlePieceSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_02::upPuzzlePieceSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pPuzzles[m_InitAniIndex++]->runAction(EaseBackOut::create(MoveBy::create(PUZZLE_PIECE_UP_TIME, Vec2(0.0f, PUZZLE_PIECE_UP_OFFSETY))));
}

//答对后延时拼图块下降
void Round011_02::delayDownPuzzlePiece(float)
{
    m_InitAniIndex = 1;   //重置为1
    this->schedule(CC_SCHEDULE_SELECTOR(Round011_02::downPuzzlePieceSchedule), PUZZLE_PIECE_DOWN_TIME, -1, 0);
}

//答对后拼图块下降
void Round011_02::downPuzzlePieceSchedule(float)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_02::downPuzzlePieceSchedule));
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round011_02::fadeOutPuzzleBoard), 1.0f);
        return;
    }
    m_pPuzzles[m_InitAniIndex]->runAction(Sequence::create(MoveBy::create(PUZZLE_PIECE_DOWN_TIME-0.2f, Vec2(0.0f, PUZZLE_PIECE_DOWN_OFFSETY)),CallFunc::create([this]{
        m_pPuzzles[m_InitAniIndex]->setVisible(false);
        m_InitAniIndex++;
    }), NULL));
}

//答对后拼图板渐变消失
void Round011_02::fadeOutPuzzleBoard(float)
{
    m_pPuzzleBoard[1]->runAction(Sequence::create(FadeOut::create(FadeOut_TIME),CallFunc::create([this]{
         downSecondBridge();                   //木桥二次下降
    }), NULL));
}

//答对后木桥下降
void Round011_02::downSecondBridge()
{
    m_pBridge->runAction(Sequence::create(MoveBy::create(BRIDGE_DOWN_TIME, Vec2(0.0f, BRIDGE_SECOND_DOWN_OFFSETY)),CallFunc::create([this]{
        actorMoveToBridge();           //主角行走到桥
    }), NULL));
}

//庆祝
void Round011_02::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(1);
}

// 主角行走到桥
void Round011_02::actorMoveToBridge()
{
    m_pStageScene->moveLeadingActorCB(ACTOR_MOV_TIME, Vec2(800.0f, 0), [this] {
        if (m_completeCB) {
            m_completeCB();
        }}, true);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto bgVScrolledCB = [this, dispatcher] (EventCustom* pEv)
    {
        // 滚动速度
        float bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
        auto rsp = m_pStageScene->getStageData()->getRoundScroll(m_pStageScene->getCurrRoundIndex());
        auto moveDuration = rsp.m_vscroll / bgScrollSpeed;
        m_pWaterfallActor->runAction(MoveBy::create(moveDuration, Vec2(0.0f, rsp.m_vscroll)));
        dispatcher->removeCustomEventListeners(BG_VSCROLL_BEGIN);
    };
    dispatcher->addCustomEventListener(BG_VSCROLL_BEGIN, bgVScrolledCB);
}

/* 开启触摸 */
void Round011_02::openTouch()
{
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
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            target->setScale(1.1f);
            m_pSBN->reorderChild(target, PUZZLE_PIECE_ZORDER + 1);
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
        m_pSBN->reorderChild(target, PUZZLE_PIECE_ZORDER);
        
        unsigned long clicknumber = (unsigned long)target->getUserData();
        auto touchPoint = this->convertTouchToNodeSpace(touch);
        if (m_rect.containsPoint(touchPoint))
        {
            if (clicknumber == 0) {
                target->setVisible(false);                      //隐藏拖动拼图
                m_pPuzzleBoard[0]->setVisible(false);           //隐藏不完整的拼图板
                m_pPuzzleBoard[1]->setVisible(true);            //显示完成的拼图板
                this->scheduleOnce(CC_SCHEDULE_SELECTOR(Round011_02::delayDownPuzzlePiece), 1.0f);
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
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pPuzzles[i]);
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
            if (m_anwerErrorCB) {   //答错
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
        m_pSBN->reorderChild(target, PUZZLE_PIECE_ZORDER);
    };
    
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pPuzzles[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pPuzzles[i]);
    }
}

/*  回合结束回调 */
void Round011_02::endRound() {}

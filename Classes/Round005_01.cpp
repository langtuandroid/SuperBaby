//
//  Round005_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/1.
//
//

#include "Round005_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define FLOWER_ZOEDER                    2                                       //花的z值
#define YELLOW_LIGHT_GAF_FILE            "res/stage005/SL-GK05-1-IT01/SL-GK05-1-IT01.gaf"        //黄色光gaf
#define YELLOW_LIGHT_ZORDER              1                                       //黄色光z值
#define NPC_GAF_POS                    Vec2(1400.0f,300.0f)                       //npc初始位置
#define NPC_GAF_ZORDER                  3                                        //npc的z值
#define BN_PLIST_FILE                   "res/stage005/SL-GK05-IT.plist"             // BN的plist 文件
#define BN_PNG_FILE                     "res/stage005/SL-GK05-IT.png"               // BN的 png 文件
/*
 *  constructor
 */
Round005_01::Round005_01()
:m_chooseTrueNum(0)
{}

/*
 *  destructor
 */
Round005_01::~Round005_01() {
    cleanUp();
}

/*
 *  清理
 */
void Round005_01::cleanUp()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    char fnBuffer[64];
    for (unsigned int i = 0; i < 5; i++) {
        sprintf(fnBuffer, "res/stage005/SL-GK05-1-TM%02d-D/SL-GK05-1-TM%02d-D.gaf",i+1,i+1);
        GafAssetCache::getInstance()->removeGafAssetForKey(fnBuffer);
    }
    GafAssetCache::getInstance()->removeGafAssetForKey(YELLOW_LIGHT_GAF_FILE);
}

//
Round005_01* Round005_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round005_01();
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
bool Round005_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);

    // 设置触摸响应区域
    m_flowerChooseRect[0] = Rect(650, 520, 240, 260);
    m_flowerChooseRect[1] = Rect(900, 700, 230, 200);
    m_flowerChooseRect[2] = Rect(1180, 520, 230, 260);
    m_flowerChooseRect[3] = Rect(1420, 690, 230, 210);
    m_flowerChooseRect[4] = Rect(1770, 600, 130, 180);
    
    //设置黄光gaf位置
    m_yellowLightPos[0] = Vec2(600, 940);
    m_yellowLightPos[1] = Vec2(1110, 940);
    
    //加载花儿actor
    char fnBuffer[64];
    float fX = 500.0f; float offSetX = 260.0f; float fY = 800.0f;
    for (unsigned int i = 0; i < 5; i++) {
        sprintf(fnBuffer, "res/stage005/SL-GK05-1-TM%02d-D/SL-GK05-1-TM%02d-D.gaf",i+1,i+1);
        m_pFlowers[i] = Actor::createWithGafFile(fnBuffer);
        addChild(m_pFlowers[i], FLOWER_ZOEDER);
        
        if (i == 1 || i == 3) {
            m_pFlowers[i]->setPosition(Vec2(fX + offSetX*i, fY+200));
        }else
        {
            m_pFlowers[i]->setPosition(Vec2(fX + offSetX*i, fY));
        }
    }
    
    //加载黄色光gaf
    for (unsigned int i = 0; i < 2; i++) {
        m_pYellowLight[i] = Actor::createWithGafFile(YELLOW_LIGHT_GAF_FILE);
        m_pYellowLight[i]->setPosition(m_yellowLightPos[i]);
        addChild(m_pYellowLight[i], YELLOW_LIGHT_ZORDER);
        m_pYellowLight[i]->setVisible(false);
    }
    
    if (m_pStageScene != nullptr)
    {
        m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
        m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    }
    return true;
}

/* 开启触摸 */
void Round005_01::openTouch()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->onTouchBegan = [this](Touch* touch, Event* event)->bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        for (unsigned int i = 0; i < 5; i++) {
            if (m_flowerChooseRect[i].containsPoint(touchPoint)) {
                
                return true;
            }
        }
        
        return false;
    };
    listenerRight->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        for (unsigned int i = 0; i < 5; i++) {
            if (m_flowerChooseRect[i].containsPoint(touchPoint)) {
                if (i == 0 || i == 2) { //答对问题
                    if (m_choosedNum == i) {
                        return;
                    }
                    m_choosedNum = i;
                    m_pStageScene->playAnswerCorrectEffect(touchPoint);     //播放答对特效
                    // 播放答对音效
                    playAnswerCorrectEffect();
                    if (getEffIndex() != -1) {
                        MyAudioEngine::getInstance()->stopAudio(getEffIndex());
                    }
                    // 答对时主角说话
                    playLeadAnsCorEff();
                    
                    m_chooseTrueNum++;
                    if (m_chooseTrueNum < 2) {
                        if (m_celebrateCB) {
                            m_celebrateCB(1);
                        }
                    }else // 2个全部答对
                    {
                        m_pStageScene->getRoundUINode()->stopReadEffect();          //停止题目音效
                        m_pStageScene->getRoundUINode()->endReadTopic();            //停止读题目改变动作
                        this->celebrate(CELEBRATE_DELAY);
                        auto dispatcher = Director::getInstance()->getEventDispatcher();
                        dispatcher->removeEventListenersForTarget(this);
                    }
                    if (i == 2) {
                        m_pYellowLight[i-1]->setVisible(true);
                    }else
                    {
                        m_pYellowLight[i]->setVisible(true);
                    }
                    break;
                }else //答错
                {
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
                }
                break;
            }
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, this);   //注册绑定监听器和对象
}

//开始回合
void Round005_01::onEnterRound()
{
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
}

//庆祝
void Round005_01::celebrate(const float celebrateDelay) {
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
    
}

/*  回合结束回调 */
void Round005_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}

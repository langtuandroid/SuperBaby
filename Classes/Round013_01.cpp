//
//  Round013_01.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//

#include "Round013_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

// 飞机 gaf
#define PLANE1_GAF                      "res/stage013/XQ-GK01-1-DA01-D/XQ-GK01-1-DA01-D.gaf"
#define PLANE2_GAF                      "res/stage013/XQ-GK01-1-DA02-D/XQ-GK01-1-DA02-D.gaf"
#define PLANE3_GAF                      "res/stage013/XQ-GK01-1-DA03-D/XQ-GK01-1-DA03-D.gaf"

// 飞机基地
#define BASE1_GAF       "res/stage013/XQ-GK01-1-MS01-cangmeng1-D+C/XQ-GK01-1-MS01-cangmeng1-D+C.gaf"
#define BASE2_GAF       "res/stage013/XQ-GK01-1-MS01-cangmeng2-D+C/XQ-GK01-1-MS01-cangmeng2-D+C.gaf"
#define BASE3_GAF       "res/stage013/XQ-GK01-1-MS01-cangmeng3-D+C/XQ-GK01-1-MS01-cangmeng3-D+C.gaf"
#define BASE1_CONFIG    "res/stage013/XQ-GK01-1-MS01-cangmeng1-D+C.json"
#define BASE2_CONFIG    "res/stage013/XQ-GK01-1-MS01-cangmeng2-D+C.json"
#define BASE3_CONFIG    "res/stage013/XQ-GK01-1-MS01-cangmeng3-D+C.json"

#define AISLE_PNG1            "res/stage013/XQ-GK01-1-QJ2.png"
#define AISLE_PNG2            "res/stage013/XQ-GK01-1-QJ1.png"

// 庆祝延时
#define CELEBRATE_DELAY                 5.0f
#define FadeOut_TIME                    2                                       // 数字淡出时间
#define MOV_TIME                        0.2f                                    // 答错回原位置移动时间

#define CYLINDER_TAG    500
#define AISLE_TAG       501

#define PASS_GAF_FILE                   "res/common/yanshi01/yanshi01.gaf"      //过关引导gaf
#define PASS_GAF_ZORDER                 3
#define BACKGROUND_MUSIC_FILE           "res/selStage/02/audio/SQ-BGM.mp3"

/*
 *  constructor
 */
Round013_01::Round013_01() {}

/*
 *  destructor
 */
Round013_01::~Round013_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    m_pStageScene->removeOBLeaderNode(CYLINDER_TAG);
    
    GafAssetCache* pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(PLANE1_GAF);
    pGafAssetCache->removeGafAssetForKey(PLANE2_GAF);
    pGafAssetCache->removeGafAssetForKey(PLANE3_GAF);
    pGafAssetCache->removeGafAssetForKey(BASE1_GAF);
    pGafAssetCache->removeGafAssetForKey(BASE2_GAF);
    pGafAssetCache->removeGafAssetForKey(BASE3_GAF);
//    pGafAssetCache->removeGafAssetForKey(CYLINDER_GAF);
    
    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->removeTextureForKey(AISLE_PNG1);
}

//
Round013_01* Round013_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round013_01();
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
bool Round013_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_roundIndex = roundIndex;
    m_pStageScene = pStageScene;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    // 试玩时播放背景音乐
    if (m_pStageScene->isTrial())
    {
        MyAudioEngine::getInstance()->playAudio(BACKGROUND_MUSIC_FILE,true,true,0.4f);
    }

    // 加载飞机
    float posx = 880.0f, posy = 760.0f, offsetX = 260.0f;
    m_planeActors[1] = Actor::createWithGafFile(PLANE1_GAF, true, 1.0f, Vec2(-130.0f, 434.0f));
    m_planeActors[1]->setUserData((void*)1);
    m_planeActors[2] = Actor::createWithGafFile(PLANE2_GAF, true, 1.0f, Vec2(-130.0f, 434.0f));
    m_planeActors[2]->setUserData((void*)2);
    m_planeActors[0] = Actor::createWithGafFile(PLANE3_GAF, true, 1.0f, Vec2(-130.0f, 434.0f));
    m_planeActors[0]->setUserData((void*)0);
    for (long i = 0; i < 3; i++)
    {
        m_planeActors[i]->setSize(Size(224.0f, 360.0f));
        addChild(m_planeActors[i], 4);
    } /*for*/
    
    // 随机设定位置
    unsigned int all = 0;
    bool selected[3] = {false, false, false};
    srand(unsigned(time(0)));
    while (all < 3) {
        unsigned int randomIndex = static_cast<unsigned int>(rand() % 3);
        if (selected[randomIndex]) continue;
        selected[randomIndex] = true;
        m_planeActors[all]->setPosition(Vec2(posx + offsetX * randomIndex, posy));
        m_originalPoints[all] = m_planeActors[all]->getPosition();
        all++;
    } /*while*/
    
    // 加载基地
    posx = 520.0f, posy = 260.0f, offsetX = 530.0f;
    m_baseActors[0] = Actor::createWithGafConfigFile(BASE1_CONFIG);
    m_baseActors[1] = Actor::createWithGafConfigFile(BASE2_CONFIG);
    m_baseActors[2] = Actor::createWithGafConfigFile(BASE3_CONFIG);
    for (long i = 0; i < 3; i++)
    {
        m_baseActors[i]->playWithSequenceAliasName("standby", true);
        m_baseActors[i]->setPosition(Vec2(posx + offsetX * i, posy));
        addChild(m_baseActors[i], 3);
    } /*for*/
    
    // 过道
    auto sp1 = Sprite::create(AISLE_PNG1);
    sp1->setPosition(Vec2(436.0f, 180.0f));
    appendRemovedTexture(AISLE_PNG1);
    addChild(sp1, 1);
    auto sp2 = Sprite::create(AISLE_PNG2);
    m_pStageScene->addOverLeaderNode(sp2, Vec2(2900.0f, 180.0f), AISLE_TAG);
    
    return true;
}

// 进入本回合
void Round013_01::onEnterRound()
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
    
    if (!UserDefault::getInstance()->getBoolForKey("delaytimetwo", false)) {
        UserDefault::getInstance()->setBoolForKey("delaytimetwo", true);
        scheduleOnce([this](float){
            playPassActor();
        }, 7.0f, "delaytime");
    }
}

/* 播放过关引导 */
void Round013_01::playPassActor()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    m_pStageScene->getRoundUINode()->addChild(colorLayer,PASS_GAF_ZORDER);
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
    Actor* passActor = Actor::createWithGafFile(PASS_GAF_FILE, false);
    passActor->setPosAtNode(Vec2(300, 500));
    passActor->setPosition(Vec2(570, 300));
    m_pStageScene->addChild(passActor, PASS_GAF_ZORDER);
    passActor->setAnimationFinishedPlayDelegate([this,passActor,colorLayer]
    {
        colorLayer->removeFromParent();
        passActor->setVisible(false);
    });
}

/* 开启触摸 */
void Round013_01::openTouch()
{
    ///////////////////
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getSize().width, target->getSize().height);
        if (rect.containsPoint(touchPoint))
        {
            m_touchBeganPoint = touch->getLocation();
            this->reorderChild(target, 5);
            target->setScale(1.2f);
            return true;
        }
        return false;
    };
    listenerDrag->onTouchMoved = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        Vec2 touchMovePoint = touch->getLocation();
        Vec2 movePos;
        
        long index = (long)target->getUserData();
        movePos.x = m_originalPoints[index].x + (touchMovePoint.x - m_touchBeganPoint.x);
        movePos.y = m_originalPoints[index].y + (touchMovePoint.y - m_touchBeganPoint.y);
        target->setPosition(movePos);
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto pSource = static_cast<Actor*>(event->getCurrentTarget());
        long index = (long)pSource->getUserData();
        this->reorderChild(pSource, 4);
        
        bool isCorrect = false;
        for (unsigned long i = 0; i < 3; i++)
        {
            auto pTarget = m_baseActors[i];
            auto touchPoint = pTarget->convertTouchToNodeSpace(touch);
            Rect rect = Rect(0.0f, 0.0f, pTarget->getSize().width, pTarget->getSize().height);
            if (rect.containsPoint(touchPoint))
            {
                if (i == index) /*答对*/ {
                    isCorrect = true;
                    m_numberOfdragComplete++;
                    break;
                }
            }
        } /*for*/
        
        if (isCorrect)
        {
            // 播放答对音效
            playAnswerCorrectEffect();
            playLeadAnsCorEff();
            // 播放答对特效
            m_pStageScene->playAnswerCorrectEffect(Vec2(touch->getLocation().x,
                                                        touch->getLocation().y));
            m_baseActors[index]->playWithSequenceAliasName("trigger", false);
            pSource->removeFromParent();
            pSource = nullptr;
            m_planeActors[index] = nullptr;
            
            if (m_numberOfdragComplete == 3) /*全部答对*/ {
                m_pStageScene->getRoundUINode()->stopReadEffect();          // 停止题目音效
                m_pStageScene->getRoundUINode()->endReadTopic();            // 停止读题目改变动作
                celebrate(CELEBRATE_DELAY);
            } else {
                if (m_celebrateCB) {
                    m_celebrateCB(1);
                }
            }
        } else {
            // 播放答错音效
            playAnswerErrorEffect();
            playLeadAnsErrEff();
            if (m_anwerErrorCB)
            {
                m_anwerErrorCB();
            }
            pSource->setScale(1.0f);
            pSource->runAction(Sequence::create(EaseBackOut::create(MoveTo::create(MOV_TIME, m_originalPoints[index])),
                                                CallFunc::create([this, pSource, index] {
                pSource->setPosition(m_originalPoints[index]);
            }), NULL));
        }
    };
    listenerDrag->onTouchCancelled = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Actor*>(event->getCurrentTarget());
        long index = (long)target->getUserData();
        this->reorderChild(target, 4);
        target->setScale(1.0f);
        target->setPosition(m_originalPoints[index]);
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_planeActors[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag->clone(), m_planeActors[i]);
    } /*for*/
}

/*
 *  庆祝 
 */
void Round013_01::celebrate(const float celebrateDelay /*庆祝延时*/ )
{
    if (m_celebrateCB) {
        m_celebrateCB(1);
    }
    m_pStageScene->getRoundUINode()->playPassActor(0);
}

/*  回合结束回调 */
void Round013_01::endRound()
{
    if (m_completeCB) {
        m_completeCB();
    }
}


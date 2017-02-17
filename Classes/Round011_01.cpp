//
//  Round011_01.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/9.
//
//

#include "Round011_01.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "RoundUINode.h"
#include "GafAssetCache.h"

#define CELEBRATE_DELAY                  5.0f                                    // 庆祝延时
#define BN_PLIST_FILE                  "res/stage011/SL-GK11-IT.plist"           //BN的plist文件
#define BN_PNG_FILE                    "res/stage011/SL-GK11-IT.png"             //BN的png文件
#define BN_ZORDER                      2                                         //BN的z值
#define LEFT_MOVE_TIME                1.0f                                     //草莓左移时间
#define LEFT_MOVE_OFFSETX             -1500                                     //草莓左移宽度
#define RIGHT_MOVE_TIME               1.0f                                     //草莓右移时间
#define RIGHT_MOVE_OFFSETX            1500                                     //草莓右移宽度
#define STRAWBERRY_ZORDER             1                                        //草莓z值
// 熊 gaf
#define BEAR_GAF_FILE                   "res/stage011/SL-GK11-1-MS01-D+C/SL-GK11-1-MS01-D+C.gaf"
#define BEAR_GAF_CONFIG_FILE            "config/stage011/bear.json"             // 熊gaf的配置文件
#define BEAR_GAF_POS                  Vec2(134,1422)                           //熊gaf的位置
#define BEAR_GAF_ZORDER               1                                        //熊gaf的z值
#define STRAWBERRY_MOVE_TIME          0.5f                                     //答对后草莓移动时间
#define STRAWBERRY_MOVE_POS           Vec2(1466,553)                           //答对后草莓移动绝对坐标
#define TREE_PNG_FILE                 "res/stage011/SL-GK11-1-IT01.png"        //树的png
#define TREE_TAG                      1000                                     //树的tag
#define WATERFALL_GAF_FILE             "res/stage011/SL-GK11-3-pubu/SL-GK11-3-pubu.gaf"

/*
 *  constructor
 */
Round011_01::Round011_01():m_InitAniIndex(0) {}

/*
 *  destructor
 */
Round011_01::~Round011_01()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners("openTouch");
    dispatcher->removeCustomEventListeners("endRound");
    
    m_pStageScene->removeOBLeaderNode(TREE_TAG);

    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->removeTextureForKey(TREE_PNG_FILE);
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(BEAR_GAF_FILE);
}

//
Round011_01* Round011_01::createWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    auto pRound = new Round011_01();
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
bool Round011_01::initWithRoundIndex(const int roundIndex, StageScene* pStageScene, const Vec2& pos)
{
    m_pStageScene = pStageScene;
    m_roundIndex = roundIndex;
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(pos);
    
    // 加载树
    m_pTree = Sprite::create(TREE_PNG_FILE);
    m_pTree->setAnchorPoint(Vec2(0.0f, 0.0f));
    m_pStageScene->addOverLeaderNode(m_pTree, Vec2(0.0f, 0.0f), TREE_TAG);
    appendRemovedTexture(TREE_PNG_FILE);
    
    // 加载熊gaf
    m_pBearActor = Actor::createWithGafConfigFile(BEAR_GAF_CONFIG_FILE);
    m_pBearActor->setPosition(BEAR_GAF_POS);
    m_pBearActor->playWithSequenceAliasName("standby", true);
    addChild(m_pBearActor, BEAR_GAF_ZORDER);
    
    // 大图
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile(BN_PLIST_FILE);
    m_pBN = SpriteBatchNode::create(BN_PNG_FILE);
    addChild(m_pBN, BN_ZORDER);
    
    // 加载草莓
    char cmBuffer[64];
    float cmX = 2300; float offSetX = 433; float cmY = 303;
    for (unsigned long i = 0; i < 3; i++) {
        sprintf(cmBuffer, "SL-GK11-1-DA%02ld.png", i + 1);
        m_pStrawberrys[i] = Sprite::createWithSpriteFrameName(cmBuffer);
        auto index = getRandomNumber(m_pCreatedIndexs, 0, 2);
        m_pStrawberrys[i]->setPosition(Vec2(cmX+offSetX*index, cmY));
        m_pStrawberrys[i]->setUserData((void*)i);
        m_pBN->addChild(m_pStrawberrys[i], STRAWBERRY_ZORDER);
    }
    m_pCreatedIndexs.clear();
    
    m_pStageScene->appendRemovedSpriteFrameFile(BN_PLIST_FILE);
    m_pStageScene->appendRemovedTexture(BN_PNG_FILE);
    
//    m_pWaterfallActor = Actor::createWithGafFile(WATERFALL_GAF_FILE);
//    m_pWaterfallActor->setScaleX(0.8f);
//    m_pStageScene->addBelowLeaderNode(m_pWaterfallActor, WATERFALL_POS, WATERFALL_TAG);
    
    
    
    return true;
}

//
int Round011_01::randomIndex(int min,int max)
{
    srand(unsigned(time(0)));
    return random(min, max);
}

//
bool Round011_01::numberIsRepeat(std::vector<int>& indexs,int number)
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

//
int Round011_01::getRandomNumber(std::vector<int>& indexs,int min, int max)
{
    while (true)
    {
        auto number = randomIndex(min,max);  //获得随机数
        auto isrepeat = numberIsRepeat(indexs,number);  //判断是否重复
        
        if (isrepeat) {
            indexs.push_back(number);
            return number;
        }
    } /*while*/
}

/*
 *  进入回合
 */
void Round011_01::onEnterRound()
{
    GafAssetCache::getInstance()->addGafAsset(WATERFALL_GAF_FILE);
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
    // 左移出现草莓
    schedule(CC_SCHEDULE_SELECTOR(Round011_01::strawberrysLeftMoveSchedule), LEFT_MOVE_TIME, -1, 0.0f);
}

// 回合开始，草莓左移出现
void Round011_01::strawberrysLeftMoveSchedule(float dt)
{
    if (m_InitAniIndex >= 3) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_01::strawberrysLeftMoveSchedule));
        m_jumped = true;
        if (m_opentouched) {
            openTouch();
        }
        return;
    }
    m_pStrawberrys[m_InitAniIndex++]->runAction(MoveBy::create(LEFT_MOVE_TIME, Vec2(LEFT_MOVE_OFFSETX, 0.0f)));
}

//选对后。设置草莓动作
void Round011_01::changeStrawberry(Node* sender)
{
    sender->runAction(Spawn::create(MoveTo::create(STRAWBERRY_MOVE_TIME, STRAWBERRY_MOVE_POS),
                                    ScaleTo::create(STRAWBERRY_MOVE_TIME, 0.6f), NULL));
}

// 选对后，改变熊actor的动作
void Round011_01::changeBearActor()
{
    m_pBearActor->playWithSequenceAliasName("trigger");
}

// 庆祝
void Round011_01::celebrate(const float celebrateDelay)
{
    if (m_celebrateCB) {
        m_celebrateCB(2);
    }
    
    // 播放过关庆祝动画
    m_pStageScene->getRoundUINode()->playPassActor(0);
    m_InitAniIndex = 1; //重置为1
}

/* 
 *  开启触摸
 */
void Round011_01::openTouch()
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
            if (clicknumber == 2) {
                changeBearActor();                 //改变熊gaf动作
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
                changeStrawberry(target);           //改变草莓状态
                for (unsigned int i = 0; i < 3; i++) {   //移除监听
                    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pStrawberrys[i]);
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
            }
        }
    };
    // 将本回合节点绑定到事件监听器
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, m_pStrawberrys[0]);
    for (unsigned int i = 1; i < 3; i++) {
        dispatcher->addEventListenerWithSceneGraphPriority(listenerRight->clone(), m_pStrawberrys[i]);
    }
}

//回合结束，草莓右移出现
void Round011_01::strawberrysRightMoveSchedule(float dt)
{
    if (m_InitAniIndex <= -1) {
        unschedule(CC_SCHEDULE_SELECTOR(Round011_01::strawberrysRightMoveSchedule));
        if (m_completeCB) {
            m_completeCB();
        }
        return;
    }
    m_pStrawberrys[m_InitAniIndex]->runAction(Sequence::create(MoveBy::create(RIGHT_MOVE_TIME-0.2f, Vec2(RIGHT_MOVE_OFFSETX, 0.0f)),CallFunc::create([this]{
        m_pStrawberrys[m_InitAniIndex]->setVisible(false);
        m_InitAniIndex--;
    }), NULL));
}

/*  
 *  回合结束回调
 */
void Round011_01::endRound()
{
    m_InitAniIndex = 1;
    schedule(CC_SCHEDULE_SELECTOR(Round011_01::strawberrysRightMoveSchedule), RIGHT_MOVE_TIME, -1, 0);
}

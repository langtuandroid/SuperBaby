//
//  StageScene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/2/28.
//
//

#include "StageScene.h"
#include "RoundBase.h"
#include "StageData.h"
#include "AppConfig.h"
#include "Player.h"
#include "Actor.h"
#include "LeadingActor.h"
#include "GAFPlayer/GAF.h"
#include "RoundUINode.h"
#include "StageDataManager.h"
#include "MyAudioEngine.h"
#include "SelStageScene.h"
#include "MobClickCpp.h"
#include "CustomMessage.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidRollBackListener.h"
#endif

#define OVER_ALL_ZORDER             3
#define BG_IMG_ZORDER               10          // 背景图片 zorder
#define BELOW_LEADER_ZORDER         35          // ZORDER 小于主角上的结点 node 的 zorder
#define LEADING_ACTOR_ZORDER        40          // 主角 zorder
#define OVER_LEADER_ZORDER          50          // 覆盖在主角上的结点 node 的 zorder
#define ANSWER_CORRECT_ZORDER       60          // 回答正确特效 zorder
#define FORE_BG_IMG_ZORDER          70          // 前景图片 zorder

#define BG_LEFT_IMAGE               "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE              "res/common/bgRight.png"

USING_NS_CC;

/*
 *  constructor
 */
StageScene::StageScene() {}

/*
 *  destructor
 */
StageScene::~StageScene()
{
    // 清除精灵帧
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    for (auto &it : m_removedSpriteFrameFiles)
    {
        spriteFrameCache->removeSpriteFramesFromFile(it);
    } /*for*/
    m_removedSpriteFrameFiles.clear();
    
    // 清除纹理资源
    auto textureCache = Director::getInstance()->getTextureCache();
    for (auto &it : m_removedTextures)
    {
        textureCache->removeTextureForKey(it);
    } /*for*/
    m_removedTextures.clear();
    
    // 释放所有背景纹理
    releaseAllBg();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (m_androidRollBackListener != nullptr) {
        delete m_androidRollBackListener;
        m_androidRollBackListener = nullptr;
    }
#endif
}

/*
 *  根据关卡数据初始化
 */
bool StageScene::initWithStageData(const StageData* pStageDatap)
{
    CCASSERT(pStageDatap != nullptr, "Invalid StageData");
    m_pAttachStageData = const_cast<StageData*>(pStageDatap);

    /* 获取场景偏移量*/
    m_sceneOffsetX = AppConfig::getInstance()->getSceneOffsetX();
    m_pContainerNode = Node::create();
    m_pContainerNode->setPosition(Vec2(m_sceneOffsetX, 0.0f));
    addChild(m_pContainerNode, 1);
    
    if (m_sceneOffsetX > 0.0f) /* 需要加载屏幕两边遮罩图片 */
    {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        
        auto bgLeftSp = Sprite::create(BG_LEFT_IMAGE);
        bgLeftSp->setAnchorPoint(Vec2(1.0f, 0.0f));
        bgLeftSp->setPosition(Vec2(m_sceneOffsetX, 0.0f));
        bgLeftSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgLeftSp->setScaleX(m_sceneOffsetX / bgLeftSp->getContentSize().width);
        addChild(bgLeftSp, 1000);
        
        auto bgRightSp = Sprite::create(BG_RIGHT_IMAGE);
        bgRightSp->setAnchorPoint(Vec2(0.0f, 0.0f));
        bgRightSp->setPosition(Vec2(m_sceneOffsetX + AppConfig::getInstance()->getDesignResolutionWidth(), 0.0f));
        bgRightSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgRightSp->setScaleX(m_sceneOffsetX / bgLeftSp->getContentSize().width);
        addChild(bgRightSp, 1000);
    }
    
    // 加载背景图
    loadBgByCurrPos();
    
    // 加载场景元素
    loadStageScene();
    
    // 告诉ui层，当前关卡数
    m_pRoundUINode->setcurLevel(pStageDatap->getStageIndex());

    return true;
}

void StageScene::loadStageScene()
{
    CCASSERT(m_pAttachStageData, "invalid attachStageData");
    
    m_pDjcfActor = Actor::createWithGafFile(m_pAttachStageData->getAnswerCorrectEffect(), false);
    m_pDjcfActor->setPosition(Vec2(-20000.0f, 0.0f));
    m_pContainerNode->addChild(m_pDjcfActor, ANSWER_CORRECT_ZORDER);
    
    // ui
    m_pRoundUINode = RoundUINode::create(1);
    m_pRoundUINode->setPosition(Vec2(m_sceneOffsetX, 0.0f));
    addChild(m_pRoundUINode, 2);
    m_pRoundUINode->attachStageScene(this);         //关联当前所属场景
    
    // 获得主角播放音效回合数
    srand((unsigned)time(0));
    m_randNumber = rand() % (m_pAttachStageData->getNumberOfRounds() - 1);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_androidRollBackListener = AndroidRollBackListener::create(4);
    m_androidRollBackListener->backScene();
    m_pRoundUINode->setAndroidRollBackListener(m_androidRollBackListener);
#endif
}

/*
 *  加载主角
 */
void StageScene::loadLeadingActor()
{
    m_pAimeeActor = Player::getInstance()->getAimeeActor();
    m_pBebeiActor = Player::getInstance()->getBebeiActor();
    
    m_pAimeeActor->setPosition(Vec2(m_pAttachStageData->getAimeePosX(), m_pAttachStageData->getAimeePosY()));
    m_pBebeiActor->setPosition(Vec2(m_pAttachStageData->getBeBeiPosX(), m_pAttachStageData->getBeBeiPosY()));
    
    m_pContainerNode->addChild(m_pAimeeActor, LEADING_ACTOR_ZORDER);
    m_pContainerNode->addChild(m_pBebeiActor, LEADING_ACTOR_ZORDER);
}

/*
 *  隐藏主角
 */
void StageScene::hideLeaderActor()
{
    m_pAimeeActor->setPosition(Vec2(0.0f, -10000.0f));
    m_pBebeiActor->setPosition(Vec2(0.0f, -10000.0f));
}

/*
 *  滚动结束
 *
 *  参数：pRoundNode
 */
void StageScene::scrollComplete()
{
    m_stageScrollCompleted = true;
    if (m_stageScrollCompleted && m_actorMovedCompleted) {
        enterRound();
    }
}

/*
 *  主角移动结束
 */
void StageScene::moveComplete()
{
    m_actorMovedCompleted = true;
    if (m_stageScrollCompleted && m_actorMovedCompleted) {
        enterRound();
    } else {
        m_pAimeeActor->wait();m_pBebeiActor->wait();
    }
}

/*
 *  进入回合
 */
void StageScene::enterRound()
{
    //回合开始，下落阅读actor
    if (!m_pAttachStageData->isEmptyRound(m_currRoundIndex)) {
        m_pRoundUINode->downReadActor(m_currRoundIndex);
    }
    
    /* 主角进入等待状态 */
    m_pAimeeActor->wait();m_pBebeiActor->wait();
    
    // 移除前一个“回合节点”
    if (m_pPreRoundNode) {
        m_pPreRoundNode->removeFromParent();
        m_pPreRoundNode = nullptr;
    }
    // 进入新回合
    m_pCurrRoundNode->enterRound();
    m_pPreRoundNode = m_pCurrRoundNode;
    m_pCurrRoundNode = nullptr;
}

/*
 *  开始一个回合
 */
void StageScene::startRound()
{
    m_actorMovedCompleted = m_stageScrollCompleted = false;
    
    // 加载新节点
    m_pCurrRoundNode = loadRound();
    
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    // 滚动速度
    float bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
    
    if (rsp.m_rso == ScrollOrient::SCROLL_HORIZONTAL) /*水平滚动*/
    {
        if (std::abs(rsp.m_scroll) > 0.0f)
        {
            auto moveDuration = std::abs(rsp.m_scroll) / bgScrollSpeed;
            // 关卡地图滚动
            auto mapScrollAction = Sequence::create(MoveBy::create(moveDuration, Vec2(rsp.m_scroll, 0.0f)),
                                                    CallFunc::create(CC_CALLBACK_0(StageScene::scrollComplete, this)), NULL);
            m_pContainerNode->runAction(mapScrollAction);
        } else {
            m_stageScrollCompleted = true;
        }
        
        // 主角行走
        auto laRsp = m_pAttachStageData->getLAMovePara(m_currRoundIndex); // 主角移动位移量
        if (std::abs(laRsp.m_scroll) > 0.0f) {
            auto moveDuration = std::abs(laRsp.m_scroll) / bgScrollSpeed;
            m_pAimeeActor->walk();m_pBebeiActor->walk();
            
            // 主角移动
            auto moveAction = Sequence::create(MoveBy::create(moveDuration, Vec2(laRsp.m_scroll, 0.0f)),
                                               CallFunc::create(CC_CALLBACK_0(StageScene::moveComplete, this)), NULL);
            m_pBebeiActor->runAction(moveAction);
            m_pAimeeActor->runAction(MoveBy::create(moveDuration, Vec2(laRsp.m_scroll, 0.0f)));
        } else {
            m_pAimeeActor->wait();m_pBebeiActor->wait();
            m_actorMovedCompleted = true;
        }
        if (m_stageScrollCompleted && m_actorMovedCompleted) {
            enterRound();
        }
    } else /*垂直滚动*/ {
        auto moveDuration = std::abs(rsp.m_vscroll) / bgScrollSpeed;
        Vec2 UpDown = Vec2(0.0f, rsp.m_vscroll);
        // 获取要升降的背景图
        auto bgSprite = m_bgSprites[rsp.m_bgImgIndex];
        // 背景图滚动结束回调
        auto bgScrollCompletedCB = CallFunc::create(std::bind(&StageScene::bgVScrolledCB, this));
        m_pAimeeActor->wait();m_pBebeiActor->wait();

        // 背景图垂直滚动
        auto scrollAction = Sequence::create(MoveBy::create(moveDuration, UpDown), bgScrollCompletedCB, NULL);
        bgSprite->runAction(scrollAction);
        
        // 通知背景开始垂直滚动
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(BG_VSCROLL_BEGIN);
    }
}

void StageScene::bgVScrolledCB()
{
    ScrollPara rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
    
    /**/
    
    if (std::abs(rsp.m_scroll) > 0.0f) {
        auto moveDuration = std::abs(rsp.m_scroll) / bgScrollSpeed;
        // 关卡地图滚动
        auto mapScrollAction = Sequence::create(MoveBy::create(moveDuration, Vec2(rsp.m_scroll, 0.0f)), CallFunc::create(CC_CALLBACK_0(StageScene::scrollComplete, this)), NULL);
        m_pContainerNode->runAction(mapScrollAction);
    } else {
        m_stageScrollCompleted = true;
    }
    // 主角行走
    auto laRsp = m_pAttachStageData->getLAMovePara(m_currRoundIndex); // 主角移动位移量
    if (std::abs(laRsp.m_scroll) > 0.0f) {
        auto moveDuration = std::abs(laRsp.m_scroll) / bgScrollSpeed;
        m_pAimeeActor->walk();m_pBebeiActor->walk();
        // 主角移动
        auto moveAction = Sequence::create(MoveBy::create(moveDuration, Vec2(laRsp.m_scroll, 0.0f)),
                                           CallFunc::create(CC_CALLBACK_0(StageScene::moveComplete, this)), NULL);
        m_pBebeiActor->runAction(moveAction);
        m_pAimeeActor->runAction(MoveBy::create(moveDuration, Vec2(laRsp.m_scroll, 0.0f)));
    } else {
        m_actorMovedCompleted = true;
    }
    if (m_stageScrollCompleted && m_actorMovedCompleted) {
        enterRound();
    }
}

void StageScene::manualScroll(float offset, const std::function<void()>& scrolledCB)
{
    auto bgLoadedCB = [this, offset, scrolledCB]
    {
        auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
        auto moveDuration = std::abs(offset) / bgScrollSpeed;
        
        auto moveCompletedCB = CallFunc::create([this, offset, scrolledCB]
                                                {
                                                    m_pAimeeActor->wait();m_pBebeiActor->wait();
                                                    m_roundNodePos.x += std::abs(offset);
                                                    releaseBgByCurrPos();
                                                    if (scrolledCB) {
                                                        scrolledCB();
                                                    }
                                                });
        // 关卡地图滚动
        auto mapScrollAction = Sequence::create(MoveBy::create(moveDuration, Vec2(-offset, 0.0f)),
                                                moveCompletedCB, NULL);
        m_pContainerNode->runAction(mapScrollAction);
        m_pAimeeActor->walk();m_pBebeiActor->walk();
        m_pAimeeActor->runAction(MoveBy::create(moveDuration, Vec2(offset, 0.0f)));
        m_pBebeiActor->runAction(MoveBy::create(moveDuration, Vec2(offset, 0.0f)));
    };
    loadBgByManualMove(offset, bgLoadedCB);
}

/* 手动滚动背景 */
void StageScene::manualBackgroundScroll(float offset, const std::function<void()>& scrolledCB)
{
    auto bgLoadedCB = [this, offset, scrolledCB]
    {
        auto bgScrollSpeed = AppConfig::getInstance()->getBgScrollSpeed();
        auto moveDuration = std::abs(offset) / bgScrollSpeed;
        
        auto moveCompletedCB = CallFunc::create([this, offset, scrolledCB]
                                                {
                                                    m_roundNodePos.x += std::abs(offset);
                                                    releaseBgByCurrPos();
                                                    
                                                    if (scrolledCB) {
                                                        scrolledCB();
                                                    }
                                                });
        
        // 关卡地图滚动
        auto mapScrollAction = Sequence::create(MoveBy::create(moveDuration, Vec2(-offset, 0.0f)),
                                                moveCompletedCB, NULL);
        m_pContainerNode->runAction(mapScrollAction);
    };
    loadBgByManualMove(offset, bgLoadedCB);
}

/*
 *  添加要移除的纹理文件
 */
void StageScene::appendRemovedTexture(const std::string& textureFile)
{
    m_removedTextures.push_back(textureFile);
}

/*
 *  添加要移除的精灵帧文件
 */
void StageScene::appendRemovedSpriteFrameFile(const std::string& spriteFrameFile)
{
    m_removedSpriteFrameFiles.push_back(spriteFrameFile);
}

/*
 *  回合胜利庆祝回调
 */
void StageScene::celebrateCB(const unsigned int celebrateTimes)
{
    if (m_pAimeeActor) {
        m_pAimeeActor->happy(celebrateTimes);
    }
    if (m_pBebeiActor) {
        m_pBebeiActor->happy(celebrateTimes);
    }
}

/*
 *  回合结束回调
 */
void StageScene::roundCompleteCB()
{
    if (m_pAttachStageData->hasRound(++m_currRoundIndex)) /*本关卡还有下一个回合*/
    {
        loadBgByCurrPosAsync();
        std::vector<ForeImgUnit> &foreBgImgArray = m_pAttachStageData->getForeImgArray();
        if (foreBgImgArray.size() > 0) {
            loadForeBgByCurrPosAsync();
        }
        scheduleUpdate();
        if (m_randNumber == m_currRoundIndex) {
            std::string effStr = AppConfig::getInstance()->getLeadWalkEff();
            MyAudioEngine::getInstance()->playAudio(effStr.c_str());
        }
    }
}

/*
 *  本调度用于判断是否达到可以切换到下一个回合
 *  有两个条件
 *      下回合的背景图加载完毕
 *      主角处于 idle 状态
 */
void StageScene::update(float dt)
{
    if (m_bgLoadCompleted && m_pAimeeActor->isWait() && m_pBebeiActor->isWait())
    {
        unscheduleUpdate();
        startRound();
    }
}

/*
 *  答题错误回调
 */
void StageScene::answerErrorCB()
{
    if (m_pAimeeActor != nullptr)
    {
        m_pAimeeActor->unhappy();
    }
    if (m_pBebeiActor != nullptr)
    {
        m_pBebeiActor->unhappy();
    }
}

/*
 *  主角移动
 */
void StageScene::moveLeadingActor(const float duration, const cocos2d::Vec2& delta, const bool isauto)
{
    CCASSERT(m_pAimeeActor && m_pBebeiActor, "invalid actor");
    if (isauto)
    {
        m_pAimeeActor->walk();
        auto callfunc = CallFunc::create([this]
                                         {
                                             m_pAimeeActor->wait();m_pBebeiActor->wait();
                                         });
        auto seq = Sequence::create(MoveBy::create(duration, delta), callfunc, NULL);
        m_pAimeeActor->runAction(seq);
        m_pBebeiActor->walk();
        m_pBebeiActor->runAction(MoveBy::create(duration, delta));
    } else {
        m_pAimeeActor->runAction(MoveBy::create(duration, delta));
        m_pBebeiActor->runAction(MoveBy::create(duration, delta));
    }
}

//
void StageScene::moveLeadingActorCB(const float duration, const cocos2d::Vec2& delta,
                                    const std::function<void()>& movedCompletedCB, const bool isauto)
{
    CCASSERT(m_pAimeeActor && m_pBebeiActor, "invalid actor");
    m_pAimeeActor->walk();
    auto callfunc = CallFunc::create([this, movedCompletedCB, isauto]
                                     {
                                         if (isauto) {
                                             m_pAimeeActor->wait();m_pBebeiActor->wait();
                                         }
                                         if (movedCompletedCB != nullptr) {
                                             movedCompletedCB();
                                         }
                                     });
    auto seq = Sequence::create(MoveBy::create(duration, delta), callfunc, NULL);
    m_pAimeeActor->runAction(seq);
    m_pBebeiActor->walk();
    m_pBebeiActor->runAction(MoveBy::create(duration, delta));
}

void StageScene::moveAimiActor(const float duration, const cocos2d::Vec2& delta, const bool isauto)
{
    CCASSERT(m_pAimeeActor, "invalid actor");
    if (isauto)
    {
        m_pAimeeActor->walk();
        auto callfunc = CallFunc::create([this]
                                         {
                                             m_pAimeeActor->wait();
                                         });
        auto seq = Sequence::create(MoveBy::create(duration, delta), callfunc, NULL);
        m_pAimeeActor->runAction(seq);
    } else {
        m_pAimeeActor->runAction(MoveBy::create(duration, delta));
    }
}
void StageScene::moveAimiActorCB(const float duration, const cocos2d::Vec2& delta,const std::function<void()>& movedCompletedCB, const bool isauto)
{
    CCASSERT(m_pAimeeActor, "invalid actor");
    m_pAimeeActor->walk();
    auto callfunc = CallFunc::create([this, movedCompletedCB, isauto]
                                     {
                                         if (isauto) {
                                             m_pAimeeActor->wait();
                                         }
                                         if (movedCompletedCB != nullptr) {
                                             movedCompletedCB();
                                         }
                                     });
    auto seq = Sequence::create(MoveBy::create(duration, delta), callfunc, NULL);
    m_pAimeeActor->runAction(seq);
}

void StageScene::moveBeibeiActor(const float duration, const cocos2d::Vec2& delta, const bool isauto)
{
    CCASSERT(m_pBebeiActor, "invalid actor");
    if (isauto)
    {
        m_pBebeiActor->walk();
        auto callfunc = CallFunc::create([this]
                                         {
                                             m_pBebeiActor->wait();
                                         });
        auto seq = Sequence::create(MoveBy::create(duration, delta), callfunc, NULL);
        m_pBebeiActor->runAction(seq);
    } else {
        m_pBebeiActor->runAction(MoveBy::create(duration, delta));
    }
}
void StageScene::moveBeibeiActorCB(const float duration, const cocos2d::Vec2& delta,const std::function<void()>& movedCompletedCB, const bool isauto)
{
    CCASSERT(m_pBebeiActor, "invalid actor");
    m_pBebeiActor->walk();
    auto callfunc = CallFunc::create([this, movedCompletedCB, isauto]
                                     {
                                         if (isauto) {
                                             m_pBebeiActor->wait();
                                         }
                                         
                                         if (movedCompletedCB != nullptr) {
                                             movedCompletedCB();
                                         }
                                     });
    auto seq = Sequence::create(MoveBy::create(duration, delta), callfunc, NULL);
    m_pBebeiActor->runAction(seq);
}

void StageScene::setVisibleAimiActor(bool visible)
{
    m_pAimeeActor->setVisible(visible);
}

void StageScene::setVisibleBeibeiActor(bool visible)
{
    m_pBebeiActor->setVisible(visible);
}

/*
 *  添加、移除 zorder 大于或小于主角的 node
 */
void StageScene::addOverLeaderNode(Node* pNode, const cocos2d::Vec2& pos, const int tag, int zorderAdd)
{
    CCASSERT(pNode != nullptr, "invalid node");
    m_pContainerNode->addChild(pNode, OVER_LEADER_ZORDER + zorderAdd, tag);
    pNode->setPosition(pos);
}

void StageScene::addOverAllNode(Node* pNode, const int tag)
{
    CCASSERT(pNode != nullptr, "invalid node");
    addChild(pNode, OVER_ALL_ZORDER, tag);
}

void StageScene::addBelowLeaderNode(Node* pNode, const cocos2d::Vec2& pos, const int tag)
{
    CCASSERT(pNode != nullptr, "invalid node");
    m_pContainerNode->addChild(pNode, BELOW_LEADER_ZORDER, tag);
    pNode->setPosition(pos);
}

void StageScene::removeOBLeaderNode(const int tag)
{
    m_pContainerNode->removeChildByTag(tag);
}

void StageScene::removeOBAllNode(const int tag)
{
    this->removeChildByTag(tag);
}

// 播放回答正确特效
void StageScene::playAnswerCorrectEffect(const Vec2 &pos)
{
    if (m_pDjcfActor != nullptr)
    {
        m_pDjcfActor->stop();
        m_pDjcfActor->setPosition(Vec2(pos.x + m_roundNodePos.x - m_sceneOffsetX, pos.y));
        m_pDjcfActor->setSize(Size(200, 200));
        m_pDjcfActor->setAnchorPoint(Vec2(0.5f, 0.5f));
        m_pDjcfActor->setPosAtNode(Vec2(-185, 315));
        m_pDjcfActor->start();
        m_pDjcfActor->setAnimationFinishedPlayDelegate([this]
                                                       {
                                                           m_pDjcfActor->setPosition(Vec2(-10000.0f, 0.0f));
                                                       });
    }
}

/*
 * 释放所有背景
 */
void StageScene::releaseAllBg()
{
    auto pTextureCache = Director::getInstance()->getTextureCache();
    std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
    for (auto it : bgImgArray)
    {
        //        log("bg imgfile = %s", it.m_file.c_str());
        pTextureCache->removeTextureForKey(it.m_file);
    } /**/
    m_bgSprites.clear();
    
    std::vector<ForeImgUnit> &foreImgArray = m_pAttachStageData->getForeImgArray();
    for (auto it : foreImgArray)
    {
        pTextureCache->removeTextureForKey(it.m_file);
    } /**/
    m_foreSps.clear();
}

// 根据当前回合结点位置加载背景图
void StageScene::loadBgByCurrPos()
{
    float offsetX = 0.0f;
    float foreOffsetX = 0.0f;
    float loadedSpritesWidth = 0.0f;
    float loadedForeSpWidth = 0.0f;
    auto visibleHeight = Director::getInstance()->getVisibleSize().height;
    
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    
    std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
    for (unsigned int i = 0; i < bgImgArray.size(); i++)
    {
        BgImgUnit biu = bgImgArray[i];
        
        // 分割的背景图片
        auto spBg = Sprite::create(biu.m_file);
        spBg->setAnchorPoint(Vec2::ZERO);
        m_loadedBgTotalWidth += spBg->getContentSize().width;
        
        if (1 == biu.m_tileOrient) /* 水平 */
        {
            spBg->setPosition(Vec2(offsetX, 0.0));
            m_pContainerNode->addChild(spBg, BG_IMG_ZORDER);
            offsetX += spBg->getContentSize().width;
        } else /* 垂直 */ {
            if (1 == biu.m_upDown) /*上升 */
            {
                spBg->setPosition(Vec2(offsetX, 0.0));
            } else /*下降*/ {
                spBg->setAnchorPoint(Vec2(0.0f, 1.0f));
                spBg->setPosition(Vec2(offsetX, visibleHeight));
            }
            m_pContainerNode->addChild(spBg, BG_IMG_ZORDER + 1);
            offsetX += (spBg->getContentSize().width);
        }
        m_bgSprites.push_back(spBg);
        
        loadedSpritesWidth += spBg->getContentSize().width;
        if (bgCanOver(std::abs(rsp.m_scroll)))
        {
            break;
        }
    } /*for*/
    
    std::vector<ForeImgUnit> &foreImgArray = m_pAttachStageData->getForeImgArray();
    for (unsigned int i = 0; i < foreImgArray.size(); i++) {
        ForeImgUnit fore = foreImgArray[i];
        
        //分割的前景图片
        auto foreBg = Sprite::create(fore.m_file);
        foreBg->setAnchorPoint(Vec2::ZERO);
        m_loadForeBgTotalWidth += foreBg->getContentSize().width;
        
        if (1 == fore.m_tileOrient)     /* 上升 */
        {
            foreBg->setPosition(Vec2(foreOffsetX, 0.0f));
            m_pContainerNode->addChild(foreBg, FORE_BG_IMG_ZORDER);
            foreOffsetX += foreBg->getContentSize().width;
        }else   /* 垂直 */
        {
            if (1 == fore.m_upDown) /*上升 */
            {
                foreBg->setPosition(Vec2(foreOffsetX, 0.0));
            } else /*下降*/ {
                foreBg->setAnchorPoint(Vec2(0.0f, 1.0f));
                foreBg->setPosition(Vec2(foreOffsetX, visibleHeight));
            }
            m_pContainerNode->addChild(foreBg, FORE_BG_IMG_ZORDER + 1);
            foreOffsetX += (foreBg->getContentSize().width);
        }
        m_foreSps.push_back(foreBg);
        
        loadedForeSpWidth += foreBg->getContentSize().width;
        if (foreBgCanOver(std::abs(rsp.m_scroll)))
        {
            break;
        }
    }
}

//
void StageScene::onBgLoadedCB(Texture2D* pTexture2D)
{
    std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
    
    // 已经加载过的背景纹理数量
    unsigned int numberOfLoadedBgs = (unsigned int)m_bgSprites.size();
    BgImgUnit biu = bgImgArray[numberOfLoadedBgs];
    
    // 分割的背景图片
    auto spBg = Sprite::createWithTexture(pTexture2D);
    spBg->setAnchorPoint(Vec2::ZERO);
    m_bgSprites.push_back(spBg);
    if (1 == biu.m_tileOrient) /* 水平 */
    {
        spBg->setPosition(Vec2(m_loadedBgTotalWidth, 0.0));
        m_pContainerNode->addChild(spBg, BG_IMG_ZORDER);
    } else /* 垂直 */ {
        if (1 == biu.m_upDown) /*上升 */
        {
            spBg->setPosition(Vec2(m_loadedBgTotalWidth, 0.0));
        } else /*下降*/ {
            spBg->setAnchorPoint(Vec2(0.0f, 1.0f));
            spBg->setPosition(Vec2(m_loadedBgTotalWidth,
                                   Director::getInstance()->getVisibleSize().height));
        }
        m_pContainerNode->addChild(spBg, BG_IMG_ZORDER + 1);
    }
    m_loadedBgTotalWidth += pTexture2D->getContentSize().width;
    
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    if (!bgCanOver(std::abs(rsp.m_scroll)))
    {
        auto pTextureCache = Director::getInstance()->getTextureCache();
        // 已经加载过的背景纹理数量
        unsigned int loadedSprites = (unsigned int)m_bgSprites.size();
        std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
        if (bgImgArray.size() == loadedSprites)
        {
            m_bgLoadCompleted = true;
        } else {
            BgImgUnit biu = bgImgArray[loadedSprites];
            pTextureCache->addImageAsync(biu.m_file, std::bind(&StageScene::onBgLoadedCB, this,
                                                               std::placeholders::_1));
        }
    } else {
        m_bgLoadCompleted = true;
    }
}

void StageScene::onForeBgLoadedCB(Texture2D* pTexture2D)
{
    std::vector<ForeImgUnit> &foreBgImgArray = m_pAttachStageData->getForeImgArray();
    
    // 已经加载过的背景纹理数量
    unsigned int numberOfLoadedForeBgs = (unsigned int)m_bgSprites.size();
    ForeImgUnit fore = foreBgImgArray[numberOfLoadedForeBgs];
    
    // 分割的背景图片
    auto spForeBg = Sprite::createWithTexture(pTexture2D);
    spForeBg->setAnchorPoint(Vec2::ZERO);
    m_foreSps.push_back(spForeBg);
    if (1 == fore.m_tileOrient) /* 水平 */
    {
        spForeBg->setPosition(Vec2(m_loadForeBgTotalWidth, 0.0));
        m_pContainerNode->addChild(spForeBg, FORE_BG_IMG_ZORDER);
    } else /* 垂直 */ {
        if (1 == fore.m_upDown) /*上升 */
        {
            spForeBg->setPosition(Vec2(m_loadForeBgTotalWidth, 0.0));
        } else /*下降*/ {
            spForeBg->setAnchorPoint(Vec2(0.0f, 1.0f));
            spForeBg->setPosition(Vec2(m_loadForeBgTotalWidth,
                                       Director::getInstance()->getVisibleSize().height));
        }
        m_pContainerNode->addChild(spForeBg, FORE_BG_IMG_ZORDER + 1);
    }
    m_loadForeBgTotalWidth += pTexture2D->getContentSize().width;
    
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    if (!foreBgCanOver(std::abs(rsp.m_scroll)))
    {
        auto pTextureCache = Director::getInstance()->getTextureCache();
        // 已经加载过的背景纹理数量
        unsigned int loadedSprites = (unsigned int)m_foreSps.size();
        std::vector<ForeImgUnit> &foreBgImgArray = m_pAttachStageData->getForeImgArray();
        if (foreBgImgArray.size() == loadedSprites)
        {
            m_asyncLoadedForeBg = true;
        } else {
            ForeImgUnit fore = foreBgImgArray[loadedSprites];
            pTextureCache->addImageAsync(fore.m_file, std::bind(&StageScene::onForeBgLoadedCB, this,
                                                                std::placeholders::_1));
        }
    } else {
        m_asyncLoadedForeBg = true;
    }
}

//
void StageScene::loadBgByCurrPosAsync()
{
    m_bgLoadCompleted = false;
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    if (!bgCanOver(std::abs(rsp.m_scroll)))
    {
        auto pTextureCache = Director::getInstance()->getTextureCache();
        // 已经加载过的背景纹理数量
        unsigned int numberOfLoadedBgs = (unsigned int)m_bgSprites.size();
        std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
        if (numberOfLoadedBgs == bgImgArray.size()) /*已经全部加载*/ {
            m_bgLoadCompleted = true;
        } else {
            BgImgUnit biu = bgImgArray[numberOfLoadedBgs];
            pTextureCache->addImageAsync(biu.m_file, std::bind(&StageScene::onBgLoadedCB, this,
                                                               std::placeholders::_1));
        }
    } else {
        m_bgLoadCompleted = true;
    }
}

void StageScene::loadForeBgByCurrPosAsync()
{
    m_asyncLoadedForeBg = false;
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    if (!foreBgCanOver(std::abs(rsp.m_scroll))) {
        auto pTextureCache = Director::getInstance()->getTextureCache();
        unsigned int numberOfLoadedForeBgs = (unsigned int)m_foreSps.size();
        std::vector<ForeImgUnit> &foreBgImgArray = m_pAttachStageData->getForeImgArray();
        if (numberOfLoadedForeBgs == foreBgImgArray.size()) {
            m_asyncLoadedForeBg = true;
        } else
        {
            ForeImgUnit fore = foreBgImgArray[numberOfLoadedForeBgs];
            pTextureCache->addImageAsync(fore.m_file, std::bind(&StageScene::onForeBgLoadedCB, this, std::placeholders::_1));
        }
    } else {
        m_asyncLoadedForeBg = true;
    }
}

void StageScene::loadBgByManualMove(const float offsetx, const std::function<void()>& loadedCB)
{
    unsigned int numberOfLoadedBgs = (unsigned int)m_bgSprites.size();
    std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
    if (numberOfLoadedBgs == bgImgArray.size()) /*已经全部加载*/
    {
        if (loadedCB){loadedCB();}
    } else {
        if (!bgCanOver(offsetx))
        {
            auto texturedLoadedCB = [this, loadedCB, bgImgArray, numberOfLoadedBgs] (Texture2D* pTexture)
            {
                BgImgUnit biu = bgImgArray[numberOfLoadedBgs];
                
                // 分割的背景图片
                auto spBg = Sprite::createWithTexture(pTexture);
                spBg->setAnchorPoint(Vec2::ZERO);
                m_bgSprites.push_back(spBg);
                if (1 == biu.m_tileOrient) /* 水平 */
                {
                    spBg->setPosition(Vec2(m_loadedBgTotalWidth, 0.0));
                    m_pContainerNode->addChild(spBg, BG_IMG_ZORDER);
                } else /* 垂直 */ {
                    if (1 == biu.m_upDown) /*上升 */
                    {
                        spBg->setPosition(Vec2(m_loadedBgTotalWidth, 0.0));
                    } else /*下降*/ {
                        spBg->setAnchorPoint(Vec2(0.0f, 1.0f));
                        spBg->setPosition(Vec2(m_loadedBgTotalWidth,
                                               Director::getInstance()->getVisibleSize().height));
                    }
                    m_pContainerNode->addChild(spBg, BG_IMG_ZORDER + 1);
                }
                m_loadedBgTotalWidth += pTexture->getContentSize().width;
                
                if (loadedCB) {loadedCB();}
            };
            auto pTextureCache = Director::getInstance()->getTextureCache();
            BgImgUnit biu = bgImgArray[numberOfLoadedBgs];
            pTextureCache->addImageAsync(biu.m_file, texturedLoadedCB);
        } else {
            if (loadedCB) {loadedCB();}
        }
    }
    
    unsigned int numberOfLoadedForeBgs = (unsigned int)m_foreSps.size();
    std::vector<ForeImgUnit> &foreImgArray = m_pAttachStageData->getForeImgArray();
    if (foreImgArray.size() > 0) {
        if (numberOfLoadedForeBgs == foreImgArray.size()) {
            if (loadedCB) {loadedCB();}
        }else
        {
            if (! foreBgCanOver(offsetx)) {
                auto texturedLoadedCB = [this, loadedCB, foreImgArray, numberOfLoadedForeBgs] (Texture2D* pTexture)
                {
                    ForeImgUnit fore = foreImgArray[numberOfLoadedForeBgs];
                    
                    // 分割的背景图片
                    auto spForeBg = Sprite::createWithTexture(pTexture);
                    spForeBg->setAnchorPoint(Vec2::ZERO);
                    m_foreSps.push_back(spForeBg);
                    if (1 == fore.m_tileOrient) /* 水平 */
                    {
                        spForeBg->setPosition(Vec2(m_loadedBgTotalWidth, 0.0));
                        m_pContainerNode->addChild(spForeBg, FORE_BG_IMG_ZORDER);
                    } else /* 垂直 */ {
                        if (1 == fore.m_upDown) /*上升 */
                        {
                            spForeBg->setPosition(Vec2(m_loadedBgTotalWidth, 0.0));
                        } else /*下降*/ {
                            spForeBg->setAnchorPoint(Vec2(0.0f, 1.0f));
                            spForeBg->setPosition(Vec2(m_loadForeBgTotalWidth,
                                                       Director::getInstance()->getVisibleSize().height));
                        }
                        m_pContainerNode->addChild(spForeBg, FORE_BG_IMG_ZORDER + 1);
                    }
                    m_loadForeBgTotalWidth += pTexture->getContentSize().width;
                    
                    if (loadedCB) {loadedCB();}
                };
            }
        }
    }
}

void StageScene::releaseBgByCurrPos()
{
    auto pTextureCache = Director::getInstance()->getTextureCache();
    std::vector<BgImgUnit> &bgImgArray = m_pAttachStageData->getBgImgArray();
    for (unsigned int i = 0; i < m_bgSprites.size(); i++)
    {
        auto bgSp = m_bgSprites[i];
        if (bgSp != nullptr)
        {
            if (bgSp->getPosition().x + bgSp->getContentSize().width <= m_roundNodePos.x)
            {
                bgSp->removeFromParent();
                pTextureCache->removeTextureForKey(bgImgArray[i].m_file);
                log("removed texture = %s", bgImgArray[i].m_file.c_str());
                m_bgSprites[i] = nullptr;
            }
        }
    } /*for*/
    
    std::vector<ForeImgUnit> &foreBgImgArray = m_pAttachStageData->getForeImgArray();
    if (foreBgImgArray.size() > 0) {
        for (unsigned int i = 0; i < m_foreSps.size(); i++)
        {
            auto bgSp = m_foreSps[i];
            if (bgSp != nullptr)
            {
                if (bgSp->getPosition().x + bgSp->getContentSize().width <= m_roundNodePos.x)
                {
                    bgSp->removeFromParent();
                    pTextureCache->removeTextureForKey(foreBgImgArray[i].m_file);
                    log("removed texture = %s", foreBgImgArray[i].m_file.c_str());
                    m_foreSps[i] = nullptr;
                }
            }
        } /*for*/
    }
}

bool StageScene::bgCanOver(const float offsetX)
{
    auto stageSceneWidth = AppConfig::getInstance()->getDesignResolutionWidth();
    return m_loadedBgTotalWidth >= (m_roundNodePos.x + stageSceneWidth + offsetX);
}

bool StageScene::foreBgCanOver(const float offsetX)
{
    auto stageSceneWidth = AppConfig::getInstance()->getDesignResolutionWidth();
    return m_loadForeBgTotalWidth >= (m_roundNodePos.x + stageSceneWidth + offsetX);
}

void StageScene::stageClear()
{
    auto stageIndex = getStageData()->getStageIndex();
    auto themeIndex = (stageIndex - (stageIndex % 12)) / 12;
    
    // 置过关标记
    auto pStageDataManager = StageDataManager::getInstance();
    bool isStageCleared = pStageDataManager->stageClear(stageIndex);
    if (!isStageCleared)
    {
        AppConfig::getInstance()->setStageClearIndex(stageIndex);
    }
    if (!AppConfig::getInstance()->isDebugMode())
    {
        AppConfig::getInstance()->addPassTimes();
    }
    
    // 友盟游戏统计
    char levelBuffer[16];
    sprintf(levelBuffer, "level-%02d", stageIndex + 1);
    umeng::MobClickCpp::finishLevel(levelBuffer);
    
    // 返回大地图
    char szBuffer[64];
    sprintf(szBuffer, "config/selStage/selStage%03d.json", themeIndex + 1);
    Director::getInstance()->replaceScene(SelStageScene::createWithStageConfig(szBuffer));
}

Sprite* StageScene::getCurrBgSprite()
{
    auto rsp = m_pAttachStageData->getRoundScroll(m_currRoundIndex);
    auto bgSprite = m_bgSprites[rsp.m_bgImgIndex];
    return bgSprite;
}


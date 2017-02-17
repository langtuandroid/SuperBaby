//
//  NpcBagNode.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/4/15.
//
//

#include "NpcBagNode.h"
#include "cocos2d.h"
#include "NpcData.h"
#include "NpcDataManager.h"
#include "NpcActor.h"
#include "NpcIntroduceNode.h"
#include "MyAudioEngine.h"
#include "LoadingNode.h"
#include "GAFPlayer/GAF.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "CustomMessage.h"
//#include "adjustPosNode.h"

using namespace ui;

#define BACKGROUND_PNG_FILE             "res/common/bag/beibaoshangceng.png"            // 背包背景框
#define BASE_PNG_FILE                   "res/common/bag/beibao-BG.png"                  // 背包背景底图
#define GEZI_WIDTH                      390                                             // 格子宽度
#define GEZI_HEIGHT                     390                                             // 格子高度

#define MAX_BOXS                        36                                              // 最大格子数

#define BASE_PNG_ZORDER                 1                                               // 背景底图z值
#define SCROLLVIEW_ZORDER               2                                               // 滚动层z值
#define BACKGROUND_PNG_ZORDER           3                                               // 背景框z值
#define BEIBAO_ZORDER                   4                                               // 背包纹理z值
#define NPC_ZORDER                      5                                               // npc z值
#define RETURN_ZORDER                   10                                              // 返回按钮z值
#define TOUCHOFFSETX                    10.0f
#define BAG_MUSIC_FILE                  "res/audio/npcbag/beibao-BGM.mp3"               //背包背景
#define NPC_NONE_COLLECT_MUSIC_FILE     "res/audio/npcbag/beibaodiancuo.mp3"            //npc未收集点击音效
#define NPC_COLLECTED_MUSIC_FILE        "res/audio/npcbag/beibaodianjidui.mp3"          //npc已收集点击音效
#define BTN_EFFECT_FILE                 "res/audio/ui/UIanniu01.mp3"

#define GWBB_IMG                        "res/common/bag/guaiwubeibao.png"
#define GWBB_PLIST                      "res/common/bag/guaiwubeibao.plist"

#define BUQH_PLIST                      "res/common/bag/BUQH.plist"
#define BUQH_PNG                        "res/common/bag/BUQH.png"
#define THEME_IMAGEVIEW_ZORDER          9

#define LOADINGNODE_TAG                 100

#define BG_PNG_FILE                     "laoding-bgD1.png"

/*
 *  构造函数
 */
NpcBagNode::NpcBagNode():m_pNpcDataManager(NpcDataManager::getInstance()),
m_pGafAssetCache(GafAssetCache::getInstance()) {
    
    m_npcTouchRect = Rect(244, 183, GEZI_WIDTH * 4, GEZI_HEIGHT * 3);
}

/*
 *  析构函数
 */
NpcBagNode::~NpcBagNode()
{
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(GWBB_PLIST);
    auto pTextureCache = Director::getInstance()->getTextureCache();
    pTextureCache->removeTextureForKey(BASE_PNG_FILE);
    pTextureCache->removeTextureForKey(BACKGROUND_PNG_FILE);
    pTextureCache->removeTextureForKey(GWBB_IMG);
    
    auto numberOfNpcs = m_pNpcDataManager->getNumberOfNpc();
    for (unsigned int i = 0; i < numberOfNpcs; i++)
    {
        NpcData* pNpcData = m_pNpcDataManager->getNpcDataByIndex(i);
        m_pGafAssetCache->removeGafAssetForKey(pNpcData->getGafFile());
    } /*for*/
    m_npcData.clear();
    m_npcBgSps.clear();
    for (unsigned int i = 0; i < 4; i++) {
        Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pThemeImageView[i]);
    }
}

//
bool NpcBagNode::init()
{
    m_pLoadingNode = LoadingNode::create();
    addChild(m_pLoadingNode, 100);
    
    auto textureLoadedCB = [this] (Texture2D* pTexture2D) {
        if (++m_textureLoaded == 3) {
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(GWBB_PLIST);
            unsigned int themeIndex = UserDefault::getInstance()->getIntegerForKey("themeIndex",0);
            loadNpcRes(themeIndex);
        }
    };
    auto pTextureCache = Director::getInstance()->getTextureCache();
    pTextureCache->addImageAsync(BASE_PNG_FILE, textureLoadedCB);
    pTextureCache->addImageAsync(BACKGROUND_PNG_FILE, textureLoadedCB);
    pTextureCache->addImageAsync(GWBB_IMG, textureLoadedCB);
    
    MyAudioEngine::getInstance()->playAudio(BAG_MUSIC_FILE,true,true,0.4f);
    return true;
}

//
void NpcBagNode::initUI()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    
    // 加载背景
    m_pbasePng = Sprite::create(BASE_PNG_FILE);
    m_pbasePng->setPosition(Vec2(dw / 2, size.height / 2));
    addChild(m_pbasePng, BASE_PNG_ZORDER);
    
    m_pbackGround = Sprite::create(BACKGROUND_PNG_FILE);
    m_pbackGround->setPosition(Vec2(dw / 2, size.height/2));
    addChild(m_pbackGround, BACKGROUND_PNG_ZORDER);
    
    // 返回按钮
    m_pReturnBtn = ui::Button::create("UI-fanhui01.png","UI-fanhui02.png","",ui::Widget::TextureResType::PLIST);
    m_pReturnBtn->setPosition(Vec2(22.0f + m_pReturnBtn->getContentSize().width / 2.0f,
                                   size.height - m_pReturnBtn->getContentSize().height / 2.0f - 22.0f));
    addChild(m_pReturnBtn, RETURN_ZORDER);
    m_pReturnBtn->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
        {
            MyAudioEngine::getInstance()->playAudio(BTN_EFFECT_FILE);
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(NPCBAG_END_CALLBACK);
            this->removeFromParent();
        }
    });
    
    // scrollview
    m_pScrollview = ScrollView::create();
    m_pScrollview->setTouchEnabled(true);
    m_pScrollview->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pScrollview->setPosition(Vec2(dw /2, size.height/2));
    m_pScrollview->setContentSize(Size(GEZI_HEIGHT * 4, GEZI_HEIGHT * 3));
    m_pScrollview->setDirection(SCROLLVIEW_DIR::VERTICAL);   //垂直滚动
    addChild(m_pScrollview, SCROLLVIEW_ZORDER);
    m_pScrollview->setBounceEnabled(true);
    
    m_pScrollview->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
    {
        if (type == ui::Widget::TouchEventType::BEGAN) {
         
        } else if (type == ui::Widget::TouchEventType::MOVED) {
         
        } else if (type == ui::Widget::TouchEventType::ENDED) {
         
        }
    });
    unsigned long themeIndex = UserDefault::getInstance()->getIntegerForKey("themeIndex",0);
    createThemeItems();
    initScrollView(themeIndex);
    createLoadingNode();
}

void NpcBagNode::createThemeItems()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BUQH_PLIST);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto OffsetWidth = AppConfig::getInstance()->getSceneOffsetX();
    char buffer[64];
    unsigned long themeIndex = UserDefault::getInstance()->getIntegerForKey("themeIndex",0);
    for (unsigned long i = 0; i < 4; i++) {
        if (i == 0) {
            if (themeIndex == i) {
                sprintf(buffer, "fmsl02.png");
            }else
            {
                sprintf(buffer, "fmsl01.png");
            }
            m_pThemeImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
            auto size = m_pThemeImageView[i]->getContentSize();
            if (themeIndex == i) {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 - size.width*3/2 - 40 - OffsetWidth, visibleSize.height - size.height/2+30));
            }else
            {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 - size.width*3/2 - 40 - OffsetWidth, visibleSize.height - size.height/2+50));
            }
        }else if (i == 1)
        {
            if (themeIndex == i) {
                sprintf(buffer, "smxq02.png");
            }else
            {
                sprintf(buffer, "smxq01.png");
            }
            m_pThemeImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
            auto size = m_pThemeImageView[i]->getContentSize();
            if (themeIndex == i) {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 - size.width/2 - 20 - OffsetWidth, visibleSize.height - size.height/2+30));
            }else
            {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 - size.width/2 - 20 - OffsetWidth, visibleSize.height - size.height/2+50));
            }
        }else if (i == 2)
        {
            if (themeIndex == i) {
                sprintf(buffer, "hlhy02.png");
            }else
            {
                sprintf(buffer, "hlhy01.png");
            }
            m_pThemeImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
            auto size = m_pThemeImageView[i]->getContentSize();
            if (themeIndex == i) {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 + size.width/2 + 20 - OffsetWidth, visibleSize.height - size.height/2+30));
            }else
            {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 + size.width/2 + 20 - OffsetWidth, visibleSize.height - size.height/2+50));
            }
        }else
        {
            if (themeIndex == i) {
                sprintf(buffer, "wmgc02.png");
            }else
            {
                sprintf(buffer, "wmgc01.png");
            }
            m_pThemeImageView[i] = ImageView::create(buffer,ui::Widget::TextureResType::PLIST);
            auto size = m_pThemeImageView[i]->getContentSize();
            if (themeIndex == i) {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 + size.width*3/2 + 40 - OffsetWidth, visibleSize.height - size.height/2+30));
            }else
            {
                m_pThemeImageView[i]->setPosition(Vec2(visibleSize.width/2 + size.width*3/2 + 40 - OffsetWidth, visibleSize.height - size.height/2+50));
            }
        }
        m_pThemeImageView[i]->setUserData((void*)i);
        this->addChild(m_pThemeImageView[i], THEME_IMAGEVIEW_ZORDER);
    }
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto themeImageViewListen = EventListenerTouchOneByOne::create();
    themeImageViewListen->setSwallowTouches(true);
    themeImageViewListen->onTouchBegan = [this](Touch* pTouch, Event* pEvent) -> bool
    {
        auto target = (ImageView*)pEvent->getCurrentTarget();
        auto touchPoint = target->convertTouchToNodeSpace(pTouch);
        auto touchRect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (touchRect.containsPoint(touchPoint))
        {
            return true;
        }
        return false;
    };
    themeImageViewListen->onTouchEnded = [this,themeIndex](Touch* pTouch, Event* pEvent)
    {
        auto target = (ImageView*)pEvent->getCurrentTarget();
        long Index = (long)target->getUserData();
        auto touchPoint = target->convertTouchToNodeSpace(pTouch);
        auto touchRect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        unsigned long curThemeIndex = UserDefault::getInstance()->getIntegerForKey("themeIndex",0);
        if (touchRect.containsPoint(touchPoint))
        {
            char buffer[64];
            if (curThemeIndex != Index) {
                if (curThemeIndex == 0) {
                    sprintf(buffer, "fmsl01.png");
                    
                }else if (curThemeIndex == 1)
                {
                    sprintf(buffer, "smxq01.png");
                    
                }else if (curThemeIndex == 2)
                {
                    sprintf(buffer, "hlhy01.png");
                    
                }else
                {
                    sprintf(buffer, "wmgc01.png");
                }
                m_pThemeImageView[curThemeIndex]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
                m_pThemeImageView[curThemeIndex]->runAction(Sequence::create(MoveBy::create(0.1f, Vec2(0.0f, 40.0f)),MoveBy::create(0.05f, Vec2(0.0f, -20.0f)), NULL));
                if (Index == 0)
                {
                    sprintf(buffer, "fmsl02.png");
                    
                }else if (Index == 1)
                {
                    sprintf(buffer, "smxq02.png");
                    
                }else if (Index == 2)
                {
                    sprintf(buffer, "hlhy02.png");
                    
                }else
                {
                    sprintf(buffer, "wmgc02.png");
                }
                m_pThemeImageView[Index]->loadTexture(buffer,ui::Widget::TextureResType::PLIST);
                m_pThemeImageView[Index]->runAction(Sequence::create(MoveBy::create(0.1f, Vec2(0.0f, -40.0f)),MoveBy::create(0.05f, Vec2(0.0f, 20.0f)), NULL));
                showLoadingLayer(Index);
                
                UserDefault::getInstance()->setIntegerForKey("themeIndex", (int)Index);
            }
        }
    };
    dispatcher->setPriority(themeImageViewListen, -135);
    dispatcher->addEventListenerWithSceneGraphPriority(themeImageViewListen, m_pThemeImageView[0]);
    dispatcher->addEventListenerWithSceneGraphPriority(themeImageViewListen->clone(), m_pThemeImageView[1]);
    dispatcher->addEventListenerWithSceneGraphPriority(themeImageViewListen->clone(), m_pThemeImageView[2]);
    dispatcher->addEventListenerWithSceneGraphPriority(themeImageViewListen->clone(), m_pThemeImageView[3]);
}

void NpcBagNode::loadNpcRes(const unsigned long themeIndex, const bool isEnter)
{
    auto numberOfNpcs = m_pNpcDataManager->getNumberOfNpc();
    auto themeNpcNum = m_pNpcDataManager->getNumberOfThemeNpc(themeIndex);
    auto gafLoadedCB = [this, themeNpcNum,themeIndex,isEnter] (GAFAsset* pGafAsset) {
        if (++m_gafLoaed == themeNpcNum) {
            m_gafLoaed = 0;
            if (isEnter) {
                initUI();
                if (m_pLoadingNode != nullptr)
                {
                    m_pLoadingNode->removeFromParent();
                    m_pLoadingNode = nullptr;
                }
            }else
            {
                initScrollView(themeIndex,false);
            }
        }
    };
    for (unsigned int i = 0; i < numberOfNpcs; i++)
    {
        NpcData* pNpcData = m_pNpcDataManager->getNpcDataByIndex(i);
        if (pNpcData->getThemeIndex() == themeIndex) {
            m_pGafAssetCache->addGafAssetAsync(pNpcData->getGafFile(), gafLoadedCB);
//            m_npcName.push_back(pNpcData->getGafConfigFile());
            m_npcData.push_back(pNpcData);
        }
    } /*for*/
}

/* 初始化scrollview */
void NpcBagNode::initScrollView(const unsigned long themeIndex, const bool isEnter)
{
    long maxRowNum = 0;
    long numberOfNpcs = m_pNpcDataManager->getNumberOfThemeNpc(themeIndex);
    
    if (numberOfNpcs % 4 > 0) {
        maxRowNum = numberOfNpcs / 4;
        maxRowNum++;
    }else
    {
        maxRowNum = numberOfNpcs / 4;
    }
    
    m_pScrollview->setInnerContainerSize(Size(GEZI_WIDTH * 4, GEZI_HEIGHT * maxRowNum));
    m_pScrollview->jumpToTop();
    Size ContainerSize = m_pScrollview->getInnerContainer()->getContentSize();
    
    // 添加 npc 底图
    char dtBuffer[64];
    bool isChange = false;       //判断是否满一行（四个），满一行更换排列顺序
    int index = 0;
    long maxBoxNum = 0;
    if (numberOfNpcs % 4 > 0) {
        maxBoxNum = numberOfNpcs / 4;
        maxBoxNum++;
    }else
    {
        maxBoxNum = numberOfNpcs / 4;
    }
    for (unsigned int i = 0; i < maxBoxNum * 4; i++) {
        if (isChange) {                     //第一种排列
            strncpy(dtBuffer,  i % 2 == 0 ? "gezi02.png" : "gezi01.png", sizeof(dtBuffer));
        } else /*第二种排列*/
        {
            strncpy(dtBuffer,  i % 2 == 0 ? "gezi01.png" : "gezi02.png", sizeof(dtBuffer));
        }
        if (++index == 4) {           //满四个（满一行）更换isChange的值，改变排列方式
            index = 0;
            isChange = (i / 4 % 2 == 0);
        }
        
        auto npcBgSp = Sprite::createWithSpriteFrameName(dtBuffer);
        npcBgSp->setAnchorPoint(Vec2::ZERO);
        npcBgSp->setTag(i);
        npcBgSp->setPosition(Vec2(npcBgSp->getContentSize().width*(i%4),
                                  (ContainerSize.height-npcBgSp->getContentSize().height*(i/4+1))));
        
        m_pScrollview->addChild(npcBgSp);
        npcBgSp->setTag(100+i);
        m_npcBgSps.push_back(npcBgSp);
    } /*for*/
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto npcListen = EventListenerTouchOneByOne::create();
    npcListen->onTouchBegan = [this](Touch* pTouch, Event* pEvent) -> bool
    {
        auto pNpcActor = (NpcActor*)pEvent->getCurrentTarget();
        auto touchPoint = pNpcActor->convertTouchToNodeSpace(pTouch);
        auto touchRect = Rect(0.0f, 0.0f, pNpcActor->getSize().width, pNpcActor->getSize().height);
        if (touchRect.containsPoint(touchPoint))
        {
            m_originalPoint = pNpcActor->getPosition();
            m_touchBeganPoint = pTouch->getLocation();
            return true;
        }
        return false;
    };
    npcListen->onTouchEnded = [this,dispatcher](Touch* pTouch, Event* pEvent)
    {
        auto pNpcActor = (NpcActor*)pEvent->getCurrentTarget();
        auto touchPoint = pNpcActor->convertTouchToNodeSpace(pTouch);
        auto layerPoint = this->convertTouchToNodeSpace(pTouch);
        auto touchRect = Rect(0.0f, 0.0f, pNpcActor->getSize().width, pNpcActor->getSize().height);
        
        Vec2 touchEndPoint = pTouch->getLocation();
        Vec2 m_offSetPos;
        
        m_offSetPos.x = touchEndPoint.x - m_touchBeganPoint.x;
        m_offSetPos.y = touchEndPoint.y - m_touchBeganPoint.y;
        
        if (fabsf(m_offSetPos.y) < TOUCHOFFSETX)
        {
            if (m_npcTouchRect.containsPoint(layerPoint)) {
                if (touchRect.containsPoint(touchPoint))
                {
                    auto pNpcData = (NpcData*)pNpcActor->getUserData();
                    if (pNpcData != nullptr && pNpcData->isCollected()) /*该npc已经被收集*/
                    {
                        MyAudioEngine::getInstance()->playAudio(NPC_COLLECTED_MUSIC_FILE);
                        //                    log("%s isCollected", pNpcActor->getKey().c_str());
                        
                        pauseTouch();           //暂停按钮触摸
                        NpcIntroduceNode* npcIntroduceNode = NpcIntroduceNode::createWithNpcKey(pNpcActor->getKey().c_str());
                        npcIntroduceNode->attachNpcBagNode(this);
                        addChild(npcIntroduceNode, 11);
                    } else {
                        MyAudioEngine::getInstance()->playAudio(NPC_NONE_COLLECT_MUSIC_FILE);
                    }
                }
            }
        }
    };
    
    unsigned int i = 0;
    for (auto const &it : m_npcBgSps)
    {
//        auto pNpcData = m_pNpcDataManager->getNpcDataByIndex(i);
        if (i < m_npcData.size()) {
            if (m_npcData[i] != nullptr)
            {
                auto npcActor = NpcActor::createWithGafConfigFile(m_npcData[i]->getGafConfigFile());
                npcActor->setUserData(m_npcData[i]);
                
                m_pScrollview->addChild(npcActor, NPC_ZORDER,i+1000);
                auto npcSize = npcActor->getSize();
                float posx = it->getPosition().x + (it->getContentSize().width - npcSize.width) / 2.0f;
                float posy = it->getPosition().y + (it->getContentSize().height - npcSize.height) / 2.0f;
                npcActor->setPosition(Vec2(posx, posy-50));
                
                it->setUserData((void*)npcActor);
                
                dispatcher->addEventListenerWithSceneGraphPriority(npcListen->clone(), npcActor);
                dispatcher->setPriority(npcListen->clone(), -130);
            } else {
                it->setUserObject(nullptr);
            }
            i++;
            if (i == m_npcData.size()) {
                if (!isEnter) {
                    hideLoadingLayer(themeIndex);
                }
            }
        }
    } /*for*/
    
    // 更新 npc 状态
    updateNpcUI();
}

/* 清除scrollView */
void NpcBagNode::clearScrollView(const unsigned long newThemeIndex)
{
//    auto numberOfNpcs = m_pNpcDataManager->getNumberOfThemeNpc((int)themeIndex);
    for (unsigned int i = 0; i < m_npcBgSps.size(); i++) {
        auto geziSp = m_pScrollview->getChildByTag(100+i);
        geziSp->removeFromParent();
    }
    
    for (unsigned int i = 0; i < m_npcData.size(); i++) {
        auto npcActor = m_pScrollview->getChildByTag(i+1000);
        npcActor->removeFromParent();
        Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(npcActor);
    }
    
//    for (unsigned int i = 0; i < numberOfNpcs; i++)
//    {
//        NpcData* pNpcData = m_npcData[i];
//        m_pGafAssetCache->removeGafAssetForKey(pNpcData->getGafFile());
//    } /*for*/
    
    for (auto &iter : m_npcData) {
        m_pGafAssetCache->removeGafAssetForKey(iter->getGafFile());
    } /*for*/
    
    m_npcData.clear();
    m_npcBgSps.clear();
    //导入新主题npc资源
    loadNpcRes(newThemeIndex,false);
}

//暂停触摸
void NpcBagNode::pauseTouch()
{
    m_pReturnBtn->setEnabled(false);
    m_pScrollview->setInertiaScrollEnabled(false);
}
//恢复触摸
void NpcBagNode::resumeTouch()
{
    m_pReturnBtn->setEnabled(true);
    m_pScrollview->setInertiaScrollEnabled(true);
}

//
void NpcBagNode::updateNpcUI()
{
    unsigned int i = 0;
    for (auto const &it : m_npcBgSps)
    {
        auto pNpcActor = (NpcActor*)it->getUserData();
        if ( pNpcActor != nullptr)
        {
            auto pNpcData = (NpcData*)pNpcActor->getUserData();
            if (pNpcData->isCollected()) {
                pNpcActor->standby(true);
            } else {
                pNpcActor->sleep();
            }
        }
        i++;
    } /*for*/
}

//创建loading节点
void NpcBagNode::createLoadingNode()
{
    auto visibleSize = Director::getInstance()->getWinSize();
    //创建loading节点
    Layer* loadingLayer = Layer::create();
    this->addChild(loadingLayer, SCROLLVIEW_ZORDER, LOADINGNODE_TAG);
    auto OffsetWidth = AppConfig::getInstance()->getSceneOffsetX();
    
    Layer* layerLeft = Layer::create();
    loadingLayer->addChild(layerLeft, 1, 1);
    
    Sprite* leftNode = Sprite::createWithSpriteFrameName("laoding-bg01.png");
    Size leftNodeSize = leftNode->getContentSize();
    leftNode->setPosition(Vec2(859+leftNodeSize.width/2, leftNodeSize.height/2));
    layerLeft->addChild(leftNode);
    
    LayerColor* colorLayerLeft = LayerColor::create(Color4B(255, 149, 23, 255), 860, leftNodeSize.height);
    layerLeft->addChild(colorLayerLeft);
    
    layerLeft->setContentSize(Size(leftNodeSize.width+860, leftNodeSize.height));
    Size layerLeftSize = layerLeft->getContentSize();
    layerLeft->setPosition(Vec2(visibleSize.width/2 - layerLeftSize.width*2 - OffsetWidth+46, (visibleSize.height - layerLeftSize.height)/2));
//    layerLeft->setPosition(Vec2(visibleSize.width/2 - layerLeftSize.width - OffsetWidth+46, (visibleSize.height - layerLeftSize.height)/2));
    
    Vec2 leftBgsPos[6];
    leftBgsPos[0] = Vec2(100, 100);
    leftBgsPos[1] = Vec2(186, 1184);
    leftBgsPos[2] = Vec2(400, 900);
    leftBgsPos[3] = Vec2(600, 200);
    leftBgsPos[4] = Vec2(630, 1000);
    leftBgsPos[5] = Vec2(500, 500);
    
    float leftBgsScale[6];
    leftBgsScale[0] = 0.4f;
    leftBgsScale[1] = 0.7f;
    leftBgsScale[2] = 0.5f;
    leftBgsScale[3] = 0.6f;
    leftBgsScale[4] = 0.9f;
    leftBgsScale[5] = 1.0f;
    
    for (unsigned int i = 0; i < 6; i++) {
        auto leftBg = Sprite::createWithSpriteFrameName(BG_PNG_FILE);
        leftBg->setScale(leftBgsScale[i]);
        leftBg->setPosition(leftBgsPos[i]);
        layerLeft->addChild(leftBg,1,100+i);
    }
    
    Layer* layerRight = Layer::create();
    loadingLayer->addChild(layerRight, 1, 2);
    
    Sprite* rightNode = Sprite::createWithSpriteFrameName("laoding-bg02.png");
    Size rightNodeSize = rightNode->getContentSize();
    rightNode->setPosition(Vec2(rightNodeSize.width/2, rightNodeSize.height/2));
    layerRight->addChild(rightNode);
    
    LayerColor* colorLayerRight = LayerColor::create(Color4B(255, 149, 23, 255), 860, rightNodeSize.height);
    colorLayerRight->setPosition(Vec2(rightNodeSize.width-1, 0));
    layerRight->addChild(colorLayerRight);
    
    layerRight->setContentSize(Size(rightNodeSize.width+860, rightNodeSize.height));
    Size layerRightSize = layerRight->getContentSize();
    layerRight->setPosition(Vec2(visibleSize.width/2 + layerRightSize.width - OffsetWidth-45, (visibleSize.height - layerRightSize.height)/2));
    
    Sprite* loadingNode = Sprite::createWithSpriteFrameName("laoding.png");
    loadingNode->setPosition(Vec2(visibleSize.width/2 - OffsetWidth, visibleSize.height/2));
    loadingLayer->addChild(loadingNode, 2, 3);
    loadingNode->runAction(RepeatForever::create(RotateBy::create(2.0f, 360)));
    loadingNode->setVisible(false);
    
    Vec2 rightBgsPos[6];
    rightBgsPos[0] = Vec2(100, 100);
    rightBgsPos[1] = Vec2(186, 1184);
    rightBgsPos[2] = Vec2(400, 900);
    rightBgsPos[3] = Vec2(600, 200);
    rightBgsPos[4] = Vec2(630, 1000);
    rightBgsPos[5] = Vec2(500, 500);
    
    float rightBgsScale[6];
    rightBgsScale[0] = 0.4f;
    rightBgsScale[1] = 0.7f;
    rightBgsScale[2] = 0.5f;
    rightBgsScale[3] = 0.6f;
    rightBgsScale[4] = 0.9f;
    rightBgsScale[5] = 1.0f;
    
    for (unsigned int i = 0; i < 6; i++) {
        auto rightBg = Sprite::createWithSpriteFrameName(BG_PNG_FILE);
        rightBg->setScale(leftBgsScale[i]);
        rightBg->setPosition(leftBgsPos[i]);
        layerRight->addChild(rightBg,1,100+i);
    }
    
//    auto anode = adjustPosNode::createWithNode(layerRight->getChildByTag(105));
//    addChild(anode, 100);
}

void NpcBagNode::showLoadingLayer(const unsigned long newThemeIndex)
{
    for (unsigned int i = 0; i < 4; i++) {
        Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(m_pThemeImageView[i]);
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto OffsetWidth = AppConfig::getInstance()->getSceneOffsetX();
    Layer* leftLayer = (Layer*)this->getChildByTag(LOADINGNODE_TAG)->getChildByTag(1);
    Size leftLayerSize = leftLayer->getContentSize();
    leftLayer->runAction(MoveTo::create(1.0f, Vec2(visibleSize.width/2 - leftLayerSize.width - OffsetWidth+46, (visibleSize.height - leftLayerSize.height)/2)));

    Layer* rightLayer = (Layer*)this->getChildByTag(LOADINGNODE_TAG)->getChildByTag(2);
    Size rightLayerSize = rightLayer->getContentSize();
    rightLayer->runAction(Sequence::create(MoveTo::create(1.0f, Vec2(visibleSize.width/2 - OffsetWidth-45, (visibleSize.height - rightLayerSize.height)/2)),CallFunc::create([this,newThemeIndex]{
        Sprite* loadingNode = (Sprite*)this->getChildByTag(LOADINGNODE_TAG)->getChildByTag(3);
        loadingNode->setVisible(true);
        clearScrollView(newThemeIndex);
    }), NULL));
}

void NpcBagNode::hideLoadingLayer(const unsigned long themeIndex)
{
    auto visibleSize = Director::getInstance()->getWinSize();
    auto OffsetWidth = AppConfig::getInstance()->getSceneOffsetX();
    Layer* leftLayer = (Layer*)this->getChildByTag(LOADINGNODE_TAG)->getChildByTag(1);
    Size leftLayerSize = leftLayer->getContentSize();
    leftLayer->runAction(MoveTo::create(1.0f, Vec2(visibleSize.width/2 - leftLayerSize.width*2 - OffsetWidth+45, (visibleSize.height - leftLayerSize.height)/2)));
    
    Layer* rightLayer = (Layer*)this->getChildByTag(LOADINGNODE_TAG)->getChildByTag(2);
    Size rightLayerSize = rightLayer->getContentSize();
    rightLayer->runAction(Sequence::create(MoveTo::create(1.0f, Vec2(visibleSize.width/2 + rightLayerSize.width - OffsetWidth-45, (visibleSize.height - rightLayerSize.height)/2)),CallFunc::create([this,themeIndex]{
        for (unsigned int i = 0; i < 4; i++) {
            if (i != themeIndex) {
                Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(m_pThemeImageView[i]);
            }
        }
    }), NULL));
    
    Sprite* loadingNode = (Sprite*)this->getChildByTag(LOADINGNODE_TAG)->getChildByTag(3);
    loadingNode->setVisible(false);
}




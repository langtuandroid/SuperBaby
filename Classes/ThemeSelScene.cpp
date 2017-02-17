//
//  ThemeSelScene.cpp
//  AbelLLExplorer
//
//  Created by YANGJIE on 2015/04/19.
//
//

#include "ThemeSelScene.h"
#include "ThemeIntroScene.h"
#include "ThemeAssetInitScene.h"
#include "Actor.h"
#include "GafAssetCache.h"
#include "MyAudioEngine.h"
#include "AppConfig.h"
#include "CustomMessage.h"
#include "MyScrollView.h"
//#include "BuyNewThemeNode.h"
#include "NetUtil.h"
#include "LoadingScene.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidRollBackListener.h"
#endif

using namespace gaf;

#define BG_IMG              "res/theme/ZTXZ-BJ.png"         // 背景
#define THEME1_IMG          "res/theme/ZTXZ-1-SL.png"       // 主题1:魔法森林
#define THEME2_IMG          "res/theme/ZTXZ-2-XQ.png"       // 主题2:神秘星球
#define THEME3_IMG          "res/theme/ZTXZ-3-HY.png"       // 主题3:海洋
#define THEME4_IMG          "res/theme/ZTXZ-4-GC.png"       // 主题4:文明古国
#define RETURN_BTN_POS      Vec2(132.0f, 1408.0f)                 //返回按钮坐标
#define SCROLLVIEW_TAG      1001
#define ZTXZ_1_GAF_FILE     "res/theme/ZTXZ-1/ZTXZ-1.gaf"   //主题一题目gaf
#define ZTXZ_2_GAF_FILE     "res/theme/ZTXZ-2/ZTXZ-2.gaf"   //主题二题目gaf
#define ZTXZ_3_GAF_FILE     "res/theme/ZTXZ-3/ZTXZ-3.gaf"   //主题三题目gaf
#define ZTXZ_4_GAF_FILE     "res/theme/ZTXZ-4/ZTXZ-4.gaf"   //主题四题目gaf
#define TOUCHOFFSETX        10
#define THEME_SLIP_MUSIC_FILE   "res/audio/theme/zhutidianji.mp3"   //点击音效
#define BTN_EFFECT_FILE                     "res/audio/ui/UIanniu01.mp3"

#define THEME_PLIST             "res/theme/zhitixuanze01.plist"
#define THEME_IMG             "res/theme/zhitixuanze01.png"

#define BG_LEFT_IMAGE               "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE              "res/common/bgRight.png"
#define BG_PLIST             "res/theme/BG-xiaodian.plist"
#define BG_PNG               "res/theme/BG-xiaodian.png"

/*
 *  constructor
 */
ThemeSelScene::ThemeSelScene()
{
    m_sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
    
    m_themePoses[0] = Vec2::ZERO;
    m_themePoses[1] = Vec2(-1252, 0);
    m_themePoses[2] = Vec2(-2504, 0);
    m_themePoses[3] = Vec2(-3756, 0);
    
    m_judgePoses[0] = Vec2(-626, 0);
    m_judgePoses[1] = Vec2(-1878, 0);
    m_judgePoses[2] = Vec2(-3130, 0);
    
    m_lumpsPos[0] = Vec2(1800, 1336);
    m_lumpsPos[1] = Vec2(1600, 436);
    m_lumpsPos[2] = Vec2(400, 736);
    m_lumpsPos[3] = Vec2(300, 1236);
    m_lumpsPos[4] = Vec2(274, 158);
    m_lumpsPos[5] = Vec2(1850, 75);
    
    m_scrollViewRect = Rect(0.0f, 120.0f, 2048.0f, 1253.0f);
}

/*
 *  destructor
 */
ThemeSelScene::~ThemeSelScene()
{
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(THEME_PLIST);
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(BG_PLIST);
    auto pTextureCache = Director::getInstance()->getTextureCache();
    pTextureCache->removeTextureForKey(THEME1_IMG);
    pTextureCache->removeTextureForKey(THEME2_IMG);
    pTextureCache->removeTextureForKey(THEME3_IMG);
    pTextureCache->removeTextureForKey(THEME4_IMG);
    pTextureCache->removeTextureForKey(THEME_IMG);
    
    GafAssetCache::getInstance()->removeGafAssetForKey(ZTXZ_1_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(ZTXZ_2_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(ZTXZ_3_GAF_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(ZTXZ_4_GAF_FILE);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    if (m_pLayer != nullptr) {
        dispatcher->removeEventListenersForTarget(m_pLayer);
    }
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (m_androidRollBackListener != nullptr) {
        delete m_androidRollBackListener;
        m_androidRollBackListener = nullptr;
    }
#endif
}

/*
 *  init
 */
bool ThemeSelScene::init()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BG_PLIST);
    m_pReturnBtn = ui::Button::create("UI-fanhui01.png","UI-fanhui02.png","UI-fanhui01.png",ui::Widget::TextureResType::PLIST);
    auto retPos = RETURN_BTN_POS;
    m_pReturnBtn->setPosition(Vec2(retPos.x + m_sceneOffset, retPos.y));
    addChild(m_pReturnBtn, 5);
    m_pReturnBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
                                        {
                                            if (ui::Widget::TouchEventType::ENDED == type) {
                                                MyAudioEngine::getInstance()->playAudio(BTN_EFFECT_FILE);
                                                //返回首页
                                                m_pReturnBtn->setEnabled(false);
                                                Director::getInstance()->replaceScene(LoadingScene::createWithIndex(1));
                                            }
                                        });
    m_pReturnBtn->setScale(0.0f);
    
    auto layerGradientBG = LayerGradient::create(Color4B(9,124,204,255), Color4B(20,200,233,255), Vec2(0.5,1));
    layerGradientBG->setAnchorPoint(Vec2::ZERO);
    layerGradientBG->setPosition(Vec2(m_sceneOffset, 0.0f));
    addChild(layerGradientBG, 1);
    
    char buffer[64];
    for (unsigned int i = 0; i < 6; i++) {
        sprintf(buffer, "BG-xiaodian%02d.png",i+1);
        auto lump = Sprite::createWithSpriteFrameName(buffer);
        lump->setPosition(m_lumpsPos[i]);
        this->addChild(lump,1);
    }
    
    return true;
}

//
void ThemeSelScene::onEnter()
{
    Scene::onEnter();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_androidRollBackListener = AndroidRollBackListener::create(2);
    m_androidRollBackListener->backScene();
#endif
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(THEME_PLIST);
    initUI();
}

//
void ThemeSelScene::initUI()
{
    
    // 主题选择
    const float hOffset = 174.0f;
    const float spacing = 150.0f;
    const float dw = AppConfig::getInstance()->getDesignResolutionWidth();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float margin = 0.0f;
    
    char ztBuffer[64];
    float ztPosX = visibleSize.width / 2.0f - 200, offSetX = 100, ztPosY = 100;
    for (unsigned int i = 0; i < 4; i++) {
        if (i == 0) {
            sprintf(ztBuffer, "ZTXZ-baidian.png");
        } else
        {
            sprintf(ztBuffer, "ZTXZ-dian.png");
        }
        m_pCurThemeIndexImageViews[i] = ImageView::create(ztBuffer,ui::Widget::TextureResType::PLIST);
        m_pCurThemeIndexImageViews[i]->setPosition(Vec2(ztPosX + offSetX * i, ztPosY));
        this->addChild(m_pCurThemeIndexImageViews[i], 2);
    }
    
    // 左右遮罩
    if (m_sceneOffset > 0.0f)
    {
        auto bgLeftSp = Sprite::create(BG_LEFT_IMAGE);
        bgLeftSp->setAnchorPoint(Vec2(1.0f, 0.0f));
        bgLeftSp->setPosition(Vec2(m_sceneOffset, 0.0f));
        bgLeftSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgLeftSp->setScaleX(m_sceneOffset / bgLeftSp->getContentSize().width);
        addChild(bgLeftSp, 1000);
        
        auto bgRightSp = Sprite::create(BG_RIGHT_IMAGE);
        bgRightSp->setAnchorPoint(Vec2(0.0f, 0.0f));
        bgRightSp->setPosition(Vec2(m_sceneOffset + AppConfig::getInstance()->getDesignResolutionWidth(), 0.0f));
        bgRightSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgRightSp->setScaleX(m_sceneOffset / bgLeftSp->getContentSize().width);
        addChild(bgRightSp, 1000);
    }
    
    m_pLayer = Layer::create();
    m_pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pLayer->setPosition(Vec2(0.0f, 0.0f));
    addChild(m_pLayer, 4);
    
    m_pScrollView = MyScrollView::create();
    m_pScrollView->setDirection(MyScrollView::Direction::HORIZONTAL);
    m_pScrollView->setAnchorPoint(Vec2::ZERO);
    m_pScrollView->setPosition(Vec2(m_sceneOffset, 120.0f));
    m_pScrollView->setBounceable(true);
    
    m_pThemeImageViews[0] = ImageView::create(THEME1_IMG);
    m_pThemeImageViews[1] = ImageView::create(THEME2_IMG);
    m_pThemeImageViews[2] = ImageView::create(THEME3_IMG);
    m_pThemeImageViews[3] = ImageView::create(THEME4_IMG);
    
    m_pScrollView->setViewSize(Size(dw, m_pThemeImageViews[0]->getContentSize().height + hOffset));
    
    margin = (dw - m_pThemeImageViews[0]->getContentSize().width) / 2.0f;
    float scrollViewWidth = m_pThemeImageViews[0]->getContentSize().width * 4 + margin * 2.0f + spacing * 3.0f;
    m_pScrollView->setContentSize(Size(scrollViewWidth, m_pThemeImageViews[0]->getContentSize().height + hOffset));
    addChild(m_pScrollView, 2, SCROLLVIEW_TAG);
    m_pScrollView->addChild(m_pThemeImageViews[0]);
    m_pThemeImageViews[0]->setAnchorPoint(Vec2::ZERO);
    m_pThemeImageViews[0]->setPosition(Vec2(margin, 0.0f));
    m_pThemeImageViews[0]->setUserData(0);
    
    // 题目gaf
    m_pTitleActor[0] = Actor::createWithGafFile(ZTXZ_1_GAF_FILE);
    m_pTitleActor[0]->setPosition(Vec2(555.0f, 950));
    m_pTitleActor[0]->setSize(Size(750, 300));
    m_pTitleActor[0]->setPosAtNode(Vec2(-110, 400));
    m_pThemeImageViews[0]->addChild(m_pTitleActor[0],1);
    m_pTitleActor[0]->setAnchorPoint(Vec2(0.5f, 0.0f));
    
    m_pScrollView->addChild(m_pThemeImageViews[1]);
    m_pThemeImageViews[1]->setAnchorPoint(Vec2::ZERO);
    m_pThemeImageViews[1]->setPosition(Vec2(margin + m_pThemeImageViews[1]->getContentSize().width + spacing, 0.0f));
    m_pThemeImageViews[1]->setUserData((void*)1);
    
    //题目gaf
    m_pTitleActor[1] = Actor::createWithGafFile(ZTXZ_2_GAF_FILE);
    m_pTitleActor[1]->setPosition(Vec2(555, 950));
    m_pTitleActor[1]->setSize(Size(750, 300));
    m_pTitleActor[1]->setPosAtNode(Vec2(-110, 400));
    m_pThemeImageViews[1]->addChild(m_pTitleActor[1],1);
    m_pTitleActor[1]->setAnchorPoint(Vec2(0.5f, 0.0f));
    m_pTitleActor[1]->setScale(0.0f);
    
    m_pScrollView->addChild(m_pThemeImageViews[2]);
    m_pThemeImageViews[2]->setAnchorPoint(Vec2::ZERO);
    m_pThemeImageViews[2]->setPosition(Vec2(margin + (m_pThemeImageViews[2]->getContentSize().width + spacing) * 2.0f, 0.0f));
    m_pThemeImageViews[2]->setUserData((void*)2);
    
    //题目gaf
    m_pTitleActor[2] = Actor::createWithGafFile(ZTXZ_3_GAF_FILE);
    m_pTitleActor[2]->setPosition(Vec2(555, 950));
    m_pTitleActor[2]->setSize(Size(750, 300));
    m_pTitleActor[2]->setPosAtNode(Vec2(-110, 400));
    m_pThemeImageViews[2]->addChild(m_pTitleActor[2],1);
    m_pTitleActor[2]->setAnchorPoint(Vec2(0.5f, 0.0f));
    m_pTitleActor[2]->setScale(0.0f);
    
    m_pScrollView->addChild(m_pThemeImageViews[3]);
    m_pThemeImageViews[3]->setAnchorPoint(Vec2::ZERO);
    m_pThemeImageViews[3]->setPosition(Vec2(margin + (m_pThemeImageViews[3]->getContentSize().width + spacing) * 3.0f, 0.0f));
    m_pThemeImageViews[3]->setUserData((void*)3);
    
    // 题目gaf
    m_pTitleActor[3] = Actor::createWithGafFile(ZTXZ_4_GAF_FILE);
    m_pTitleActor[3]->setPosition(Vec2(555, 950));
    m_pTitleActor[3]->setSize(Size(750, 300));
    m_pTitleActor[3]->setPosAtNode(Vec2(-110, 400));
    m_pThemeImageViews[3]->addChild(m_pTitleActor[3],1);
    m_pTitleActor[3]->setAnchorPoint(Vec2(0.5f, 0.0f));
    m_pTitleActor[3]->setScale(0.0f);
    
    // 放大退出按钮
    m_pReturnBtn->runAction(ScaleTo::create(0.5f, 1.0f));
    
    
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Layer*>(event->getCurrentTarget());
        if (target->getBoundingBox().containsPoint(touch->getLocation()))
        {
            m_originalPoint = target->getPosition();
            m_touchBeganPoint = touch->getLocation();
            for (unsigned int i = 0; i < 4; i++)
            {
                auto pImageButton = m_pThemeImageViews[i];
                Rect rect = Rect(0.0f, 0.0f,
                                 pImageButton->getContentSize().width,
                                 pImageButton->getContentSize().height);
                if (rect.containsPoint(pImageButton->convertTouchToNodeSpace(touch)))
                {
                    auto themeIndex = (long)pImageButton->getUserData();
                    if (themeIndex == 0) {
  
                    } else if (themeIndex == 1) {

                    } else if (themeIndex == 2)
                    {

                    }else if (themeIndex == 3)
                    {

                    }
                }
            } /*for*/
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<ImageView*>(event->getCurrentTarget());
        auto numberIndex = (long)target->getUserData();
        
        Vec2 touchEndPoint = touch->getLocation();
        m_offSetPos.x = touchEndPoint.x - m_touchBeganPoint.x;
        m_offSetPos.y = touchEndPoint.y - m_touchBeganPoint.y;
        if (m_scrollViewRect.containsPoint(touch->getLocation()))
        {
            if (std::abs((int)m_offSetPos.x) < TOUCHOFFSETX)
            {
                for (unsigned int i = 0; i < 4; i++)
                {
                    auto pImageButton = m_pThemeImageViews[i];
                    Rect rect = Rect(0.0f, 0.0f,
                                     pImageButton->getContentSize().width,
                                     pImageButton->getContentSize().height);
                    if (rect.containsPoint(pImageButton->convertTouchToNodeSpace(touch)))
                    {
                        MyAudioEngine::getInstance()->playAudio(THEME_SLIP_MUSIC_FILE);
                        auto themeIndex = (long)pImageButton->getUserData();
                        enterTheme((int)themeIndex);
                        break;
                    }
                } /*for*/
            } else {
                for (unsigned int i = 0; i < 4; i++)
                {
                    auto pImageButton = m_pThemeImageViews[i];
                    Rect rect = Rect(0.0f, 0.0f,
                                     pImageButton->getContentSize().width,
                                     pImageButton->getContentSize().height);
                    if (rect.containsPoint(pImageButton->convertTouchToNodeSpace(touch)))
                    {
                        auto themeIndex = (long)pImageButton->getUserData();
                        if (themeIndex == 0) {
                            
                        }else if (themeIndex == 1)
                        {

                        }else if (themeIndex == 2)
                        {

                        }else if (themeIndex == 3)
                        {

                        }
                    }
                } /*for*/
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pLayer);
    dispatcher->setPriority(listener, -130);
    
    m_pScrollView->setScrolledCB([this]
     {
         m_pScrollView->getContainer()->stopActionByTag(1000);
         m_pTitleActor[m_curPageIndex]->runAction(ScaleTo::create(0.2f,0.0f));
         m_pCurThemeIndexImageViews[m_curPageIndex]->loadTexture("ZTXZ-dian.png",ui::Widget::TextureResType::PLIST);
         Vec2 innerPos = m_pScrollView->getContainer()->getPosition();
         getCurThemeIndex(innerPos);
         m_pTitleActor[m_curPageIndex]->runAction(ScaleTo::create(0.2f, 1.0f));
         m_pCurThemeIndexImageViews[m_curPageIndex]->loadTexture("ZTXZ-baidian.png",ui::Widget::TextureResType::PLIST);
         if (!m_pScrollView->getBounceAnimate())
         {
             auto action = EaseBackOut::create(MoveTo::create(0.5f, Vec2(m_themePoses[m_curPageIndex].x, 0.0f)));
             action->setTag(1000);
             m_pScrollView->getContainer()->runAction(action);
         }
     });
}

void ThemeSelScene::enterTheme(const int themeIndex)
{
    if (0 == themeIndex || AppConfig::getInstance()->isStageAssetInited(themeIndex))
    {
        UserDefault::getInstance()->setIntegerForKey("curThemeIndex", themeIndex);
        UserDefault::getInstance()->flush();
        
        char szBuffer[64];
        sprintf(szBuffer, "config/themeIntroScene/theme%02d.json", themeIndex + 1);
        Director::getInstance()->replaceScene(ThemeIntroScene::createWithStageConfig(szBuffer));
    } else {
        if (NetUtil::isConnectionAvailable())
        {
            if (!NetUtil::isWIFIEnabled())
            {
                MessageBox("资源包较大，请用 wifi 下载", "提示");
            } else {
                Director::getInstance()->replaceScene(ThemeAssetInitScene::createWithThemeIndex(themeIndex));
            }
        } else {
            MessageBox("设备未联网!", "提示");
        }
    }
}

//
void ThemeSelScene::getCurThemeIndex(Vec2 pos)
{
    if ((int)pos.x > m_judgePoses[0].x) {
        m_curPageIndex = 0;
    }else if ((int)pos.x <= m_judgePoses[0].x && (int)pos.x > m_judgePoses[1].x)
    {
        m_curPageIndex = 1;
    }else if ((int)pos.x <= m_judgePoses[1].x && (int)pos.x > m_judgePoses[2].x)
    {
        m_curPageIndex = 2;
    }else if ((int)pos.x <= m_judgePoses[2].x)
    {
        m_curPageIndex = 3;
    }
}


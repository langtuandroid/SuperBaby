//
//  BuyNewThemeNode.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/5/6.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "IOSiAP_Bridge.h"
#endif

#include "BuyNewThemeNode.h"
#include "MyAudioEngine.h"
#include "LoadingStageScene.h"
#include "CustomMessage.h"
#include "MobClickCpp.h"

#define RESTORE_IMG             "res/theme/ZTXZ-yigoumai01.png"
#define RESTORE_GRAYIMG         "res/theme/ZTXZ-yigoumai02.png"
#define BUTTON_AUDIO            "res/audio/welcome/shouyeanniu.mp3"

/*
 *  constructor
 */
BuyNewThemeNode::BuyNewThemeNode() {}

/*
 *  destructor
 */
BuyNewThemeNode::~BuyNewThemeNode()
{
    auto pTextureCache = Director::getInstance()->getTextureCache();
    pTextureCache->removeTextureForKey(RESTORE_IMG);
    pTextureCache->removeTextureForKey(RESTORE_GRAYIMG);
}

//
BuyNewThemeNode* BuyNewThemeNode::createWithThemeKey(const long themeIndex)
{
    BuyNewThemeNode* ret = new BuyNewThemeNode();
    if (ret && ret->initWithThemeKey(themeIndex)) {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

//
bool BuyNewThemeNode::initWithThemeKey(const long themeIndex)
{
    m_themeIndex = themeIndex;
    
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    addChild(colorLayer);
    
    m_pBackImg = ImageView::create("ZTXZ-goumaioditu.png", ui::Widget::TextureResType::PLIST);
    
    m_pBackImg->setPosition(Vec2(size.width/2, size.height/2));
    addChild(m_pBackImg);
    m_pBackImg->cocos2d::Node::setScale(0.0f);
    
    auto bgImgSize = m_pBackImg->getContentSize();
    
    // 内购按钮
    m_pBuyButton = Button::create("ZTXZ-goumai01.png","ZTXZ-goumai02.png","ZTXZ-goumai01.png",ui::Widget::TextureResType::PLIST);
    m_pBuyButton->setAnchorPoint(Vec2(0.0f, 0.5f));
    m_pBuyButton->setPosition(Vec2(42.0f, bgImgSize.height / 2.0f - 40.0f));
    m_pBackImg->addChild(m_pBuyButton);
    m_pBuyButton->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                        {
                                            if (type == ui::Widget::TouchEventType::ENDED)
                                            {
                                                MyAudioEngine::getInstance()->playAudio(BUTTON_AUDIO);
                                                hideUI();
                                                
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                                                if (!IOSiAP_Bridge::getInstance()->checkNetwork())
                                                {
                                                    m_pOfflineImg->setVisible(true);
                                                    scheduleOnce(std::bind(&BuyNewThemeNode::restoreUI, this), 2.0f, "restoreUI");
                                                } else {
                                                    m_pIsBuyingImg->setVisible(true);
                                                    // 购买
                                                    IOSiAP_Bridge::getInstance()->setOnBuyReturn(std::bind(&BuyNewThemeNode::onBuyReturn, this, std::placeholders::_1));
                                                    IOSiAP_Bridge::getInstance()->requestProducts(2);
                                                }
#endif
                                            }
                                        });
    
    // 内购恢复按钮
    m_pRestoreBtn = Button::create(RESTORE_IMG, RESTORE_GRAYIMG, RESTORE_IMG, ui::Widget::TextureResType::LOCAL);
    m_pRestoreBtn->setAnchorPoint(Vec2(0.0f, 0.5f));
    m_pRestoreBtn->setPosition(Vec2(bgImgSize.width / 2.0f + 2.0f, bgImgSize.height / 2.0f - 40.0f));
    m_pBackImg->addChild(m_pRestoreBtn);
    m_pRestoreBtn->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                         {
                                             if (type == ui::Widget::TouchEventType::ENDED)
                                             {
                                                 MyAudioEngine::getInstance()->playAudio(BUTTON_AUDIO);
                                                 hideUI();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                                                 
                                                 if (!IOSiAP_Bridge::getInstance()->checkNetwork())
                                                 {
                                                     m_pOfflineImg->setVisible(true);
                                                     scheduleOnce(std::bind(&BuyNewThemeNode::restoreUI, this), 2.0f, "restoreUI");
                                                 } else {
                                                     m_pIsRestoreingImg->setVisible(true);
                                                     // 恢复购买
                                                     IOSiAP_Bridge::getInstance()->setOnRestoreReturn(std::bind(&BuyNewThemeNode::onRestoreReturn, this, std::placeholders::_1));
                                                     IOSiAP_Bridge::getInstance()->restore();
                                                 }
#endif
                                             }
                                         });
    
    // 试玩按钮
    m_pTryPlayBtn = Button::create("ZTXZ-shiwan01.png","ZTXZ-shiwan02.png","ZTXZ-shiwan01.png",ui::Widget::TextureResType::PLIST);
    m_pTryPlayBtn->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f - 190.0f));
    m_pBackImg->addChild(m_pTryPlayBtn);
    m_pTryPlayBtn->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                         {
                                             if (type == ui::Widget::TouchEventType::ENDED)
                                             {
                                                 m_pTryPlayBtn->setEnabled(false);
                                                 Director::getInstance()->replaceScene(LoadingStageScene::createWithStageIndex((unsigned int)m_themeIndex * 12, true));
                                             }
                                         });
    
    //
    m_pPromptImg = ImageView::create("TS-01.png", ui::Widget::TextureResType::PLIST);
    m_pPromptImg->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f + 130.0f));
    m_pBackImg->addChild(m_pPromptImg, 1);
    
    // "购买中"标题
    m_pIsBuyingImg = ImageView::create("TS-05.png", ui::Widget::TextureResType::PLIST);
    m_pIsBuyingImg->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f));
    m_pIsBuyingImg->setVisible(false);
    m_pBackImg->addChild(m_pIsBuyingImg, 1);
    
    // “恢复中”标题
    m_pIsRestoreingImg = ImageView::create("TS-04.png", ui::Widget::TextureResType::PLIST);
    m_pIsRestoreingImg->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f));
    m_pIsRestoreingImg->setVisible(false);
    m_pBackImg->addChild(m_pIsRestoreingImg, 1);
    
    // 网络不给力
    m_pOfflineImg = ImageView::create("TS-06.png", ui::Widget::TextureResType::PLIST);
    m_pOfflineImg->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f));
    m_pOfflineImg->setVisible(false);
    m_pBackImg->addChild(m_pOfflineImg, 1);
    
    if (m_themeIndex != 1)
    {
        m_pPromptImg->setVisible(false);
        
        // “恢复中”标题
        auto qidaiImg = ImageView::create("TS-02.png", ui::Widget::TextureResType::PLIST);
        qidaiImg->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f + 90.0f));
        m_pBackImg->addChild(qidaiImg, 1);
        
        m_pBuyButton->setPosition(Vec2(0.0f, -10000.0f));
        m_pRestoreBtn->setPosition(Vec2(0.0f, -10000.0f));
        m_pTryPlayBtn->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f - 60.0f));
    }
    
    m_closeBtn = Button::create("ZTXZ-guanbi01.png","ZTXZ-guanbi02.png","ZTXZ-guanbi01.png",ui::Widget::TextureResType::PLIST);
    auto closeSize = m_closeBtn->getContentSize();
    m_closeBtn->setPosition(Vec2(bgImgSize.width - closeSize.width / 2.0f + 20.0f,
                                 bgImgSize.height - closeSize.height / 2.0f + 16.0f));
    m_pBackImg->addChild(m_closeBtn);
    m_closeBtn->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type)
                                      {
                                          if (type == ui::Widget::TouchEventType::ENDED)
                                          {
                                              // 关闭
                                              m_pBackImg->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]
                                                                                                                                                                                                 {
                                                                                                                                                                                                     this->removeFromParent();
                                                                                                                                                                                                 }), NULL));
                                          }
                                      });
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    dispatcher->setPriority(listener, -130);
    m_pBackImg->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)));
    return true;
}

/*
 *  购买返回
 */
void BuyNewThemeNode::onBuyReturn(bool isBuySuccessed)
{
    m_pIsBuyingImg->setVisible(false);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSiAP_Bridge::getInstance()->setOnBuyReturn(nullptr);
#endif
    
    if (isBuySuccessed)
    {
        // 友盟游戏购买统计
        umeng::MobClickCpp::pay(6.0f, 1, "神秘星球", 1, 6.0f);
        
        auto dispatcher = Director::getInstance()->getEventDispatcher();
//        dispatcher->dispatchCustomEvent(UPDATE_THEME_LOCK);
        this->removeFromParent();
    } else {
        restoreUI();
    }
}

//
void BuyNewThemeNode::onRestoreReturn(bool isRestored)
{
    m_pIsRestoreingImg->setVisible(false);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IOSiAP_Bridge::getInstance()->setOnRestoreReturn(nullptr);
#endif
    if (isRestored)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
//        dispatcher->dispatchCustomEvent(UPDATE_THEME_LOCK);
        this->removeFromParent();
    } else {
        restoreUI();
    }
}

//
void BuyNewThemeNode::hideUI()
{
    m_pPromptImg->setVisible(false);
    m_closeBtn->setPosition(Vec2(0.0f, -10000.0f));
    m_pBuyButton->setPosition(Vec2(0.0f, -10000.0f));
    m_pRestoreBtn->setPosition(Vec2(0.0f, -10000.0f));
    m_pTryPlayBtn->setPosition(Vec2(0.0f, -10000.0f));
}

//
void BuyNewThemeNode::restoreUI()
{
    m_pOfflineImg->setVisible(false);
    m_pPromptImg->setVisible(true);
    
    auto bgImgSize = m_pBackImg->getContentSize();
    auto closeSize = m_closeBtn->getContentSize();
    
    m_closeBtn->setPosition(Vec2(bgImgSize.width - closeSize.width / 2.0f + 20.0f,
                                 bgImgSize.height - closeSize.height / 2.0f + 16.0f));
    m_pRestoreBtn->setPosition(Vec2(bgImgSize.width / 2.0f + 2.0f, bgImgSize.height / 2.0f - 40.0f));
    
    m_pBuyButton->setPosition(Vec2(42.0f, bgImgSize.height / 2.0f - 40.0f));
    m_pPromptImg->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f + 130.0f));
    m_pTryPlayBtn->setPosition(Vec2(bgImgSize.width / 2.0f, bgImgSize.height / 2.0f - 190.0f));
}


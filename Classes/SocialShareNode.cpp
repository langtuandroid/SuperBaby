//
//  SocialShareNode.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/26.
//
//

#include "SocialShareNode.h"
#include "Actor.h"
#include "RoundUINode.h"
#include "ui/CocosGUI.h"
#include "MyAudioEngine.h"
#include "SocialUtils.h"
#include "StageDataManager.h"
#include "AppConfig.h"

using namespace ui;

#define SHARE_GAF_FILE      "res/common/chenggongdonghua/chenggongdonghua.gaf"
#define OK_PNG_FILE         "res/common/NPC-shoujiOKanniu.png"
#define SHARE_PNG_FILE      "res/common/chenggong-fenxiang.png"
#define PLATFORM_SHARE_PLIST_FILE           "res/common/fengxiangtuankaung.plist"
#define PLATFORM_SHARE_PNG_FILE             "res/common/fengxiangtuankaung.png"
#define BTN_CLICK_MUSIC_FILE                "res/audio/ui/fengxiangdianji.mp3"

/*
 *  constructor
 */
SocialShareNode::SocialShareNode(){}

/*
 *  destructor
 */
SocialShareNode::~SocialShareNode()
{
    SpriteFrameCache::getInstance()->removeSpriteFramesFromFile(PLATFORM_SHARE_PLIST_FILE);
}

//
bool SocialShareNode::init()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(PLATFORM_SHARE_PLIST_FILE);
    
    auto size = Director::getInstance()->getVisibleSize();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    this->addChild(colorLayer);
    
    m_pLayer = Layer::create();
    addChild(m_pLayer);
    m_pLayer->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pLayer->setPosition(Vec2(0, 0));
    m_pLayer->setScale(0.0f);
    
    m_pActor = Actor::createWithGafFile(SHARE_GAF_FILE);
    m_pLayer->addChild(m_pActor, 1);
    m_pActor->setPosition(Vec2(330.0f, 1600.0f));
    
    // ok 按钮
    auto btnOk = ui::Button::create("NPC-shoujiOKanniu01.png","","",ui::Widget::TextureResType::PLIST);
    btnOk->setPosition(Vec2(1050.0f, 390.0f));
    m_pLayer->addChild(btnOk, 2);
    btnOk->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
        if (ui::Widget::TouchEventType::ENDED == type) {
            MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
            if (m_okCB) {
                m_okCB();
            }
        }
    });
    btnOk->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.7f, 0.9f),ScaleTo::create(0.7f, 1.0f), NULL)));
    // share 按钮
    auto btnShare = ui::Button::create("chenggong-fenxiang01.png","","",ui::Widget::TextureResType::PLIST);
    btnShare->setPosition(Vec2(1050.0f, 590.0f));
    m_pLayer->addChild(btnShare, 2);
    btnShare->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
        if (ui::Widget::TouchEventType::ENDED == type) {
            MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
            m_pLayer->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
                m_pLayer->removeFromParent();
                createPlatformShareView();
            }), NULL));
            
        }
    });
    btnShare->runAction(RepeatForever::create(Sequence::create(ScaleTo::create(0.8f, 0.9f),ScaleTo::create(0.8f, 1.0f), NULL)));
    return true;
}

void SocialShareNode::setScaleShareNode()
{
    m_pLayer->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)));
}

void SocialShareNode::createPlatformShareView()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    m_pShareBackGround = Sprite::createWithSpriteFrameName("fenxiangdaodibu.png");
    m_pShareBackGround->setPosition(Vec2(dw/2, size.height/2));
    addChild(m_pShareBackGround,1);
    m_pShareBackGround->setScale(0.0f);
    
    auto closeBtn = Button::create("ZTXZ-guanbi01.png","","",ui::Widget::TextureResType::PLIST);
    closeBtn->setPosition(Vec2(850,570));
    m_pShareBackGround->addChild(closeBtn, 2);
    closeBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type){
        if (ui::Widget::TouchEventType::ENDED == type) {
            MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
            //关闭分享
            if (m_closeCB) {
                m_closeCB();
            }
        }
    });
    
    const float socialBtnTopY = 308.0f;
    auto wxSessionBtn = Button::create("weixin01.png","","",ui::Widget::TextureResType::PLIST);
    wxSessionBtn->setPosition(Vec2(250.0f, socialBtnTopY));
    m_pShareBackGround->addChild(wxSessionBtn, 2);
    wxSessionBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
    {
         if (ui::Widget::TouchEventType::ENDED == type)
         {
             MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
             std::string text = StageDataManager::getInstance()->getSocialShareText(m_stageIndex);
             SocialUtils::shareToWXSession(text);
         }
    });
    
    auto wxTimelineBtn = Button::create("pengyouquan01.png","","",
                                           ui::Widget::TextureResType::PLIST);
    wxTimelineBtn->setPosition(Vec2(404.0f, socialBtnTopY));
    m_pShareBackGround->addChild(wxTimelineBtn, 2);
    wxTimelineBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (ui::Widget::TouchEventType::ENDED == type)
        {
            MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
            std::string text = StageDataManager::getInstance()->getSocialShareText(m_stageIndex);
            SocialUtils::shareToWXTimeline(text);
        }
    });
    
    auto weiBoBtn = Button::create("weibo01.png","","",ui::Widget::TextureResType::PLIST);
    weiBoBtn->setPosition(Vec2(558.0f, socialBtnTopY));
    //    weiBoBtn->setColor(Color3B(153, 153, 153));
    m_pShareBackGround->addChild(weiBoBtn, 2);
    weiBoBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (ui::Widget::TouchEventType::ENDED == type)
        {
            MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
            // 分享到微博
            std::string text = StageDataManager::getInstance()->getSocialShareText(m_stageIndex);
            SocialUtils::shareToWeibo("超能宝贝", text.c_str());
        }
    });
    
    auto QQBtn = Button::create("QQ01.png","","",ui::Widget::TextureResType::PLIST);
    QQBtn->setPosition(Vec2(712.0f, socialBtnTopY));
    m_pShareBackGround->addChild(QQBtn, 2);
    QQBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
    {
        if (ui::Widget::TouchEventType::ENDED == type)
        {
            MyAudioEngine::getInstance()->playAudio(BTN_CLICK_MUSIC_FILE);
            // 分享到 QQ
            std::string text = StageDataManager::getInstance()->getSocialShareText(m_stageIndex);
//             SocialUtils::shareToQQ("超能宝贝", text.c_str());
            SocialUtils::shareToQQZone("超能宝贝", text.c_str());
        }
    });
    m_pShareBackGround->runAction(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)));
}


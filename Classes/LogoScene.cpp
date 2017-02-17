//
//  LogoScene.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/08.
//
//

#include "LogoScene.h"
#include "commonFunc.h"
#include "Actor.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "WelcomeScene.h"
#include "LoadingNode.h"
#include "MyAudioEngine.h"

#define LOGO_GAF_FILE       "res/common/logodonghua/logodonghua.gaf"
#define STORY_GAF_FILE      "res/common/dh-dagushi0508/dh-dagushi0508.gaf"
#define STORY_AUDIO_FILE    "res/common/dh-dagushi.mp3"

/*
 *  constructor
 */
LogoScene::LogoScene()
{
    m_dWidth = AppConfig::getInstance()->getDesignResolutionWidth();
}

/*
 *  desstructor
 */
LogoScene::~LogoScene() {}

//
bool LogoScene::init()
{
    addLrMask(this);
    playLogo();
    return true;
}

//
void LogoScene::playLogo()
{
    auto size = Director::getInstance()->getVisibleSize();
    m_pLogoActor = Actor::createWithGafFile(LOGO_GAF_FILE, false);
    m_pLogoActor->setAnimationFinishedPlayDelegate(std::bind(&LogoScene::onLogoCompleted, this));
    m_pLogoActor->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    m_pLogoActor->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
    m_pLogoActor->setSize(Size(m_dWidth, size.height));
    m_pLogoActor->setPosAtNode(Vec2(2.0f, 1535.0f));
    addChild(m_pLogoActor, 1);
}

//
void LogoScene::onLogoCompleted()
{
    m_pLogoActor->setPosition(Vec2(0.0f, -5000.0f));
    
    auto sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
    m_pLoadingNode = LoadingNode::create();
    m_pLoadingNode->setPosition(Vec2(sceneOffset, 0.0f));
    addChild(m_pLoadingNode, 100);
    
    auto cb = [this] (GAFAsset* pGAFAsset)
    {
        m_pLogoActor->removeFromParent();m_pLogoActor = nullptr;
        GafAssetCache::getInstance()->removeGafAssetForKey(LOGO_GAF_FILE);
        
        if (m_pLoadingNode != nullptr)
        {
            m_pLoadingNode->removeFromParent();
            m_pLoadingNode = nullptr;
        }
        
        auto size = Director::getInstance()->getVisibleSize();
        m_pStoryActor = Actor::createWithGafFile(STORY_GAF_FILE, false, 2.0f, Vec2(2.0f, 1535.0f));
        m_pStoryActor->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        m_pStoryActor->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
        m_pStoryActor->setSize(Size(m_dWidth, size.height));
        addChild(m_pStoryActor, 2);
        m_pStoryActor->setAnimationFinishedPlayDelegate(std::bind(&LogoScene::onStoryCompleted, this));
        MyAudioEngine::getInstance()->playAudio(STORY_AUDIO_FILE,false,true);
    };
    GafAssetCache::getInstance()->addGafAssetAsync(STORY_GAF_FILE, cb);
    
    //
    AppConfig::createAssetPath();
    
    auto eventListenForEnterWelcome = EventListenerTouchOneByOne::create();
    eventListenForEnterWelcome->onTouchBegan = [this] (Touch* pTouch, Event *pEvent) ->bool
    {
        return true;
    };
    eventListenForEnterWelcome->onTouchEnded = [this](Touch* pTouch, Event* pEvent)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
        MyAudioEngine::getInstance()->stopAudio(0,true);
        onStoryCompleted();
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListenForEnterWelcome, this);
}

//
void LogoScene::onStoryCompleted()
{
    auto cb = [this](float dt)
    {
        m_pStoryActor->removeFromParent();
        GafAssetCache::getInstance()->removeGafAssetForKey(STORY_GAF_FILE);
        Director::getInstance()->replaceScene(WelcomeScene::createWithFirst(true));
    };
    scheduleOnce(cb, 0.2f, "enterWelcome");
}


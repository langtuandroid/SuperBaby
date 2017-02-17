//
//  ThemeIntroScene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/05/11.
//
//

#include "ThemeIntroScene.h"
#include "SelStageScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "MyAudioEngine.h"
#include "AppConfig.h"

#define BG_LEFT_IMAGE               "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE              "res/common/bgRight.png"

/*
 *  constructor
 */
ThemeIntroScene::ThemeIntroScene() {
    m_sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
}

/*
 *  desstructor
 */
ThemeIntroScene::~ThemeIntroScene() {}

//
Scene* ThemeIntroScene::createWithStageConfig(const std::string& stageConfigFile)
{
    ThemeIntroScene* pScene = new ThemeIntroScene();
    if (pScene && pScene->initWithStageConfig(stageConfigFile))
    {
        pScene->autorelease();
        return pScene;
    } else {
        CC_SAFE_DELETE(pScene);
        return nullptr;
    }
}

//
bool ThemeIntroScene::initWithStageConfig(const std::string& stageConfigFile)
{
    MyAudioEngine::getInstance()->pauseAudio(0,true);
    
    std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(stageConfigFile);
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    
    m_themeIndex = doc["themeIndex"].GetDouble();
    m_videoFile = doc["video"].GetString();
    
    return true;
}
//
void ThemeIntroScene::onEnter()
{
    Scene::onEnter();
    playVideo();
}

//
void ThemeIntroScene::onExit()
{
    Scene::onExit();
}

void ThemeIntroScene::playVideo()
{
    Size size = Director::getInstance()->getVisibleSize();
    // 左右遮罩
    if (m_sceneOffset > 0.0f)
    {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        
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
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    m_pVideoPlay = cocos2d::experimental::ui::VideoPlayer::create();
    m_pVideoPlay->setPosition(Vec2(size.width / 2, size.height / 2));
    m_pVideoPlay->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    auto width = AppConfig::getInstance()->getDesignResolutionWidth();
    m_pVideoPlay->setContentSize(Size(width , size.height));
    addChild(m_pVideoPlay);
    if (m_pVideoPlay)
    {
        m_pVideoPlay->setFileName(m_videoFile);
        m_pVideoPlay->addEventListener(CC_CALLBACK_2(ThemeIntroScene::videoEventCallback, this));
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
        {
            return true;
        };
        listener->onTouchEnded = [this](Touch* touch, Event* event)
        {
            if (m_pVideoPlay) {
                m_pVideoPlay->stop();
            }
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pVideoPlay);
            replaceNextScene();
        };
        dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pVideoPlay);
#endif
        //
        m_pVideoPlay->play();
    }
#endif
}

void ThemeIntroScene::replaceNextScene()
{
    MyAudioEngine::getInstance()->resumeAudio(0,true);
    char szBuffer[64];
    sprintf(szBuffer, "config/selStage/selStage%03ld.json", m_themeIndex + 1);
    Director::getInstance()->replaceScene(SelStageScene::createWithStageConfig(szBuffer));
}

/**
 * 视频播放完成的回调函数
 */
void ThemeIntroScene::videoPlayOverCallback()
{
    char szBuffer[64];
    sprintf(szBuffer, "config/selStage/selStage%03ld.json", m_themeIndex + 1);
    Director::getInstance()->replaceScene(SelStageScene::createWithStageConfig(szBuffer));
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void ThemeIntroScene::videoEventCallback(Ref* sender,
                                         cocos2d::experimental::ui::VideoPlayer::EventType eventType)
{
    switch (eventType)
    {
        case cocos2d::experimental::ui::VideoPlayer::EventType::PLAYING:
            break;
        case cocos2d::experimental::ui::VideoPlayer::EventType::PAUSED:
            m_pVideoPlay->stop();
            videoPlayOverCallback();
            break;
        case cocos2d::experimental::ui::VideoPlayer::EventType::STOPPED:
            break;
        case cocos2d::experimental::ui::VideoPlayer::EventType::COMPLETED:
            videoPlayOverCallback();
            break;
        default:
            break;  
    }  
}  
#endif

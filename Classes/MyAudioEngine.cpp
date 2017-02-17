//
//  MyAudioEngine.cpp
//  superBaby
//
//  Created by Administrator on 15/6/15.
//
//

#include "MyAudioEngine.h"
USING_NS_CC;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #include <SimpleAudioEngine.h>
    using namespace CocosDenshion;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include <AudioEngine.h>
    using namespace experimental;
#endif

// 单例对象
static MyAudioEngine* pMyAudioEngine = nullptr;

/*
 *    获取单例对象
 */
MyAudioEngine* MyAudioEngine::getInstance()
{
    if (!pMyAudioEngine) {
        pMyAudioEngine = new MyAudioEngine();
    }
    return pMyAudioEngine;
}

/*
 *    释放单例对象内存
 */
void MyAudioEngine::destroyInstance()
{
    CC_SAFE_DELETE(pMyAudioEngine);
}

/*
 *  constructor
 */
MyAudioEngine::MyAudioEngine() {}

/*
 *  destructor
 */
MyAudioEngine::~MyAudioEngine()
{
    
}

unsigned int MyAudioEngine::playAudio(const char *filePath, bool loop, bool isBackMusic, float gain, float pitch, float pan, const cocos2d::experimental::AudioProfile *profile)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (isBackMusic) {
        SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath,loop);
        SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(gain);
    } else {
        m_musicId = SimpleAudioEngine::getInstance()->playEffect(filePath,loop,pitch,pan,gain);
        return m_musicId;
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (isBackMusic) {
        AudioEngine::lazyInit();
        stopAudio(m_musicId);
        m_musicId = AudioEngine::play2d(filePath,loop,gain,profile);
        return m_musicId;
    }else
    {
        AudioEngine::lazyInit();
        return AudioEngine::play2d(filePath,loop,gain,profile);
    }

#endif
    return 0;
}

//停止所有声音
void MyAudioEngine::stopAllAudio()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->stopAllEffects();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AudioEngine::stopAll();
#endif
}
//停止某个声音
void MyAudioEngine::stopAudio(int audioID, bool isBackMusic)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (isBackMusic) {
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }else
    {
        SimpleAudioEngine::getInstance()->stopEffect(audioID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (audioID != -1) {
        if (isBackMusic) {
            AudioEngine::stop(m_musicId);
        }else
        {
            AudioEngine::stop(audioID);
        }
        
    }
#endif
}
//暂停某个声音
void MyAudioEngine::pauseAudio(int audioID, bool isBackMusic)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (isBackMusic) {
        SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    }else
    {
        SimpleAudioEngine::getInstance()->pauseEffect(audioID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (isBackMusic) {
        AudioEngine::pause(m_musicId);
    }else
    {
        AudioEngine::pause(audioID);
    }
    
#endif
}
//暂停所有声音
void MyAudioEngine::pauseAllAudio()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AudioEngine::pauseAll();
#endif
}

void MyAudioEngine::resumeAllAudio()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SimpleAudioEngine::getInstance()->resumeAllEffects();
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AudioEngine::resumeAll();
#endif
}

void MyAudioEngine::resumeAudio(int audioID, bool isBackMusic)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    if (isBackMusic) {
        SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    }else
    {
        SimpleAudioEngine::getInstance()->resumeEffect(audioID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (isBackMusic) {
        AudioEngine::resume(m_musicId);
    }else
    {
        AudioEngine::resume(audioID);
    }
    
#endif
}

void MyAudioEngine::unloadAudio(const std::string& filePath)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SimpleAudioEngine::getInstance()->unloadEffect(filePath.c_str());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AudioEngine::uncache(filePath);
#endif
    
}

//背景声音是否正在播放
bool MyAudioEngine::isPlaying()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (cocos2d::experimental::AudioEngine::getState(m_musicId) == cocos2d::experimental::AudioEngine::AudioState::PLAYING) {    //1表示正在播放PLAYING
        return true;
    }else
    {
        return false;
    }
#endif
}

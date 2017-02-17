//
//  MyAudioEngine.h
//  superBaby
//
//  Created by Administrator on 15/6/15.
//
//

#ifndef __superBaby__MyAudioEngine__
#define __superBaby__MyAudioEngine__

#include <cocos2d.h>

namespace cocos2d {
    namespace experimental
    {
        class AudioProfile;
    }
}

class MyAudioEngine
{
public:
    MyAudioEngine();
    ~MyAudioEngine();
    
public:
    static MyAudioEngine* getInstance();
    static void destroyInstance();
    
public:
    //播放声音（包括背景音乐和音效）
    unsigned int playAudio(const char* filePath, bool loop = false, bool isBackMusic = false, float gain = 1.0f, float pitch = 1.0f, float pan = 0.0f, const cocos2d::experimental::AudioProfile *profile = nullptr);
    //停止所有声音
    void stopAllAudio();
    //停止某个声音
    void stopAudio(int audioID = -1, bool isBackMusic = false);
    //暂停某个声音
    void pauseAudio(int audioID = -1, bool isBackMusic = false);
    //暂停所有声音
    void pauseAllAudio();
    //恢复所有声音
    void resumeAllAudio();
    //恢复某个声音
    void resumeAudio(int audioID = -1, bool isBackMusic = false);
    //删除某个声音缓存
    void unloadAudio(const std::string& filePath);
    //背景声音是否正在播放
    bool isPlaying();
    
    int getMusicId(){return m_musicId;}

private:
    int m_musicId = -1;
};

#endif /* defined(__superBaby__MyAudioEngine__) */

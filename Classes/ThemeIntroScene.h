//
//  ThemeIntroScene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/05/11.
//
//

#ifndef __AbelLLExplorer__ThemeIntroScene__
#define __AbelLLExplorer__ThemeIntroScene__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class ThemeIntroScene : public Scene
{
public:
    ThemeIntroScene();
    virtual ~ThemeIntroScene();
    
    static Scene* createWithStageConfig(const std::string&);
    bool initWithStageConfig(const std::string&);
    
    void replaceNextScene();
    
public:
    virtual void onEnter();
    virtual void onExit();
    
    void videoPlayOverCallback();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    void videoEventCallback(Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType);
#endif
    
    void playVideo();
    
private:
    unsigned long m_themeIndex = 0;
    std::string m_videoFile = "";
    cocos2d::experimental::ui::VideoPlayer* m_pVideoPlay = nullptr;
    float m_sceneOffset = 0.0f;
};

#endif /* defined(__AbelLLExplorer__ThemeIntroScene__) */

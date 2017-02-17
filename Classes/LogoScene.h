//
//  LogoScene.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/05/08.
//
//

#ifndef __AbelLLExplorer__LogoScene__
#define __AbelLLExplorer__LogoScene__

#include "cocos2d.h"

USING_NS_CC;

class Actor;
class LogoScene : public Scene
{
public:
    LogoScene();
    virtual ~LogoScene();
    
public:
    CREATE_FUNC(LogoScene);
    bool init();
    
private:
    void playLogo();
    void playStory();
    
    void onLogoCompleted();
    void onStoryCompleted();
    
private:
    float m_dWidth = 0.0f;
    
    Actor* m_pLogoActor = nullptr;
    Actor* m_pStoryActor = nullptr;
    
    Node* m_pLoadingNode = nullptr;
};

#endif /* defined(__AbelLLExplorer__LogoScene__) */

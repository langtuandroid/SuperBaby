//
//  ThemeAssetInitScene.h
//  superBaby
//
//  Created by adinnet on 2015/08/13.
//
//

#ifndef __superBaby__ThemeAssetInitScene__
#define __superBaby__ThemeAssetInitScene__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

class Actor;
class ThemeAssetInitScene : public cocos2d::Scene
{
public:
    ThemeAssetInitScene();
    ~ThemeAssetInitScene();

    static Scene* createWithThemeIndex(const int);
    bool initWithThemeIndex(const int);
    
public:
    virtual void onEnter();
    virtual void onExit();
    
    void eventListenAssetsManagerCB(cocos2d::extension::EventAssetsManagerEx*);
    void destroyAssetsManager();

private:
    void startDownLoadStageRes();
    
private:
    int m_themeIndex = 0;
    cocos2d::Label* m_pHintLabel = nullptr;
    
    // 下载进度条动画
    static int m_actorIndex;
    std::string m_actorFile[4];
    cocos2d::Vec2 m_actorNodePos[4];
    cocos2d::Size m_actorSize[4];
    std::string m_currGafFile = "";
    
    cocos2d::extension::AssetsManagerEx* _am = nullptr;
    cocos2d::extension::EventListenerAssetsManagerEx* _amListener = nullptr;
};

#endif /* defined(__superBaby__ThemeAssetInitScene__) */

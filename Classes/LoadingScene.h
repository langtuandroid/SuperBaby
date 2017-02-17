//
//  LoadingScene.h
//  superBaby
//
//  Created by Administrator on 15/7/10.
//
//

#ifndef __superBaby__LoadingScene__
#define __superBaby__LoadingScene__

#include "cocos2d.h"

class LoadingScene : public cocos2d::Scene
{
public:
    LoadingScene();
    ~LoadingScene();
    
public:
    static cocos2d::Scene* createWithIndex(const unsigned int);
    bool initWithIndex(const unsigned int);
private:
    /* 进入关卡 */
    void enterStage();
    
    virtual void onEnter();
    
    void update(float);
    
private:
    
    std::vector<std::string> m_imgs;
    
    std::vector<std::string> m_gafs;
    
    unsigned long m_loadedBgs = 0;
    
    unsigned long m_preloadImgs = 0;
    unsigned long m_loadedImgs = 0;
    unsigned long m_preloadGafs = 0;
    unsigned long m_loadedGafs = 0;
    
    unsigned int m_index = 0;
    
    unsigned int m_willLoadedImg = 0;
    unsigned int m_willLoadedGaf = 0;
    bool m_isLoadedCompleteImg = false;
    bool m_isLoadedCompleteGaf = false;
};

#endif /* defined(__superBaby__LoadingScene__) */

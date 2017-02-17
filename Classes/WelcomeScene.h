//
//  WelcomeScene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/19.
//
//  欢迎界面
//

#ifndef __AbelLLExplorer__WelcomeScene__
#define __AbelLLExplorer__WelcomeScene__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
namespace cocos2d {
    namespace ui {
        class Button;
    }
}

USING_NS_CC;
using namespace ui;

class Actor;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
class AndroidRollBackListener;
#endif
class WelcomeScene : public Scene
{
public:
    WelcomeScene();
    virtual ~WelcomeScene();
    
    static WelcomeScene* createWithFirst(const bool first = true);
    bool initWithFirst(const bool);
    
public:
    virtual void onEnter();
    virtual void onExit();
    
    // enter 按钮动作
    void enterBtnAction();
    
    // 背包按钮动作
    void bagBtnActions();
    
    // 播放按钮动作
    void playBtnActions();
    
    // 加载投影 gaf
    void addTouyingActor();
    
    // 播放艾米介绍音效
    void playEffectAimi();
    // 播放蓓蓓介绍音效
    void playEffectBeibei();
    
private:
    void initUi();
    void moveOutUi();
    void restoreUi();
private:
    Actor* m_pbeiBeiActor = nullptr;                // 蓓蓓
    Actor* m_paiMiActor = nullptr;                  // 艾米
    Actor* m_ptitleActor = nullptr;                 // 标题
    Actor* m_ptouyingActor = nullptr;               // 投影
    Actor* m_pBagActor = nullptr;                   // 背包
    Sprite* m_pbackground = nullptr;                // 背景
    Button* m_pEnterBtn = nullptr;                  // 进入按钮
    Button* m_pPlayBtn= nullptr;                    // 播放按钮
    ImageView* m_pplayBack = nullptr;               // 放映状态的按钮背景图
    LayerColor* m_playerColor = nullptr;            // 颜色蒙版层
    bool m_isplayActor = false;                     // 是否正在播放动画
    int m_effectId = 0;                             // 音效play的id
    bool m_first = false;                           // 是否第一次进入
    bool m_isOpenBag = false;                       // 用于判断是否在没有播放背景音乐之前打开背包界面
    
    bool m_touyingResReady = false;                 // 投影相关资源是否加载完毕
    int m_touyingResLoaded = 0;
    
    float m_sceneOffset = 0.0f;
    
    Vec2 m_bagPos = Vec2::ZERO;
    Vec2 m_playBtnPos = Vec2::ZERO;
    
    Node* m_pContainerNode = nullptr;
    
    friend class AndroidRollBackListener;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AndroidRollBackListener* m_androidRollBackListener = nullptr;
#endif
};

#endif /* defined(__AbelLLExplorer__WelcomeScene__) */

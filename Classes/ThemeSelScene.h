//
//  ThemeSelScene.h
//  AbelLLExplorer
//
//  Created by YANGJIE on 2015/04/19.
//
//  主题选择
//

#ifndef __AbelLLExplorer__ThemeSelScene__
#define __AbelLLExplorer__ThemeSelScene__

#include <cocos2d.h>
#include "ui/CocosGUI.h"
#include <extensions/cocos-ext.h>

USING_NS_CC;
using namespace ui;

class Actor;
class MyScrollView;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
class AndroidRollBackListener;
#endif

class ThemeSelScene : public Scene
{
public:
    ThemeSelScene();
    virtual ~ThemeSelScene();
    
    virtual void onEnter();
    
public:
    bool init();
    CREATE_FUNC(ThemeSelScene);
    
public:
    void getCurThemeIndex(Vec2);
    void enterTheme(const int);
    
private:
    void initUI();
    
private:
    Vec2 m_originalPoint;           // 原来的位置
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    ImageView* m_pThemeImageViews[4];       //主题image
    ImageView* m_pCurThemeIndexImageViews[4];  //当前所在主题显示image
    MyScrollView* m_pScrollView = nullptr;
    Actor* m_pTitleActor[4];                //主题题目gaf
    Vec2 m_offSetPos;
    Vec2 m_themePoses[4];
    Vec2 m_judgePoses[3];
    unsigned int m_curPageIndex = 0;
    
    Rect m_scrollViewRect;
    
    Button* m_pReturnBtn;
    
    int m_numberOfLoaded = 0;
    
    float m_sceneOffset = 0.0f;
    Node* m_pLoadingNode = nullptr;
    Vec2 m_lumpsPos[6];
    Layer* m_pLayer = nullptr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AndroidRollBackListener* m_androidRollBackListener = nullptr;
#endif
};

#endif /* defined(__AbelLLExplorer__ThemeSelScene__) */

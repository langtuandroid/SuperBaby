//
//  NpcBagNode.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/04/15.
//
//

#ifndef __AbelLLExplorer__NpcBagNode__
#define __AbelLLExplorer__NpcBagNode__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Actor.h"
#include <vector>

USING_NS_CC;

class NpcDataManager;
class GafAssetCache;
class NpcData;
class NpcBagNode : public Node
{
public:
    NpcBagNode();
    virtual ~NpcBagNode();
    
    bool init();
    CREATE_FUNC(NpcBagNode);
    
public:
    
    /* 更新 npc ui 显示 */
    void updateNpcUI();
    
    //暂停触摸
    void pauseTouch();
    //恢复触摸
    void resumeTouch();
    
    /* 初始化scrollview */
    void initScrollView(const unsigned long, const bool isEnter = true);
    
    /* 清除scrollView */
    void clearScrollView(const unsigned long);
    
    //创建loading节点
    void createLoadingNode();
    
    void createThemeItems();
    
    void loadNpcRes(const unsigned long, const bool isEnter = true);
    
    void showLoadingLayer(const unsigned long);
    
    void hideLoadingLayer(const unsigned long);

private:
    void initUI();
    
private:
    Sprite* m_pbackGround = nullptr;            // 背景框
    Sprite* m_pbasePng = nullptr;               // 背景底图
    ui::ScrollView* m_pScrollview = nullptr;    // 滚动层
    ui::Button* m_pReturnBtn = nullptr;

    NpcDataManager* m_pNpcDataManager = nullptr;
    GafAssetCache* m_pGafAssetCache = nullptr;
    
    std::vector<Sprite*> m_npcBgSps;
    Vec2 m_originalPoint;           // 原来的位置
    Vec2 m_touchBeganPoint;         // 拖放开始位置
//    std::vector<std::string> m_npcName;
    std::vector<NpcData*> m_npcData;
    int m_textureLoaded = 0;
    int m_gafLoaed = 0;
    
    Node* m_pLoadingNode = nullptr;
    
    ui::ImageView* m_pThemeImageView[4];
    
    Rect m_npcTouchRect;
};

#endif /* defined(__AbelLLExplorer__NpcBagNode__) */

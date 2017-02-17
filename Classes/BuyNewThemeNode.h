//
//  BuyNewThemeNode.h
//  AbelLLExplorer
//
//  Created by Administrator on 2015/5/6.
//
//

#ifndef __AbelLLExplorer__BuyNewThemeNode__
#define __AbelLLExplorer__BuyNewThemeNode__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <vector>

USING_NS_CC;
using namespace ui;

class RoundUINode;
class NpcBagNode;
class BuyNewThemeNode : public Node
{
public:
    BuyNewThemeNode();
    virtual ~BuyNewThemeNode();
    
public:
    static BuyNewThemeNode* createWithThemeKey(const long);
    bool initWithThemeKey(const long);

private:
    void onBuyReturn(bool);
    void onRestoreReturn(bool);
    
    void hideUI();
    void restoreUI();
    
private:
    ImageView* m_pBackImg = nullptr;
    Button* m_pBuyButton = nullptr;
    Button* m_pRestoreBtn = nullptr;
    Button* m_pTryPlayBtn = nullptr;
    Button* m_closeBtn = nullptr;
    
    // 提示
    ImageView* m_pPromptImg = nullptr;
    
    // “购买中“标题
    ImageView* m_pIsBuyingImg = nullptr;
    // “恢复中“标题
    ImageView* m_pIsRestoreingImg = nullptr;
    // “网络故障“标题
    ImageView* m_pOfflineImg = nullptr;
    //
    ImageView* m_pNeedBuyImg = nullptr;
    
    std::vector<std::string> m_removeTextures;
    long m_themeIndex = -1;
};

#endif /* defined(__AbelLLExplorer__BuyNewThemeNode__) */

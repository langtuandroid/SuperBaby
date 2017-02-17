//
//  UIBase.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/16.
//
//

#ifndef __AbelLLExplorer__UIBase__
#define __AbelLLExplorer__UIBase__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class NpcCollectCompleteNode;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
class AndroidRollBackListener;
#endif

class UIBase:public Node
{
    
    CC_SYNTHESIZE(unsigned int, m_curLevel, curLevel);
public:
    UIBase();
    virtual ~UIBase();
    
    virtual void onEnter();
    
    static UIBase* create(unsigned int, const bool isSceneOffset = false);
    
    /* 初始化 ui 层 */
    //两个参数，第一个参数用于区分具体界面返回上层界面的区别,0表示从关卡选择返回主题选择，1表示从回合返回关卡选择
    virtual bool initUILayer(const unsigned int, const bool isSceneOffset);
    
    void updateMusicOpenBtn();   //更新背景音乐开关按钮状态
    //改变背包gaf动作
    void changeBagGafActor(unsigned int);
    
    // 获取 npc 背包位置
    Vec2 getNpcBagPos();
    
    //添加npc收集展示界面
    void addCollectNpcNode();
    
    //得到收集npc的key
    void setCollectNpcKey(std::string key);
    
    //开启背包触摸
    void openBagTouch();
    
    //得到不同主题相应背景音乐
    std::string getBackMusic(const std::string& stageConfigFile);
    
    void returnStage();
    /* 关联当前关卡场景 */
    inline void attachStageScene(StageScene* pStageScene) {m_pStageScene = pStageScene;}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    void setAndroidRollBackListener(AndroidRollBackListener* androidRollBackListener){m_androidRollBackListener = androidRollBackListener;}
#endif
    
protected:
    Button* m_pReturnBtn = nullptr;         // 返回按钮
    Button* m_pMusicSwitchBtn = nullptr;    // 背景音乐开关按钮
    Actor* m_pBagActor = nullptr;           // 背包入口actor
    Layer* m_pSuperTouchLayer = nullptr;    // 触摸响应层
    NpcCollectCompleteNode* m_pNpcCollectNode = nullptr;
    
    std::string m_npcKey;           //当前收集npc的key
    
    bool m_isBagEnabled = true;
    // ui结点所属关卡场景
    StageScene* m_pStageScene = nullptr;
    int m_topicAudioId = 0;
private:
    bool m_isSceneOffset = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AndroidRollBackListener* m_androidRollBackListener = nullptr;
#endif
};
#endif /* defined(__AbelLLExplorer__UIBase__) */

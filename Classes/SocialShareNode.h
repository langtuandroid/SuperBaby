//
//  SocialShareNode.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/26.
//
//  社交共享 ui，用于完成整个关卡答题后将过关消息通过社交软件分享到朋友圈
//

#ifndef __AbelLLExplorer__SocialShareNode__
#define __AbelLLExplorer__SocialShareNode__

#include "cocos2d.h"

USING_NS_CC;

class Actor;
class RoundUINode;
class SocialShareNode : public Node
{
public:
    SocialShareNode();
    ~SocialShareNode();
    
public:
    Actor* getShareActor(){return m_pActor;}
    // 创建平台分享界面
    void createPlatformShareView();
    
    // 关联当前ui界面
    inline void attachUINode(RoundUINode* roundUINode){m_pRoundUINode = roundUINode;}
    
    void setScaleShareNode();
    
    Sprite* getShareBackGround(){return m_pShareBackGround;}
    Layer* getLayer(){return m_pLayer;}
    
public:
    bool init();
    CREATE_FUNC(SocialShareNode);

public:
    void setOKCb(std::function<void ()> okCb) {m_okCB = okCb;}
    void setCloseCb(std::function<void ()> closeCb) {m_closeCB = closeCb;}
    inline void setStageIndex(const int stageIndex) {m_stageIndex = stageIndex;}
    
private:
    std::function<void ()> m_okCB = nullptr;
    std::function<void ()> m_closeCB = nullptr;
    Actor* m_pActor = nullptr;
    unsigned int m_platform;           //分享平台index
    Layer* m_pLayer = nullptr;
    Sprite* m_pShareBackGround = nullptr;
    RoundUINode* m_pRoundUINode = nullptr;
    
    int m_stageIndex = -1;
};

#endif /* defined(__AbelLLExplorer__SocialShareNode__) */

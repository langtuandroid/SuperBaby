//
//  RoundBase.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  小关卡回合基类
//

#ifndef __AbelLLExplorer__RoundBase__
#define __AbelLLExplorer__RoundBase__

#include "cocos2d.h"
#include <vector>

USING_NS_CC;

class StageScene;
class Actor;
class RoundBase : public Node
{
public:
    RoundBase();
    virtual ~RoundBase();
    virtual void update(float);
    
public:
    // 进入回合
    virtual void onEnterRound() = 0;
    /* 庆祝 */
    virtual void celebrate(const float celebrateDelay /*庆祝延时*/) {;}
    /* 开启触摸 */
    virtual void openTouch(){;}
    /*  回合结束回调 */
    virtual void endRound(){;}
    /*  处理npc收集 */
    virtual void processNpcCollect(){;}
    // 清理资源
    void cleanUp();
    
    /* 设置"答题错误通知"回调函数 */
    void setAnswerErrorNotifyCB(std::function<void ()> ccb) {m_anwerErrorCB = ccb;};
    /* 设置"答题(正确)庆祝通知"回调函数 */
    void setCelebrateNotifyCB(std::function<void (const unsigned int)> ccb) {m_celebrateCB = ccb;};
    /* 设置"答题(正确)完成通知"回调函数 */
    void setCompleteNotifyCB(std::function<void ()> ccb) {m_completeCB = ccb;};
    /* 是否本关卡最后一个回合 */
    virtual bool isLastRound() const {return false;}
    
    /* 预加载资源（异步） */
    void preLoadRes();
    /* 预加载gaf（异步） */
    void preLoadGaf();
    /* 进入本回合 */
    inline void enterRound() {m_isEnterRound = true;}
    /* 预加载资源完成 */
    virtual void onPreLoadedRes();
    
    // 播放答对音效
    void playAnswerCorrectEffect();
    // 播放答错音效
    void playAnswerErrorEffect();
    
    //答对时主角说话音效
    void playLeadAnsCorEff();
    //答错时主角说话音效
    void playLeadAnsErrEff();
    //主角行走说话音效
    void playLeadWalkEff();
    //npc收集主角说话音效
    void playNpcCollectEff();
    
    int getEffIndex(){return m_effIndex;}
    
protected:
    void appendRemovedTexture(std::string);
    /* 添加要异步加载的纹理文件 */
    inline void appendAsyncLoadTexture(const std::string &textureFile)
    {m_asyncTextures.push_back(textureFile);}
    /* 添加要异步加载的纹理文件 */
    inline void appendAsyncLoadGaf(const std::string &gafFile)
    {m_asyncGafs.push_back(gafFile);}
    
protected:
    // 回合索引
    int m_roundIndex = -1;
    
    // 答题错误(回调函数)
    std::function<void ()> m_anwerErrorCB = nullptr;
    // 回合完成庆祝(回调函数)
    std::function<void (const unsigned int)> m_celebrateCB = nullptr;
    // 回合完成(回调函数)
    std::function<void ()> m_completeCB = nullptr;
    // 本回合结点所属关卡场景
    StageScene* m_pStageScene = nullptr;
    
    // 存储所有本回合结束后要释放的纹理文件
    std::vector<std::string> m_removedTextures;
    
    // 需要(异步)预加载的纹理
    std::vector<std::string> m_asyncTextures;
    // 已经(异步)加载成功的纹理数量
    unsigned int m_loadedTextures = 0;
    
    // 需要(异步)预加载的gaf
    std::vector<std::string> m_asyncGafs;
    // 已经(异步)加载成功的纹理数量
    unsigned int m_loadedGafs = 0;

private:
    bool m_isEnterRound = false;        // 是否已经进入本回合
    bool m_isAsyncResLoaded = false;    //
    int m_effIndex = -1;                // 主角音效index
};

#endif /* defined(__AbelLLExplorer__RoundBase__) */

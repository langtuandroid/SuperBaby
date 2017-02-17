//
//  StageScene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/2/28.
//
//  关卡场景基类
//
//  场景基类功能
//          加载预设在配置文件中的资源
//          小关卡切换时滚动场景

#ifndef __AbelLLExplorer__StageScene__
#define __AbelLLExplorer__StageScene__

#include "cocos2d.h"
#include <vector>

USING_NS_CC;

#define ROUND_ZORDER                30          // 回合(小关卡) zorder

class StageData;
class Actor;
class LeadingActor;
class RoundBase;
class RoundUINode;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
class AndroidRollBackListener;
#endif

class StageScene : public Scene
{
public:
    StageScene();
    virtual ~StageScene();
    
    /**/
    virtual void update(float);
    
public:
    /* 加载当前回合 */
    virtual RoundBase* loadRound() = 0;
    /* */
    virtual bool isPopShareNode(const unsigned int) = 0;
    /* 获取当前回合索引 */
    inline unsigned int getCurrRoundIndex() const {return m_currRoundIndex;}
    /* 获取关联的关卡数据 */
    inline StageData* getStageData() const {return m_pAttachStageData;}

    // 根据关卡数据初始化
    virtual bool initWithStageData(const StageData*);
    
    /* 庆祝回调 */
    virtual void celebrateCB(const unsigned int celebrateTimes = 1);
    /* 回合结束(答题正确)回调 */
    virtual void roundCompleteCB();
    /* 答题错误回调 */
    virtual void answerErrorCB();
    
public:
    /* 加载主角 */
    void loadLeadingActor();
    /* 隐藏主角 */
    void hideLeaderActor();
    
    // 开始一个回合
    void startRound();
    
    /* 主角移动  */
    void moveLeadingActor(const float, const cocos2d::Vec2&, const bool isauto = false /*是否自动切换动作*/);
    void moveLeadingActorCB(const float, const cocos2d::Vec2&,
                            const std::function<void()>& movedCompletedCB = nullptr, const bool isauto = true /*是否自动切换动作*/);
    
    void moveAimiActor(const float, const cocos2d::Vec2&, const bool isauto = false /*是否自动切换动作*/);
    void moveAimiActorCB(const float, const cocos2d::Vec2&,
                       const std::function<void()>& movedCompletedCB = nullptr, const bool isauto = true /*是否自动切换动作*/);
    
    void moveBeibeiActor(const float, const cocos2d::Vec2&, const bool isauto = false /*是否自动切换动作*/);
    void moveBeibeiActorCB(const float, const cocos2d::Vec2&,
                         const std::function<void()>& movedCompletedCB = nullptr, const bool isauto = true /*是否自动切换动作*/);
    void setVisibleAimiActor(bool);
    void setVisibleBeibeiActor(bool);
    
    /* 添加、移除 zorder 大于、小于主角的 node */
    void addOverLeaderNode(Node*, const cocos2d::Vec2&, const int, int zorderAdd = 0);
    void addOverAllNode(Node*, const int);
    void addBelowLeaderNode(Node*, const cocos2d::Vec2&, const int);
    void removeOBLeaderNode(const int);
    void removeOBAllNode(const int);
    
    LeadingActor* getAimeeActor(){return m_pAimeeActor;}
    LeadingActor* getBeibeiActor(){return m_pBebeiActor;}
    
    // 播放回答正确特效
    void playAnswerCorrectEffect(const Vec2&);
    
    RoundUINode* getRoundUINode(){return m_pRoundUINode;}
    
    // 回合结束移动阅读actor
    void roundEndMoveActor();
    
    // 根据当前回合结点位置释放已经看不见的背景图
    void releaseBgByCurrPos();
    
    //
    void onBgLoadedCB(Texture2D*);
    
    void onForeBgLoadedCB(Texture2D*);
    
    //
    void loadBgByCurrPosAsync();
    
    void loadForeBgByCurrPosAsync();
    
    /* 配合手动移动场景，加载相应地背景图片 */
    void loadBgByManualMove(const float, const std::function<void()>&);
    
    /* 过关 */
    void stageClear();
    
    /* 添加要移除的纹理文件 */
    void appendRemovedTexture(const std::string&);

    /* 添加要移除的精灵帧文件 */
    void appendRemovedSpriteFrameFile(const std::string&);
    
    /***/
    inline bool isTrial() const {return m_isTrial;}
    inline void setTrial(const bool isTrial) {m_isTrial = isTrial;}
    
    /* 手动滚动场景 */
    void manualScroll(float offset, const std::function<void()>&);
    
    /* 手动滚动背景 */
    void manualBackgroundScroll(float offset, const std::function<void()>&);
    
    /**/
    inline float getStageSceneOffsetX() const {return m_sceneOffsetX;}
    
    /* 获取当前背景精灵 */
    Sprite* getCurrBgSprite();
    
protected:
    Node *m_pContainerNode = nullptr;           // 作为背景图的容器节点
    StageData* m_pAttachStageData = nullptr;    // 关联的关卡数据
    RoundBase* m_pPreRoundNode = nullptr;       // 前一个回合结点
    RoundBase* m_pCurrRoundNode = nullptr;      // 当前回合结点
    RoundUINode* m_pRoundUINode = nullptr;      // 场景ui节点
    
    /*
     *  当前回合节点在 m_pContainerNode 容器节点中的位置
     */
    cocos2d::Vec2 m_roundNodePos = cocos2d::Vec2::ZERO;
    
    LeadingActor* m_pAimeeActor = nullptr;               // 艾米
    LeadingActor* m_pBebeiActor = nullptr;               // 蓓蓓

    // 当前回合索引号
    unsigned int m_currRoundIndex = 0;
    // 点击触发 Actor
    Actor* m_pDjcfActor = nullptr;
    
private:
    /* 根据关联的关卡数据加载关卡场景 */
    void loadStageScene();
    
    /* 滚动结束 */
    void scrollComplete();

    /* 主角移动结束 */
    void moveComplete();
    
    /* */
    void enterRound();
    
   // 释放所有背景
    void releaseAllBg();
    /* 根据当前回合结点位置加载背景图 */
    void loadBgByCurrPos();
    
    /* 背景图片是否能够覆盖  */
    bool bgCanOver(const float);
    
    /* 前景图片是否能够覆盖 */
    bool foreBgCanOver(const float);
    
    /* 背景图垂直滚动结束回调 */
    void bgVScrolledCB();
    
private:
    bool m_actorMovedCompleted = false;         // 主角是否移动到目的地
    bool m_stageScrollCompleted = false;        // 关卡是否移动完成
    
    float m_loadedBgTotalWidth = 0.0f;          // 所有已加载的背景图的总宽度
    
    float m_loadForeBgTotalWidth = 0.0f;        // 所有已加载的前景图的总宽度
    
    /* 存储所有要释放的纹理文件 */
    std::vector<std::string> m_removedTextures;
    
    /* 存储要释放的精灵帧文件 */
    std::vector<std::string> m_removedSpriteFrameFiles;
    
    // 已经加载的背景图片
    std::vector<cocos2d::Sprite*> m_bgSprites;
    
    // 已经加载的前景图片
    std::vector<Sprite*> m_foreSps;
    
    // 背景图片加载是否已经完成
    bool m_bgLoadCompleted = false;
    
    bool m_asyncLoadedForeBg = false;
    
    unsigned int m_randNumber = 0;          // 随机回合数，用于确定播放主角音效的回合
    
    bool m_isTrial = false;                 // 是否试玩
    
    /* 游戏场景水平偏移量，用于适配 visibleSize.width 大于 2048 的设备 */
    float m_sceneOffsetX = 0.0f;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AndroidRollBackListener* m_androidRollBackListener = nullptr;
#endif
};

#endif /* defined(__AbelLLExplorer__StageScene__) */

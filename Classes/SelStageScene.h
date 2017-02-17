//
//  SelStageScene.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/5/15.
//
//  关卡选择
//

#ifndef __AbelLLExplorer__SelStageScene__
#define __AbelLLExplorer__SelStageScene__

#include <map>
#include <vector>
#include "cocos2d.h"
#include <ui/CocosGUI.h>

USING_NS_CC;

// 关卡按钮
typedef struct BtnUnit
{
    std::string shapeKey;           // 按钮背景形状 key 值
    std::string normalImgFrame;
    std::string highLightImgFrame;
    std::string disableImgFrame;
    Sprite* btnBgSp = nullptr;                // 背景图片精灵
    Vec2 btnBgPos = Vec2::ZERO;
} BtnUnit;

// 图元
typedef struct ImgUnit
{
    std::string frameName = "";
    Vec2 pos = Vec2::ZERO;
} ImgUnit;

class GafAssetCache;
class StageDataManager;
class Actor;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
class AndroidRollBackListener;
#endif

class SelStageScene : public Scene
{
public:
    SelStageScene();
    virtual ~SelStageScene();
    
public:
    static Scene* createWithStageConfig(const std::string&);
    bool initWithStageConfig(const std::string&);
    
    virtual void onEnter();
    virtual void update(float);
    
public:
    void updateMenuButtonStage();
    void startMoveHead();
    
private:
    void initHead();
    std::string getBtnImgFrame(const std::string&, const std::string&);
    
    /* 更新数字按钮背景 */
    void updateBtnBg(const unsigned int, const unsigned int);
    
    /* 绑定按钮事件 */
    void bindBtnEvent(const unsigned int);
    
    /* */
    void moveHeadSchedule();
    void moveHeadEnd();
    
    /**/
    void preloadGafs();
    
    /**/
    void updateUi();
    
    /* 播放动动操 */
    void playDongDongCao();
    
    /* 绑定按钮事件 */
    void bindBtnsEvent();
    
    /* 弹出通关奖励 */
    void popStageClearAward();
    
    /* 显示所有轨迹点，调试用 */
    void showAllTrackPoint();
    
    /**/
    void onDongdongcalCompleted(float);
    
private:
    StageDataManager* m_pStageDataManager = nullptr;
    GafAssetCache* m_pGafAssetCache = nullptr;
    SpriteBatchNode* m_pSbn = nullptr;
    
    int m_themeIndex = -1;
    
    // 数字按钮图片信息
    std::map<std::string, std::map<std::string, std::string>> m_btnImgMap;
    
    // 关卡按钮
    BtnUnit m_btnUnit[12];
    
    // 背景图精灵
    Sprite* m_pBgSprite = nullptr;
    
    // 轨迹点
    std::vector<std::vector<Vec2>> m_trackPoints;
    // head
    Vec2 m_headPositions[12];        // 头像位置
    Node* m_headNode = nullptr;
    int m_headIndex = 0;
    std::vector<Vec2> m_headMove;
    std::vector<Vec2>::iterator m_headMoveIter;
    Vec2 m_headPos;
    
    bool m_headIsMove = false;
    
    std::string m_contentStr = "";
    
    //
    LayerColor* m_LayerColorAward = nullptr;
    
    Vec2 m_originalPoint;           // 原来的位置
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    
    cocos2d::experimental::ui::VideoPlayer* m_pVideoPlay = nullptr;
    
    // 预加载资源是否加载完毕
    bool m_preloaded = false;
    unsigned int m_numberOfPreLoadedTextures = 0;
    unsigned int m_numberOfPreLoadedGafs = 0;
    
    ///////
    std::vector<std::string> m_preloadTextures;
    std::vector<std::string> m_SpriteFrameFiles;
    std::vector<std::string> m_preloadGafs;
    
    // 过关奖励触发 gaf 是配置文件
    std::string m_stageClearAwardTriggerGafConfig = "";
    
    // 过关奖励待机状态 gaf
    Actor* m_awordStandByActor = nullptr;
    // 过关奖励触发状态 gaf
    Actor* m_awordTriggerActor = nullptr;
    /* 动动操 Actor */
    Actor* m_pdongdongcaoActor = nullptr;
    
    //////////////////////////////////////////////////////
    // 过关关卡号
    // 该值大于 0 表示有通关，并且该值保存通关关卡索引号
    //////////////////////////////////////////////////////
    int m_stageClearIndex = -1;
    
    // 最大过关关卡号
    int m_maxStageClearIndex = -1;
    bool m_hasStageClear = false;
    
    bool m_needMoveHead = false;
    bool m_needPopStageClearAward = false;
    
    ui::ScrollView* m_pScrollView = nullptr;
    
    bool m_isPopAward = false;
    
    float m_sceneOffset = 0.0f;
    
    Actor* m_pDongdongCaoActor = nullptr;
    std::string m_bgMusic = "";
    float m_bgMusicVolume = 0.0f;
    Node* m_pLoadingNode = nullptr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    AndroidRollBackListener* m_androidRollBackListener = nullptr;
#endif
};

#endif /* defined(__AbelLLExplorer__SelStageScene__) */

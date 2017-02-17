//
//  AppConfig.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/4.
//
//  程序配置信息
//

#ifndef __AbelLLExplorer__AppConfig__
#define __AbelLLExplorer__AppConfig__

#include <stdio.h>
#include <vector>

class AppConfig
{
public:
    ~AppConfig();
private:
    AppConfig();
    AppConfig(const AppConfig&);
    AppConfig& operator=(const AppConfig&);
    
public:
    static AppConfig* getInstance();
    /*
     *  生成下载资源目录
     */
    static void createAssetPath();
    
public:
    void loadConfigData();
    
    //
    inline std::string getAppVersion() const {return "1.1.0";}
    
    // 获取背景滚动速度
    inline float getBgScrollSpeed() const {return m_bgScrollSpeed;}
    // 获取背景滚动速度
    inline float getOverRoundCelebrateDurdation() const {return m_overRoundCelebrateDurdation;}
    
    //设置背景音乐开启状态
    void setBackMusicOpen(bool isOpen){m_backMusicOpen = isOpen;}
    //获取背景音乐开启状态
    bool getBackMusicOpen(){return m_backMusicOpen;}
    
    /* 是否显示 gaf 对象的包围框，调试用 */
    inline bool gafDebugDraw() const {return false;}
    
    /**/
    inline int getStageClearIndex() const {return m_stageClearIndex;}
    void setStageClearIndex(const int);
    
    void appendLeadAnsCorEff(const std::string);
    
    void appendLeadAnsErrEff(const std::string);
    
    void appendLeadWalkEff(const std::string);
    void appendLeadNpcCollectEff(const std::string);
    
    std::string getLeadAnsCorEff();
    
    std::string getLeadAnsErrEff();
    
    std::string getLeadWalkEff();
    
    std::string getLeadNpcCollectEff();
    
    void setPassTimes(const unsigned int passTime){m_passTimes = passTime;}
    
    void addPassTimes(){m_passTimes++;}
    
    unsigned int getPassTimes(){return m_passTimes;}
    
    bool isDebugMode() const;
    
    /* 获取设计分辨率宽度 */
    inline float getDesignResolutionWidth() const {return 2048.0f;}
    
    /* 获取场景水平偏移量 */
    float getSceneOffsetX();
    
    /* 获取关卡热更新资源本地路径 */
    
    /* 指定主题热更新资源是否已经初始化 */
    bool isStageAssetInited(const int);
    /* 设置指定主题热更新资源初始化完成 */
    void setStageAssetInited(const int);
    
private:
    // 背景图滚动速度，单位是每秒滚动的逻辑单位数
    float m_bgScrollSpeed = 10.0f;
    
    // 过小回合庆祝动画播放时间
    float m_overRoundCelebrateDurdation = 5.0f;
    
    // 背景音乐是否开启状态,默认开启
    bool m_backMusicOpen = true;
    
    // 记录通关关卡索引号，大于0的值表示有通关
    int m_stageClearIndex = -1;
    
    //主角答对说话音效
    std::vector<std::string> m_leadAnsCorEff;
    
    //主角答错说话音效
    std::vector<std::string> m_leadAnsErrEff;
    
    //主角行走说话音效
    std::vector<std::string> m_leadWaikEff;
    // npc收集时主角说话音效
    std::vector<std::string> m_leadNpcCollectEff;
    
    // 通过关卡次数，用于判断是否播放动动操
    unsigned int m_passTimes = 0;
};

#endif /* defined(__AbelLLExplorer__AppConfig__) */

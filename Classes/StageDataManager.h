//
//  StageDataManager.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/2/28.
//
//  关卡管理
//

#ifndef __AbelLLExplorer__StageDataManager__
#define __AbelLLExplorer__StageDataManager__

#include <stdio.h>
#include <map>

class StageData;
class StageDataManager
{
public:
    virtual ~StageDataManager();

private:
    StageDataManager();
    StageDataManager(const StageDataManager&);
    StageDataManager& operator=(const StageDataManager&);

public:
    static StageDataManager* getInstance();
    
public:
    /* 关卡配置文件 */
    void loadStageConfig();
    
    /* 加载过关社交共享文本 */
    void loadSocialShareText();
    
    /* 获取关卡配置文件 */
    std::string getStageConfigFile(const int);
    
    /* 根据 json 关卡数据加载关卡数据*/
    StageData* loadStageDataWithJson(const std::string&);
    /* 清除关卡数据 */
    void destoryStageData();

    /* 获取关卡数 */
    unsigned int getNumberOfStages();
    
    /* 获取和设置关卡数据 */
    void setStageData(const unsigned int, StageData*);
    StageData* getStageData(const unsigned int);
    
    bool stageIsClear(const int);
    bool stageClear(const int);
    int getMaxClearStageIndex(const int);
    void manualSetMaxClearStageIndex(const int);
    
    /**/
    std::string getSocialShareText(const int);
    
private:
    /* 关卡配置文件 */
    std::map<int, std::string> m_stageConfigFiles;
    /* 关卡数据 */
    std::map<int, StageData*> m_stageData;
    
    /* 关卡过关社交分享文本 */
    std::map<int, std::string> m_stageSocialTexts;
};

#endif /* defined(__AbelLLExplorer__StageDataManager__) */

//
//  LoadingStageScene.h
//  AbelLLExplorer
//
//  Created by YANGJIE on 2015/04/01.
//
//  检测关卡资源缺失（除第一主题外）
//  检测到关卡资源文件有缺失的话需要从 UCloud 服务器下载相应地资源文件
//  加载关卡资源并进入相应关卡
//

#ifndef __AbelLLExplorer__LoadingStageScene__
#define __AbelLLExplorer__LoadingStageScene__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "network/HttpClient.h"
#include <vector>
#include <string>

USING_NS_CC;
using namespace cocos2d::network;

class Actor;
class StageData;
class LoadingStageScene : public Scene
{
public:
    LoadingStageScene();
    virtual ~LoadingStageScene();
    
    static Scene* createWithStageIndex(const unsigned int, const unsigned int, const bool isTrial = false);
    bool initWithStageIndex(const unsigned int, const unsigned int, const bool isTrial);
    void onEnter();

private:
    void onLoadCompletedCB(Texture2D*);
    
    /* 预加载场景 img 资源 */
    void preloadStageImgs();
    
    /* 预加载场景 gaf 资源 */
    void preloadStageGafs();
    
    /* 进入关卡 */
    void enterStage();
    
    void update(float);
    
    /* 是否存在缺失的文件 */
    bool hasMissFile();
    
    void startLoad();
    
    
    void downloadScdule(float);
    void onStageFileDownloadedCB(HttpClient*, HttpResponse *);
    void downloadStageFile();
    
    void onAudioFileDownloadedCB(HttpClient*, HttpResponse*);
    void downloadAudioFile();
    
    std::string basename(const std::string& path) const;
    bool decompress(const std::string &zip);
    
    /* 返回关卡选择界面 */
    void retToSelStage();
    
private:
    int m_stageIndex = -1;
    int m_themeIndex = -1;
    unsigned int m_numberOfWantLoadedRes = 0;
    
    StageData* m_pStageData = nullptr;
    
    ui::LoadingBar* m_pLoadingBar = nullptr;
    Sprite* m_loadbarSp = nullptr;
    
    // 加载进度动画
    static int m_actorIndex;
    std::string m_actorFile[4];
    Vec2 m_actorNodePos[4];
    Size m_actorSize[4];
    std::string m_currGafFile = "";
    
    unsigned int m_loadedBgs = 0;
    float m_loadedBgWidth = 0;
    
    unsigned long m_preloadImgs = 0;
    unsigned long m_loadedImgs = 0;
    unsigned long m_preloadGafs = 0;
    unsigned long m_loadedGafs = 0;
    
    bool m_isTrial = false;
    
    unsigned int m_willLoadedImg = 0;
    unsigned int m_willLoadedGaf = 0;
    bool m_isLoadedCompleteImg = false;
    bool m_isLoadedCompleteGaf = false;
    std::vector<std::string> m_imgs;
    std::vector<std::string> m_gafs;
    unsigned long m_resNum = 0;
    float m_addPercent = 0.0f;
    
    std::string m_stageRemoteURL = "";
    std::string m_stageRemoteFileMD5 = "";
    std::string m_audioRemoteURL = "";
    std::string m_audioRemoteFileMD5 = "";
    bool m_hasMissStageFile = false;
    bool m_hasMissAudioFile = false;
};

#endif /* defined(__AbelLLExplorer__LoadingStageScene__) */

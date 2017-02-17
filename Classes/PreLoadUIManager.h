//
//  PreLoadUIManager.h
//  AbelLLExplorer
//
//  Created by YANGJIE on 2015/4/16.
//
//

#ifndef __AbelLLExplorer__PreLoadUIManager__
#define __AbelLLExplorer__PreLoadUIManager__

#include <vector>
#include <functional>

class PreLoadUIManager
{
public:
    ~PreLoadUIManager();

private:
    PreLoadUIManager();
    PreLoadUIManager(const PreLoadUIManager&);
    PreLoadUIManager& operator= (const PreLoadUIManager&);
    
public:
    static PreLoadUIManager* getInstance();
    
public:
    void startPreloadUI(std::function<void ()> cb);
    
private:
    /* 添加要预加载的 UI 资源 */
    void appendPreLoadRes();
    /* 释放预加载的 UI 资源 */
    void destroyPreloadUI();

    /* 预加载纹理 */
    void preLoadTextures();
    /* 预加载 gaf */
    void preLoadGafs();
    /* 预加载 plist */
    void preLoadPlists();
    
private:
    // 需要预加载的纹理文件
    std::vector<std::string> m_preLoadTextures;
    // 需要预加载的 gaf 文件
    std::vector<std::string> m_preLoadGafs;
    // 需要预加载的 plist 文件
    std::vector<std::string> m_preLoadPlists;
    
    // 预加载完成回调
    std::function<void ()> m_completedDelegate = nullptr;
    
    int m_gafLoaded = 0;
};

#endif /* defined(__AbelLLExplorer__PreLoadUIManager__) */

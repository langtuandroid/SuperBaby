//
//  GafAssetCache.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/4/15.
//
//  GafAsset 对象缓存，提高 gaf 对象加载性能
//

#ifndef __AbelLLExplorer__GafAssetCache__
#define __AbelLLExplorer__GafAssetCache__

#include "cocos2d.h"
#include <queue>
#include <vector>

USING_NS_CC;

namespace gaf
{
    class GAFAsset;
}

using namespace gaf;
class GafAssetCache : public Ref
{
public:
    GafAssetCache();
    ~GafAssetCache();

public:
    static GafAssetCache* getInstance();
    static void destroyInstance();

public:
    struct AsyncStruct
    {
    public:
        AsyncStruct(const std::string& fn, std::function<void(GAFAsset*)> f) : filename(fn), callback(f) {}
        std::string filename;
        std::function<void(GAFAsset*)> callback;
    };
    
public:
    /**/
    void waitForQuit();
    /**/
    GAFAsset* addGafAsset(const std::string &);
    /**/
    void addGafAssetAsync(const std::string &, const std::function<void(GAFAsset*)>&);
    /**/
    void removeGafAssetForKey(const std::string &, bool isShowRef = false);
    /**/
    GAFAsset* getGafAssetForKey(const std::string &) const;
    /* */
    void removeUnusedAssets();

private:
    /* 加载 GafAsset 的线程函数 */
    void loadGafAssetThread();
    /* 将 gafAsset 对象加入容器  */
    void addGafAssetAsyncCallBack(float);
    
protected:
    typedef struct _GafAssetInfo
    {
        AsyncStruct *asyncStruct;
        GAFAsset        *gafAsset;
    } GafAssetInfo;
    
    std::thread* _loadingThread;
    std::mutex _sleepMutex;
    std::condition_variable _sleepCondition;
    bool _needQuit;
    int _asyncRefCount;
    std::unordered_map<std::string, GAFAsset*> _gafAssets;

    std::queue<AsyncStruct*>* _asyncStructQueue;
    std::mutex _asyncStructQueueMutex;
    std::deque<GafAssetInfo*>* _gafAssetInfoQueue;
    std::mutex gafAssetInfoMutex;
};

#endif /* defined(__AbelLLExplorer__GafAssetCache__) */


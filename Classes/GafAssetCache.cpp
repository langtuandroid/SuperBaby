//
//  GafAssetCache.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/4/15.
//
//

#include "GafAssetCache.h"
#include "GAFPlayer/GAF.h"
#include "GAFPlayer/GAFShaderManager.h"

using namespace gaf;

static GafAssetCache* pGafAssetCache = nullptr;

/*
 *  constructor
 */
GafAssetCache::GafAssetCache():
_loadingThread(nullptr),
_needQuit(false),
_asyncRefCount(0),
_asyncStructQueue(nullptr),
_gafAssetInfoQueue(nullptr)
{
    GAFShaderManager::Initialize();
}

/*
 *  destructor
 */
GafAssetCache::~GafAssetCache() {}

//
GafAssetCache* GafAssetCache::getInstance()
{
    if (pGafAssetCache == nullptr) {
        pGafAssetCache = new GafAssetCache();
    }
    return pGafAssetCache;
}

//
void GafAssetCache::destroyInstance()
{
    if (pGafAssetCache) {
        pGafAssetCache->waitForQuit();
        CC_SAFE_DELETE(pGafAssetCache);
    }
}

//
void GafAssetCache::waitForQuit()
{
    // notify sub thread to quick
    _needQuit = true;
    _sleepCondition.notify_one();
    if (_loadingThread) {
        _loadingThread->join();
        _loadingThread = nullptr;
    }
}

//
GAFAsset* GafAssetCache::addGafAsset(const std::string &path)
{
    GAFAsset* pGafAsset = nullptr;
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);
    if (fullpath.size() == 0) {
        return nullptr;
    }
    auto it = _gafAssets.find(fullpath);
    if( it != _gafAssets.end() )
        pGafAsset = it->second;
    
    if (pGafAsset == nullptr)
    {
        pGafAsset = gaf::GAFAsset::create(fullpath);
        if (pGafAsset != nullptr)
        {
            _gafAssets.insert( std::make_pair(fullpath, pGafAsset) );
            CC_SAFE_RETAIN(pGafAsset);
        } else {
            CCLOG("cocos2d: Couldn't create gafAsset for file:%s in GafAssetCache", path.c_str());
        }
    }
    return pGafAsset;
}

//
void GafAssetCache::addGafAssetAsync(const std::string & path, const std::function<void(GAFAsset*)>& callback)
{
    GAFAsset *pGAFAsset = nullptr;
    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);
    auto it = _gafAssets.find(fullpath);
    if( it != _gafAssets.end() )
        pGAFAsset = it->second;
    if (pGAFAsset != nullptr)
    {
        callback(pGAFAsset);
        return;
    }
    
    // lazy init
    if (_asyncStructQueue == nullptr)
    {
        _asyncStructQueue  = new std::queue<AsyncStruct*>();
        _gafAssetInfoQueue = new std::deque<GafAssetInfo*>();
        
        // create a new thread to load gafAsset
        _loadingThread = new std::thread(&GafAssetCache::loadGafAssetThread, this);
        
        _needQuit = false;
    }
    
    if (0 == _asyncRefCount)
    {
        Director::getInstance()->getScheduler()->schedule(CC_SCHEDULE_SELECTOR(GafAssetCache::addGafAssetAsyncCallBack), this, 0, false);
    }
    
    ++_asyncRefCount;
    
    // generate async struct
    AsyncStruct *data = new (std::nothrow) AsyncStruct(fullpath, callback);
    
    // add async struct into queue
    _asyncStructQueueMutex.lock();
    _asyncStructQueue->push(data);
    _asyncStructQueueMutex.unlock();
    
    _sleepCondition.notify_one();
}

/*
 *
 */
void GafAssetCache::addGafAssetAsyncCallBack(float dt)
{
    std::deque<GafAssetInfo*> *gafAssetsQueue = _gafAssetInfoQueue;
    
    gafAssetInfoMutex.lock();
    if (gafAssetsQueue->empty())
    {
        gafAssetInfoMutex.unlock();
    } else {
        GafAssetInfo *pGafAssetInfo = gafAssetsQueue->front();
        gafAssetsQueue->pop_front();
        gafAssetInfoMutex.unlock();
        
        AsyncStruct *asyncStruct = pGafAssetInfo->asyncStruct;
        GAFAsset *gafAsset = pGafAssetInfo->gafAsset;
        const std::string& filename = asyncStruct->filename;
        
        if (gafAsset)
        {
            // cache the gafAsset.
            _gafAssets.insert( std::make_pair(filename, gafAsset) );
        } else {
            auto it = _gafAssets.find(filename);
            if(it != _gafAssets.end())
                gafAsset = it->second;
        }
        if (asyncStruct->callback)
        {
            asyncStruct->callback(gafAsset);
        }
        delete asyncStruct;
        delete pGafAssetInfo;
        
        --_asyncRefCount;
        if (0 == _asyncRefCount)
        {
            Director::getInstance()->getScheduler()->unschedule(CC_SCHEDULE_SELECTOR(GafAssetCache::addGafAssetAsyncCallBack), this);
        }
    }
}

/*
 *  加载 gafAsset 的线程函数
 */
void GafAssetCache::loadGafAssetThread()
{
    AsyncStruct *asyncStruct = nullptr;
    while (true)
    {
        std::queue<AsyncStruct*> *pQueue = _asyncStructQueue;
        _asyncStructQueueMutex.lock();
        if (pQueue->empty())
        {
            _asyncStructQueueMutex.unlock();
            if (_needQuit) {
                break;
            } else {
                std::unique_lock<std::mutex> lk(_sleepMutex);
                _sleepCondition.wait(lk);
                continue;
            }
        } else {
            asyncStruct = pQueue->front();
            pQueue->pop();
            _asyncStructQueueMutex.unlock();
        }
        
        // generate gafAsset
        auto pGafAsset =  new gaf::GAFAsset();
        if (!pGafAsset) {
            CCLOG("can not load %s", asyncStruct->filename.c_str());
            continue;
        }
        pGafAsset->initWithGAFFile(asyncStruct->filename, nullptr);
        
        // generate image info
        GafAssetInfo *gafAssetInfo = new (std::nothrow) GafAssetInfo();
        gafAssetInfo->asyncStruct = asyncStruct;
        gafAssetInfo->gafAsset = pGafAsset;
        
        // put the GafAsset info into the queue
        gafAssetInfoMutex.lock();
        _gafAssetInfoQueue->push_back(gafAssetInfo);
        gafAssetInfoMutex.unlock();
    } /*while*/
    CC_SAFE_DELETE(_asyncStructQueue);
}

//
void GafAssetCache::removeGafAssetForKey(const std::string &gafAssetKeyName, bool isShowRef)
{
    std::string key = gafAssetKeyName;
    auto it = _gafAssets.find(key);
    
    if( it == _gafAssets.end() )
    {
        key = FileUtils::getInstance()->fullPathForFilename(gafAssetKeyName);
        it = _gafAssets.find(key);
    }
    if( it != _gafAssets.end() )
    {
        if (isShowRef) {
            log("assetFile = %s refCount = %d", gafAssetKeyName.c_str(),
                (it->second)->getReferenceCount() );
        }
        (it->second)->release();
        _gafAssets.erase(it);
    }
}

//
void GafAssetCache::removeUnusedAssets()
{
    for( auto it = _gafAssets.cbegin(); it != _gafAssets.cend(); /* nothing */) {
        GAFAsset* pGAFAsset = it->second;
        if( pGAFAsset->getReferenceCount() == 1 ) {
            log("GafAssetCache: removing unused gafAsset: %s", it->first.c_str());
            pGAFAsset->release();
            _gafAssets.erase(it++);
        } else {
            ++it;
        }
    } /*for*/
}

//
GAFAsset* GafAssetCache::getGafAssetForKey(const std::string &assetKeyName) const
{
    std::string key = assetKeyName;
    auto it = _gafAssets.find(key);
    if (it == _gafAssets.end() )
    {
        key = FileUtils::getInstance()->fullPathForFilename(assetKeyName);
        it = _gafAssets.find(key);
    }
    
    if( it != _gafAssets.end() )
        return it->second;
    return nullptr;
}

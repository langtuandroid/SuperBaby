//
//  UpdateManager.h
//  yanxispace
//
//  Created by adinnet on 2015/07/07.
//
//

#ifndef __yanxispace__UpdateManager__
#define __yanxispace__UpdateManager__

#include <functional>
#include <string>
#include "network/HttpClient.h"
#include "extensions/cocos-ext.h"

using namespace cocos2d::network;

/*
 *  用于在线更新 Apk
 *  仅适用于 android 平台
 */
class UpdateManager
{
public:
    ~UpdateManager();
private:
    UpdateManager();
    UpdateManager(const UpdateManager&);
    UpdateManager& operator=(const UpdateManager&);

public:
    static UpdateManager* getInstance();

public:
    void setHttpErrCB(const std::function<void(long)>& cb) {m_httpErrorCb = cb;}
    void chkSvrUpt(const std::function<void(bool, const std::string&)>&);
    void apkUptFromSvr(const std::function<void(bool)>&,
                       const std::function<void(float)>);
    void installNewApk();

private:
    void onHttpRequestCompleted(HttpClient*, HttpResponse*);
    void eventListenAssetsManagerCB(cocos2d::extension::EventAssetsManagerEx*);
    void onGetSvrVer(char*);
    void destroyAssetsManager();
    
private:
    cocos2d::extension::AssetsManagerEx* _am = nullptr;
    cocos2d::extension::EventListenerAssetsManagerEx* _amListener = nullptr;
    
    std::function<void(bool, const std::string&)> m_chkCB = nullptr;
    std::function<void(bool)> m_apkCB = nullptr;
    std::function<void(float)> m_updateProgressCB = nullptr;
    std::function<void(long)> m_httpErrorCb = nullptr;
};

#endif /* defined(__yanxispace__UpdateManager__) */

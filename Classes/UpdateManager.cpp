//
//  UpdateManager.cpp
//  yanxispace
//
//  Created by adinnet on 2015/07/07.
//
//  使用方法
//      第一步：调用 chkSvrUpt 方法检查是否有新版本
//
//      可以调用 setHttpErrCB 设置 http 错误回调
//      如果回调函数返回表示服务器有新版本，调用 apkUpt 方法下载
//
//
//isNetworkAvailable

#include "UpdateManager.h"
#include "NetUtil.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "cocos2d.h"
#include "AppConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

#define GET_VER_TAG                 "GVT"

#define LOCAL_MANIFEST_FILE         "res/versionUpdate/project.manifest"
#define SERVER_VERSION_FILE         "http://abelclass.ufile.ucloud.cn/superbaby/version.json"
#define LOCAL_APK_FILE              "superBaby.apk"

#define FAIL_REASON_NETWORK         "没有网络连接"
#define FAIL_REASON_GETPROFILE      "获取版本配置文件失败"
#define FAIL_REASON_ISALNEW         "已经是最新版本了"

USING_NS_CC;
USING_NS_CC_EXT;

/*
 *  constructor
 */
UpdateManager::UpdateManager() {}

/*
 *  destructor
 */
UpdateManager::~UpdateManager()
{
    destroyAssetsManager();
}

UpdateManager* UpdateManager::getInstance()
{
    static UpdateManager s_UpdateManager;
    return &s_UpdateManager;
}

void UpdateManager::chkSvrUpt(const std::function<void(bool, const std::string&)>& chkCB)
{
    m_chkCB = chkCB;
    if (NetUtil::isConnectionAvailable())
    {
        HttpRequest* requestGetApk = new HttpRequest();
        requestGetApk->setUrl(SERVER_VERSION_FILE);
        requestGetApk->setRequestType(HttpRequest::Type::GET);
        requestGetApk->setTag(GET_VER_TAG);
        requestGetApk->setResponseCallback(CC_CALLBACK_2(UpdateManager::onHttpRequestCompleted, this));
        HttpClient::getInstance()->send(requestGetApk);
        requestGetApk->release();
    } else {
        if (m_chkCB != nullptr) {
            m_chkCB(false, FAIL_REASON_NETWORK);
            m_chkCB = nullptr;
        }
    }
}

void UpdateManager::destroyAssetsManager()
{
    CC_SAFE_RELEASE_NULL(_am);
    if (_amListener != nullptr)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_amListener);
        _amListener = nullptr;
    }
}

void UpdateManager::eventListenAssetsManagerCB(cocos2d::extension::EventAssetsManagerEx* ev)
{
    static int failCount = 0;
    switch (ev->getEventCode())
    {
    case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
        {
            log("No local manifest file found, skip assets update.");
            destroyAssetsManager();
        }
        break;
    case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
        {
            std::string assetId = ev->getAssetId();
            float percent = ev->getPercent();
            std::string str;
            if (assetId == AssetsManagerEx::VERSION_ID)
            {
                str = StringUtils::format("Version file: %.2f", percent) + "%";
            }
            else if (assetId == AssetsManagerEx::MANIFEST_ID)
            {
                str = StringUtils::format("Manifest file: %.2f", percent) + "%";
            }
            else
            {
                str = StringUtils::format("%.2f", percent) + "%";
                log("%.2f Percent", percent);
                if (m_updateProgressCB != nullptr) {
                    m_updateProgressCB(percent);
                }
            }
        }
        break;
    case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
    case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
        {
            log("Fail to download manifest file, update skipped.");
            destroyAssetsManager();
            if (m_apkCB != nullptr)
            {
                m_apkCB(false);
                m_apkCB = nullptr;
            }
        }
        break;
    case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
        {
            log("Update finished. ALREADY_UP_TO_DATE");
            destroyAssetsManager();
            if (m_apkCB != nullptr)
            {
                m_apkCB(false);
                m_apkCB = nullptr;
            }
        }
        break;
    case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
        {
            log("Update finished. %s", ev->getMessage().c_str());
            destroyAssetsManager();
            if (m_apkCB != nullptr)
            {
                m_apkCB(true);
                m_apkCB = nullptr;
            }
        }
        break;
    case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
        {
            log("Update failed. %s", ev->getMessage().c_str());
            if (++failCount < 3)
            {
                _am->downloadFailedAssets();
            } else {
                log("Reach maximum fail count, exit update process");
                failCount = 0;
                destroyAssetsManager();
                if (m_apkCB != nullptr) {
                    m_apkCB(false);
                    m_apkCB = nullptr;
                }
            }
        }
        break;
    case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
        {
            log("Asset %s : %s", ev->getAssetId().c_str(), ev->getMessage().c_str());
        }
        break;
    case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
        {
            log("%s", ev->getMessage().c_str());
        }
        break;
    default:
        break;
    };
}

void UpdateManager::apkUptFromSvr(const std::function<void(bool)>& apkCB,
                                  const std::function<void(float)> upCB)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_apkCB = apkCB;
    m_updateProgressCB = upCB;
    
    std::string storagePath = FileUtils::getInstance()->getUpdatePath() + "update/";
    FileUtils::getInstance()->addSearchPath(storagePath);
    
    _am = AssetsManagerEx::create(LOCAL_MANIFEST_FILE, storagePath);
    _am->retain();
    
    if (!_am->getLocalManifest()->isLoaded())
    {
        log("Fail to update assets, step skipped.");
    } else {
        _amListener = EventListenerAssetsManagerEx::create(_am,
                std::bind(&UpdateManager::eventListenAssetsManagerCB, this, std::placeholders::_1));
        
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_amListener, 1);
        _am->update();
    }
#endif
}

void UpdateManager::onGetSvrVer(char* jsonBuffer)
{
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(jsonBuffer);
    if (doc.HasParseError()) /*解析错误*/
    {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        //RAPIDJSON_ASSERT(!doc.HasParseError());
        if (m_chkCB != nullptr) {
            m_chkCB(false, FAIL_REASON_GETPROFILE);
            m_chkCB = nullptr;
        }
        return;
    }
    std::string svrVersion = doc["apkVersion"].GetString();
    if (m_chkCB != nullptr)
    {
        std::string message = "";
        bool hasNew = svrVersion != AppConfig::getInstance()->getAppVersion();
        if (!hasNew) {
            message = FAIL_REASON_ISALNEW;
        }
        m_chkCB(hasNew, message);
        m_chkCB = nullptr;
    }
}

void UpdateManager::installNewApk()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo minfo;
    jobject jobj;
    bool b = JniHelper::getStaticMethodInfo(minfo,
                                            "org/cocos2dx/cpp/AppActivity",
                                            "cppCall_logsth",
                                            "()Ljava/lang/Object;");
    if (!b) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    
    b = JniHelper::getMethodInfo(minfo, "org/cocos2dx/cpp/AppActivity",
                                 "updateVersion",
                                 "(Ljava/lang/String;)V");
    if (!b) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        std::string storagePath = FileUtils::getInstance()->getUpdatePath() + "update/";
        std::string localApkFile = storagePath + LOCAL_APK_FILE;
        
        jstring jkey = minfo.env->NewStringUTF(localApkFile.c_str());
        minfo.env->CallVoidMethod(jobj, minfo.methodID, jkey);
        minfo.env->DeleteLocalRef(jkey);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
#endif
}

//
void UpdateManager::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
    long statusCode = response->getResponseCode();
    std::string tag = response->getHttpRequest()->getTag();
    char statusStrBuf[64];
    sprintf(statusStrBuf, "HTTP Status Code: %ld, tag = %s", statusCode, tag.c_str());
    cocos2d::log("response code: %ld", statusCode);
    
    do
    {
        if (!response->isSucceed())
        {
            cocos2d::log("response failed, error message is: %s", response->getErrorBuffer());
            if (m_httpErrorCb != nullptr)
            {
                m_httpErrorCb(statusCode);
                m_httpErrorCb = nullptr;
            }
            break;
        }
        
        std::vector<char> *pBuffer = response->getResponseData();
        if (tag == GET_VER_TAG)
        {
            char* pReceBuffer = (char *)malloc((unsigned int)pBuffer->size() + 1);
            for (int i = 0; i < pBuffer->size(); i++)
            {
                pReceBuffer[i] = (*pBuffer)[i];
            } /*for*/
            pReceBuffer[pBuffer->size()] = '\0';
            onGetSvrVer(pReceBuffer);
            free(pReceBuffer);
            pReceBuffer = nullptr;
        }
    } while(false);
}

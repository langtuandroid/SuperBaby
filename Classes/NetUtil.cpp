//
//  NetUtil.cpp
//  superBaby
//
//  Created by adinnet on 2015/08/17.
//
//

#include "NetUtil.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

#include "cocos2d.h"
USING_NS_CC;

#endif

bool NetUtil::isConnectionAvailable()
{
    bool isConnAvailable = false;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    isConnAvailable = isConnectionAvailableAndroid();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    isConnAvailable = isConnectionAvailableIos();
#endif
    return isConnAvailable;
}

bool NetUtil::isWIFIEnabled()
{
    bool isWifiEnabled = false;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    isWifiEnabled = isWIFIEnabledAndroid();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    isWifiEnabled = isWIFIEnabledIos();
#endif
    return isWifiEnabled;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
bool NetUtil::isConnectionAvailableAndroid()
{
    bool isNetOk = true;
    JniMethodInfo minfo;
    jboolean jIsNetOk = JNI_FALSE;
    bool isHave = JniHelper::getStaticMethodInfo(minfo,
                                                 "org/cocos2dx/cpp/AppActivity",
                                                 "isNetworkAvailable",
                                                 "()Z");
    if (!isHave) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jIsNetOk = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    isNetOk = (jIsNetOk == JNI_TRUE);
    return isNetOk;
}

bool NetUtil::isWIFIEnabledAndroid()
{
    bool isWifiOk = true;
    JniMethodInfo minfo;
    jboolean jIsWifiOk = JNI_FALSE;
    bool isHave = JniHelper::getStaticMethodInfo(minfo,
                                                 "org/cocos2dx/cpp/AppActivity",
                                                 "isWifiEnable",
                                                 "()Z");
    if (!isHave) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jIsWifiOk = minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    isWifiOk = (jIsWifiOk == JNI_TRUE);
    return isWifiOk;
}
#endif

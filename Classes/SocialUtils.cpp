//
//  SocialUtils.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/5/27.
//
//

#include "SocialUtils.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

jstring stoJstring(JNIEnv* env, const char* pat)
{
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}
#endif

void SocialUtils::shareToWXTimeline(const std::string& shareMessage)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    SocialUtils::shareToWXTimelineForAndroid(shareMessage);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SocialUtils::shareToWXTimelineForIOS(shareMessage);
#endif
}

void SocialUtils::shareToWXSession(const std::string& shareMessage)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    SocialUtils::shareToWXSessionForAndroid(shareMessage);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SocialUtils::shareToWXSessionForIOS(shareMessage);
#endif
}

void SocialUtils::shareToQQ(const char* pTitle, const char* pSummary)
{
    CCASSERT(pTitle && pSummary, "invalid para");
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    SocialUtils::shareToQQForAndroid(pTitle, pSummary);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SocialUtils::shareToQQForIOS(pTitle, pSummary);
#endif
}

void SocialUtils::shareToQQZone(const char* pTitle, const char* pSummary)
{
    CCASSERT(pTitle && pSummary, "invalid para");
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SocialUtils::shareToQQZoneForIOS(pTitle, pSummary);
#endif
}

void SocialUtils::shareToWeibo(const char* pTitle, const char* pDescription)
{
    CCASSERT(pTitle && pDescription, "invalid para");
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    SocialUtils::shareToWeiboForAndroid(pDescription);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SocialUtils::shareToWeiboForIOS(pTitle, pDescription);
#endif
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void SocialUtils::shareToWXTimelineForAndroid(const std::string& shareMessage)
{
    JniMethodInfo minfo;
    bool isHave = JniHelper::getStaticMethodInfo(minfo, "org/cocos2dx/cpp/AppActivity",
                                                 "sendMsgToWxTimeLine", "(Ljava/lang/String;)V");
    if (!isHave) {
        log("jni:sendMsgToTimeLine is null");
    } else {
        jstring shareContent = stoJstring(minfo.env, shareMessage.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, shareContent);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void SocialUtils::shareToWXSessionForAndroid(const std::string& shareMessage)
{
    JniMethodInfo minfo;
    bool isHave = JniHelper::getStaticMethodInfo(minfo, "org/cocos2dx/cpp/AppActivity",
                                                 "sendMsgToWxSession", "(Ljava/lang/String;)V");
    if (!isHave) {
        log("jni:sendMsgToTimeLine is null");
    } else {
        jstring shareContent = stoJstring(minfo.env, shareMessage.c_str());
        minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, shareContent);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void SocialUtils::shareToWeiboForAndroid(const std::string& shareMsg)
{
    JniMethodInfo minfo;
    jobject jobj;
    bool isHave = JniHelper::getStaticMethodInfo(minfo,
                                                 "org/cocos2dx/cpp/AppActivity",
                                                 "cppCall_logsth",
                                                 "()Ljava/lang/Object;");
    if (!isHave) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    
    isHave = JniHelper::getMethodInfo(minfo, "org/cocos2dx/cpp/AppActivity",
                                      "sendMsgToWeibo",
                                      "(Ljava/lang/String;)V");
    if (!isHave) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jstring jstringMsg = stoJstring(minfo.env, shareMsg.c_str());
        minfo.env->CallVoidMethod(jobj, minfo.methodID, jstringMsg);
        minfo.env->DeleteLocalRef(jstringMsg);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

void SocialUtils::shareToQQForAndroid(const char* pTitle, const char* pSummary)
{
    JniMethodInfo minfo;
    jobject jobj;
    bool isHave = JniHelper::getStaticMethodInfo(minfo,
                                            "org/cocos2dx/cpp/AppActivity",
                                            "cppCall_logsth",
                                            "()Ljava/lang/Object;");
    if (!isHave) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
    
    isHave = JniHelper::getMethodInfo(minfo, "org/cocos2dx/cpp/AppActivity",
                                 "shareToQQ",
                                 "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!isHave) {
        cocos2d::log("JniHelper::getMethodInfo error...");
    } else {
        jstring jstringTitle = stoJstring(minfo.env, pTitle);
        jstring jstringSummary = stoJstring(minfo.env, pSummary);
        minfo.env->CallVoidMethod(jobj, minfo.methodID, jstringTitle, jstringSummary);
        minfo.env->DeleteLocalRef(jstringTitle);
        minfo.env->DeleteLocalRef(jstringSummary);
        minfo.env->DeleteLocalRef(minfo.classID);
    }
}

//void SocialUtils::destroyForAndroid()
//{
//    JniMethodInfo jmi;
//    bool ishave = JniHelper::getStaticMethodInfo(jmi,Utils::getAndroidPackageInfo(),"onDestroy","()V");
//
//    if(ishave)
//    {
//        jmi.env->CallStaticVoidMethod(jmi.classID,jmi.methodID);
//    }
//}
#endif

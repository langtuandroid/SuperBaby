//
//  SocialUtils.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/05/27.
//
//

#ifndef __AbelLLExplorer__SocialUtils__
#define __AbelLLExplorer__SocialUtils__

#include "cocos2d.h"
#include <string>

USING_NS_CC;

class SocialUtils
{
public:
    static void shareToWXTimeline(const std::string& shareMsg);
    static void shareToWXSession(const std::string& shareMsg);
    static void shareToQQ(const char*, const char*);
    static void shareToQQZone(const char*, const char*);
    static void shareToWeibo(const char*, const char*);
    
public:
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    static void shareToWXTimelineForAndroid(const std::string& shareMessage);
    static void shareToWXSessionForAndroid(const std::string& shareMessage);
    static void shareToWeiboForAndroid(const std::string& shareMsg);
    static void shareToQQForAndroid(const char*, const char*);
#endif
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    static void shareToWXTimelineForIOS(const std::string& shareMessage);
    static void shareToWXSessionForIOS(const std::string& shareMessage);
    static void shareToQQForIOS(const char*, const char*);
    static void shareToQQZoneForIOS(const char*, const char*);
    static void shareToWeiboForIOS(const char*, const char*);
#endif
};

#endif /* defined(__AbelLLExplorer__SocialUtils__) */

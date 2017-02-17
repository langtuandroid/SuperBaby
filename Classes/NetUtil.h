//
//  NetUtil.h
//  superBaby
//
//  Created by adinnet on 2015/08/17.
//
//  网络工具类
//

#ifndef __superBaby__NetUtil__
#define __superBaby__NetUtil__

#include <stdio.h>

class NetUtil
{
public:
    static bool isConnectionAvailable();
    static bool isWIFIEnabled();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    static bool isConnectionAvailableAndroid();
    static bool isWIFIEnabledAndroid();
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    static bool isConnectionAvailableIos();
    static bool isWIFIEnabledIos();
#endif
};

#endif /* defined(__superBaby__NetUtil__) */

//
//  NetUtil.m
//  superBaby
//
//  Created by adinnet on 2015/8/17.
//

#import <Foundation/Foundation.h>
#include "NetUtil.h"
#import "Reachability.h"

#define TEST_URL    @"www.baidu.com"

bool NetUtil::isConnectionAvailableIos()
{
    BOOL isExistenceNetwork = YES;
    Reachability *reach = [Reachability reachabilityWithHostName:TEST_URL];
    switch ([reach currentReachabilityStatus]) {
        case NotReachable:
            isExistenceNetwork = NO;
            //NSLog(@"notReachable");
            break;
        case ReachableViaWiFi:
            isExistenceNetwork = YES;
            //NSLog(@"WIFI");
            break;
        case ReachableViaWWAN:
            isExistenceNetwork = YES;
            //NSLog(@"3G");
            break;
    }
    return isExistenceNetwork == YES ? true : false;
}

// 检测WIFI是否可用
bool NetUtil::isWIFIEnabledIos()
{
    return ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] != NotReachable);
}

#import <UIKit/UIKit.h>
#import "WXApi.h"
#import <TencentOpenAPI/TencentOAuth.h>
#import "WeiboSDK.h"

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate, WXApiDelegate,
TencentSessionDelegate, WeiboSDKDelegate>
{
    UIWindow *window;
    TencentOAuth* _tencentOAuth;
    NSString* _wbtoken;
}

@property(nonatomic, readonly) RootViewController* viewController;

@end


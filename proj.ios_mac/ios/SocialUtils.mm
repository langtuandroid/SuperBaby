//
//  SocialUtils.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/05/27.
//
//

#include "SocialUtils.h"

#import <Foundation/Foundation.h>
#import "WXApi.h"
#import "WeiboSDK.h"
#import <TencentOpenAPI/QQApiInterface.h>

static NSString* SHARE_LINK = @"http://www.abelclass.com/";

#define SHARE_ICON_URL  @"http://abelclass.ufile.ucloud.com.cn/icon.png"

void SocialUtils::shareToWXTimelineForIOS(const std::string& shareMessage)
{
    if ([WXApi isWXAppInstalled] )
    {
        WXMediaMessage *message = [WXMediaMessage message];
        message.title = [NSString stringWithUTF8String:shareMessage.c_str()];
        message.description = [NSString stringWithUTF8String:shareMessage.c_str()];
        [message setThumbImage:[UIImage imageNamed:@"Icon-152.png"]];
        WXWebpageObject *ext = [WXWebpageObject object];
        ext.webpageUrl = SHARE_LINK;
        message.mediaObject = ext;
        SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init]autorelease];
        req.bText = NO;
        req.message = message;
        req.scene = WXSceneTimeline; // 微信朋友圈
        
        [WXApi sendReq:req];
    }
}

void SocialUtils::shareToWXSessionForIOS(const std::string& shareMessage)
{
    if ([WXApi isWXAppInstalled] )
    {
        WXMediaMessage *message = [WXMediaMessage message];
        
        message.title = [NSString stringWithUTF8String:"超能宝贝"];
        message.description = [NSString stringWithUTF8String:shareMessage.c_str()];
        [message setThumbImage:[UIImage imageNamed:@"Icon-152.png"]];
        WXWebpageObject *ext = [WXWebpageObject object];
        ext.webpageUrl = SHARE_LINK;
        message.mediaObject = ext;
        SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init]autorelease];
        req.bText = NO;
        req.message = message;
        req.scene = WXSceneSession; // 微信好友
        
        [WXApi sendReq:req];
    }
}

void SocialUtils::shareToQQForIOS(const char* pTitle, const char* pSummary)
{
    NSString *title = [NSString stringWithUTF8String:pTitle];
    NSString *summary = [NSString stringWithUTF8String:pSummary];
    NSString *previewImageUrl = SHARE_ICON_URL;
    QQApiNewsObject *newsObj = [QQApiNewsObject
                                objectWithURL:[NSURL URLWithString:SHARE_LINK]
                                title:title
                                description:summary
                                previewImageURL:[NSURL URLWithString:previewImageUrl]];
    SendMessageToQQReq *req = [SendMessageToQQReq reqWithContent:newsObj];
    [QQApiInterface sendReq:req];
}

void SocialUtils::shareToQQZoneForIOS(const char* pTitle, const char* pSummary)
{
    NSString *title = [NSString stringWithUTF8String:pTitle];
    NSString *summary = [NSString stringWithUTF8String:pSummary];
    NSString *previewImageUrl = SHARE_ICON_URL;
    QQApiNewsObject *newsObj = [QQApiNewsObject
                                objectWithURL:[NSURL URLWithString:SHARE_LINK]
                                title:title
                                description:summary
                                previewImageURL:[NSURL URLWithString:previewImageUrl]];
    SendMessageToQQReq *req = [SendMessageToQQReq reqWithContent:newsObj];
    [QQApiInterface SendReqToQZone:req];
}

void SocialUtils::shareToWeiboForIOS(const char* pTitle, const char* pDescription)
{
    NSString* iconPath = [[NSBundle mainBundle] pathForResource:@"Icon" ofType:@"png"];
    
    WBWebpageObject *webPageObject = [WBWebpageObject object];
    webPageObject.objectID = @"uniqueid1";
    webPageObject.title = [NSString stringWithUTF8String:pTitle];
    webPageObject.description = [NSString stringWithUTF8String:pDescription];
    webPageObject.webpageUrl = SHARE_LINK;
    webPageObject.thumbnailData = [NSData dataWithContentsOfFile:iconPath];
    
    WBMessageObject *msg = [WBMessageObject message];
    msg.text = [NSString stringWithUTF8String:pDescription];
    msg.mediaObject = webPageObject;
    WBSendMessageToWeiboRequest *req = [WBSendMessageToWeiboRequest requestWithMessage:msg];
    [WeiboSDK sendRequest:req];
}

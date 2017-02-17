#include "AppDelegate.h"

#include "SimpleAudioEngine.h"
#include "Player.h"
#include "LogoScene.h"
#include "StageDataManager.h"
#include "NpcDataManager.h"
#include "AppConfig.h"
#include "MyAudioEngine.h"
#include "MobClickCpp.h"

USING_NS_CC;
using namespace CocosDenshion;

#define RH  1536    // 资源分辨率高
#define DW  2048    // 设计分辨率宽
#define DH  1536    // 设计分辨率高

#define UMENG_APP_KEY       "5591f81c67e58e4cc1000215"

/*
 *  预加载
 */
static void preload()
{
    // 加载玩家数据
    Player::getInstance()->loadPlayerData();
    // 加载应用程序配置数据
    AppConfig::getInstance()->loadConfigData();
    // 加载关卡配置文件数据
    StageDataManager::getInstance()->loadStageConfig();
    // 加载过关社交共享文本
    StageDataManager::getInstance()->loadSocialShareText();
    // 用户收集 npc 数据
    NpcDataManager::getInstance()->loadNpcData();
}

/*
 *  constructor
 */
AppDelegate::AppDelegate()
{
    preload();
}

/*
 *  destructor
 */
AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::getInstance()->end();
    umeng::MobClickCpp::end();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }
    
    // 设置缩放因子（因为是横版，所以采用 资源分辨率高 / 设计分辨率高 ）
    director->setContentScaleFactor(RH / DH);
    
    // 设计设计分辨率大小及策略（因为是横版，所以采用 ResolutionPolicy::FIXED_HEIGHT）
    glview->setDesignResolutionSize(DW, DH, ResolutionPolicy::FIXED_HEIGHT);
    
    // turn on display FPS
    //    director->setDisplayStats(true);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    //
    /*
     *
     *  xiaomiStore     小米
     *  qihooStore      奇虎360
     *  baiduStore      百度
     *  91Store         91助手
     *  androidStore    安卓市场
     *  useStore        应用宝
     *  peaStore        豌豆夹
     *  huaweiStore     华为市场
     *  lenovoStore     联想市场
     *  yuzhuang        预装
     */
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    MOBCLICKCPP_START_WITH_APPKEY_AND_CHANNEL(UMENG_APP_KEY, "AppStore");
#else
    MOBCLICKCPP_START_WITH_APPKEY_AND_CHANNEL(UMENG_APP_KEY, "yuzhuang");
#endif
    
    // 进入欢迎界面
    director->runWithScene(LogoScene::create());
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    MyAudioEngine::getInstance()->pauseAllAudio();
    
    //友盟
    umeng::MobClickCpp::applicationDidEnterBackground();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    if (AppConfig::getInstance()->getBackMusicOpen()) {
        MyAudioEngine::getInstance()->resumeAllAudio();
    }
    
    //友盟
    umeng::MobClickCpp::applicationWillEnterForeground();
}

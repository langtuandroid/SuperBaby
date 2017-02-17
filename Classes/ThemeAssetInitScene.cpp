//
//  ThemeAssetInitScene.cpp
//  superBaby
//
//  Created by adinnet on 2015/08/13.
//
//

#include "ThemeAssetInitScene.h"
#include "ThemeIntroScene.h"
#include "AppConfig.h"
#include "ThemeSelScene.h"
#include "NetUtil.h"
#include "UpdateAppDialog.h"
#include "Actor.h"
#include "GafAssetCache.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define LOADING_BAR_TAG     1001
#define FONT_FILE           "fonts/DFGBHBC_0.TTC"

// 用于切换下载进度 gaf 动画
int ThemeAssetInitScene::m_actorIndex = 0;

/*
 *  constructor
 */
ThemeAssetInitScene::ThemeAssetInitScene()
{
    m_actorNodePos[0] = Vec2(-218.0f, 581.0f);
    m_actorNodePos[1] = Vec2(-221.0f, 589.0f);
    m_actorNodePos[2] = Vec2(-221.0f, 589.0f);
    m_actorNodePos[3] = Vec2(-242.0f, 661.0f);
    m_actorSize[0] = Size(360.0f, 360.0f);
    m_actorSize[1] = Size(360.0f, 380.0f);
    m_actorSize[2] = Size(360.0f, 340.0f);
    m_actorSize[3] = Size(360.0f, 360.0f);
    m_actorFile[0] = "res/common/loading/loading01/loading01.gaf";
    m_actorFile[1] = "res/common/loading/loading02/loading02.gaf";
    m_actorFile[2] = "res/common/loading/loading03/loading03.gaf";
    m_actorFile[3] = "res/common/loading/loading04/loading04.gaf";
}

/*
 * destructor
 */
ThemeAssetInitScene::~ThemeAssetInitScene()
{
    unschedule("UpdateLabelHint");
    destroyAssetsManager();
    GafAssetCache::getInstance()->removeGafAssetForKey(m_currGafFile);
}

Scene* ThemeAssetInitScene::createWithThemeIndex(const int themeIndex)
{
    ThemeAssetInitScene* pScene = new ThemeAssetInitScene();
    if (pScene && pScene->initWithThemeIndex(themeIndex))
    {
        pScene->autorelease();
        return pScene;
    } else {
        CC_SAFE_DELETE(pScene);
        return nullptr;
    }
}

bool ThemeAssetInitScene::initWithThemeIndex(const int themeIndex)
{
    m_themeIndex = themeIndex;
    return true;
}

void ThemeAssetInitScene::startDownLoadStageRes()
{
    std::string localManifestFile = StringUtils::format("config/assetManager/project%02d.manifest", m_themeIndex + 1);
    std::string localStoragePath = FileUtils::getInstance()->getWritablePath() + StringUtils::format("theme%02d/", m_themeIndex + 1);
    _am = AssetsManagerEx::create(localManifestFile, localStoragePath);
    _am->retain();
    if (!_am->getLocalManifest()->isLoaded())
    {
        log("Fail to update assets, step skipped.");
        this->removeChildByTag(10);
    } else {
        auto pDirector = Director::getInstance();
        _amListener = EventListenerAssetsManagerEx::create(_am,
        std::bind(&ThemeAssetInitScene::eventListenAssetsManagerCB, this, std::placeholders::_1));
        pDirector->getEventDispatcher()->addEventListenerWithFixedPriority(_amListener, 1);
        _am->update();
    }
}

void ThemeAssetInitScene::onEnter()
{
    Scene::onEnter();
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    auto pBackgroundLayer = LayerColor::create( Color4B(19, 165, 212, 255) );
    pBackgroundLayer->cocos2d::Node::setContentSize( visibleSize );
    this->addChild(pBackgroundLayer, 0, 10);
    
    m_currGafFile = m_actorFile[m_actorIndex];
    auto actor = Actor::createWithGafFile(m_currGafFile);
    actor->setSize(m_actorSize[m_actorIndex]);
    actor->setPosAtNode(m_actorNodePos[m_actorIndex]);
    actor->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f - 180.0f);
    addChild(actor, 5);
    actor->setAnchorPoint(Vec2(0.5f, 0.0f));
    m_actorIndex = m_actorIndex < 3 ? m_actorIndex + 1 : 0;
    
    auto pLoadBg = ui::ImageView::create("res/ui/laoding-01.png");
    pLoadBg->ignoreAnchorPointForPosition( false );
    pLoadBg->setScaleX( 2.0f );
    pLoadBg->setAnchorPoint( Vec2::ANCHOR_MIDDLE );
    pLoadBg->setPosition( Vec2(pBackgroundLayer->getContentSize().width*0.5f, pBackgroundLayer->getContentSize().height*0.25f) );
    pBackgroundLayer->addChild( pLoadBg );
    
    auto pLoadBar = ui::LoadingBar::create("res/ui/laoding-02.png", 0);
    pLoadBar->ignoreAnchorPointForPosition( false );
    pLoadBar->cocos2d::Node::setScaleX( 2.0f );
    pLoadBar->setCapInsets(Rect(0, 0, 0, 0));
    pLoadBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    pLoadBar->setPosition( pLoadBg->getPosition() );
    pLoadBar->setName("LoadingBar");
    pBackgroundLayer->addChild(pLoadBar);
    
    auto pHint = Label::createWithSystemFont("亲爱滴主人,不要太心急哟...", "Arial", 50);
    pHint->setAnchorPoint(Vec2(0.0f, 0.5f));
    pHint->setColor(Color3B(255, 255, 255));
    pHint->setPosition(pLoadBg->getPositionX()-pHint->getContentSize().width*0.5f, pLoadBg->getPositionY() + pLoadBg->getContentSize().height*0.5f + pHint->getContentSize().height*0.5f);
    pBackgroundLayer->addChild(pHint);
    
    this->schedule([pHint](float dt)
    {
        std::string loadStr = pHint->getString();
       
        auto dotCount = cocos2d::StringUtils::getCharacterCountInUTF8String(loadStr) - 12;
        dotCount = dotCount % 3 + 1;
        std::string textStr = "亲爱滴主人,不要太心急哟";
        if (dotCount >= 0)
        {
            while (dotCount--)
            {
               textStr.append( "." );
            } /*while*/
        }
       pHint->setString(textStr);
    }, 0.5f, "UpdateLabelHint");
    startDownLoadStageRes();
}

void ThemeAssetInitScene::onExit()
{
    Scene::onExit();
}

void ThemeAssetInitScene::destroyAssetsManager()
{
    CC_SAFE_RELEASE_NULL(_am);
    if (_amListener != nullptr)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_amListener);
        _amListener = nullptr;
    }
}

void ThemeAssetInitScene::eventListenAssetsManagerCB(cocos2d::extension::EventAssetsManagerEx* ev)
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
            } else if (assetId == AssetsManagerEx::MANIFEST_ID)
            {
                str = StringUtils::format("Manifest file: %.2f", percent) + "%";
            } else {
                str = StringUtils::format("%.2f", percent) + "%";
                log("%.2f Percent", percent);
                
                auto pLoadingBar = dynamic_cast<ui::LoadingBar*>(ui::Helper::seekNodeByName(this, "LoadingBar"));
                if (pLoadingBar)
                {
                    if( percent > 0.0f )
                    {
                        pLoadingBar->setPercent(percent);
                    } else {
                        auto percent = pLoadingBar->getPercent();
                        if (percent < 70.0f) {
                            percent += 4.0f;
                        } else {
                            percent += 1.0f;
                        }
                        pLoadingBar->setPercent(percent);
                    }
                }
            }
        }
        break;
    case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
    case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
        {
            log("Fail to download manifest file, update skipped.");
            destroyAssetsManager();
        }
        break;
    case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
            log("ALREADY_UP_TO_DATE %s ", ev->getMessage().c_str());
            Director::getInstance()->replaceScene( ThemeSelScene::create() );
            break;
    case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
        {
            log("Update finished. %s", ev->getMessage().c_str());
            
            UserDefault::getInstance()->setIntegerForKey("curThemeIndex", m_themeIndex);
            UserDefault::getInstance()->flush();
            AppConfig::getInstance()->setStageAssetInited(m_themeIndex);
            char szBuffer[64];
            sprintf(szBuffer, "config/themeIntroScene/theme%02d.json", m_themeIndex + 1);
            Director::getInstance()->replaceScene(ThemeIntroScene::createWithStageConfig(szBuffer));
        }
        break;
    case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
        {
            log("Update failed. %s", ev->getMessage().c_str());
            if (++failCount < 5)
            {
                _am->downloadFailedAssets();
            } else {
                log("Reach maximum fail count, exit update process");
                failCount = 0;

                Director::getInstance()->replaceScene( ThemeSelScene::create() );
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

//
//  PreLoadUIManager.cpp
//  AbelLLExplorer
//
//  Created by YANGJIE on 2015/4/16.
//
//

#include "PreLoadUIManager.h"
#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "GafAssetCache.h"
#include "AppConfig.h"

#define PRELOADUI_DATA_FILE         "config/preloadui.json"
#define BG_LEFT_IMAGE               "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE              "res/common/bgRight.png"

//
PreLoadUIManager* PreLoadUIManager::getInstance()
{
    static PreLoadUIManager preLoadUIManager;
    return &preLoadUIManager;
}

/*
 *  constructor
 */
PreLoadUIManager::PreLoadUIManager()
{
    appendPreLoadRes();
}

/*
 *  destructor
 */
PreLoadUIManager::~PreLoadUIManager()
{
    destroyPreloadUI();
}

/*
 *  添加要预加载的资源
 */
void PreLoadUIManager::appendPreLoadRes()
{
    m_preLoadTextures.clear();
    m_preLoadGafs.clear();
    m_preLoadPlists.clear();
    
    std::string file_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(PRELOADUI_DATA_FILE);
    rapidjson::Document doc;
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    
    const rapidjson::Value &texturesValue = doc["textures"];
    for (unsigned int i = 0; i < texturesValue.Size(); i++) {
        m_preLoadTextures.push_back(texturesValue[i].GetString());
    } /*for*/
    if (AppConfig::getInstance()->getSceneOffsetX() > 0.0f)
    {
        m_preLoadTextures.push_back(BG_LEFT_IMAGE);
        m_preLoadTextures.push_back(BG_RIGHT_IMAGE);
    }
    
    const rapidjson::Value &gafsValue = doc["gafs"];
    for (unsigned int i = 0; i < gafsValue.Size(); i++) {
        m_preLoadGafs.push_back(gafsValue[i].GetString());
    } /*for*/
    
    const rapidjson::Value &plistsValue = doc["plists"];
    for (unsigned int i = 0; i < plistsValue.Size(); i++) {
        m_preLoadPlists.push_back(plistsValue[i].GetString());
    } /*for*/
}

/*
 *  释放预加载资源
 */
void PreLoadUIManager::destroyPreloadUI()
{
    auto pSpriteFrameCache = SpriteFrameCache::getInstance();
    for (auto &it : m_preLoadPlists)
    {
        pSpriteFrameCache->removeSpriteFramesFromFile(it);
    } /*for*/
    auto textureCache = Director::getInstance()->getTextureCache();
    for (auto &it : m_preLoadTextures) {
        textureCache->removeTextureForKey(it);
    } /*for*/
    auto pGafAssetCache = GafAssetCache::getInstance();
    for (auto &it : m_preLoadGafs) {
        pGafAssetCache->removeGafAssetForKey(it);
    } /*for*/
    m_preLoadTextures.clear();
    m_preLoadGafs.clear();
    m_preLoadPlists.clear();
}

/*
 *  开始预加载 ui 资源(异步)
 */
void PreLoadUIManager::startPreloadUI(std::function<void()> completedDelegate)
{
    m_completedDelegate = completedDelegate;
    preLoadTextures();
}

/*
 *  预加载纹理资源
 */
void PreLoadUIManager::preLoadTextures()
{
    auto textureCache = Director::getInstance()->getTextureCache();
    auto addImageAsyncCB = [this](Texture2D * pTexture2D) {
        static unsigned int texturesLoaded = 0;
        if (++texturesLoaded == m_preLoadTextures.size()) /*所有预加载纹理加载完毕*/ {
            preLoadGafs();
        }
    };
    for (auto const &it : m_preLoadTextures) {
        textureCache->addImageAsync(it, addImageAsyncCB);
    } /*for*/
}

/*
 *  预加载 gaf
 */
void PreLoadUIManager::preLoadGafs()
{
    auto pGafAssetCache = GafAssetCache::getInstance();
    auto gafPreloadCB = [this](gaf::GAFAsset* pGAFAsset)
    {
        if (++m_gafLoaded == m_preLoadGafs.size())
        {
            preLoadPlists();
        }
    };
    // 预加载 gaf 文件
    m_gafLoaded = 0;
    for (auto const &it : m_preLoadGafs) {
        pGafAssetCache->addGafAssetAsync(it, gafPreloadCB);
    } /*for*/
}

/*
 *  预加载 plist
 */
void PreLoadUIManager::preLoadPlists()
{
    auto pSpriteFrameCache = SpriteFrameCache::getInstance();
    for (auto const &it : m_preLoadPlists) {
        pSpriteFrameCache->addSpriteFramesWithFile(it);
    } /*for*/
    
    if (m_completedDelegate != nullptr) {
        m_completedDelegate();
    }
}

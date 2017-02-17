//
//  LoadingStageScene.cpp
//  AbelLLExplorer
//
//  Created by YANGJIE on 2015/04/01.
//
//

#include "LoadingStageScene.h"
#include "SelStageScene.h"
#include "StageDataManager.h"
#include "StageData.h"
#include "Player.h"
#include "Actor.h"
#include "GafAssetCache.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "MD5.h"
#include "unzip.h"
#include "NetUtil.h"

#define LOADINGBAR_PNG_FILE         "res/common/loading/Loadingtiao01.png"
#define LOADINGBAR_BOX_FILE         "res/common/loading/Loadingtiao02.png"

#define BUFFER_SIZE    8192
#define MAX_FILENAME   512

// 用于切换加载 gaf 动画
int LoadingStageScene::m_actorIndex = 0;

/*
 *  constuctor
 */
LoadingStageScene::LoadingStageScene()
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
 *  destructor
 */
LoadingStageScene::~LoadingStageScene()
{
    auto textureCache = Director::getInstance()->getTextureCache();
    textureCache->removeTextureForKey(LOADINGBAR_PNG_FILE);
    textureCache->removeTextureForKey(LOADINGBAR_BOX_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(m_currGafFile);
}

//
Scene* LoadingStageScene::createWithStageIndex(const unsigned int stageIndex, const unsigned int themeIndex,
                                               const bool isTrial)
{
    auto pRet = new LoadingStageScene();
    if (pRet && pRet->initWithStageIndex(stageIndex, themeIndex, isTrial))
    {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

//
bool LoadingStageScene::initWithStageIndex(const unsigned int stageIndex, const unsigned int themeIndex,
                                           const bool isTrial)
{
    m_stageIndex = stageIndex;
    m_themeIndex = themeIndex;
    m_isTrial = isTrial;
    
    // 获取关卡数据
    m_pStageData = StageDataManager::getInstance()->getStageData(stageIndex);
    if (m_pStageData == nullptr) {
        std::string stageCfgFile = StageDataManager::getInstance()->getStageConfigFile(m_stageIndex);
        m_pStageData = StageDataManager::getInstance()->loadStageDataWithJson(stageCfgFile);
    }
    CCASSERT(m_pStageData, "invalid stageData");
    
    return true;
}

void LoadingStageScene::onEnter()
{
    Scene::onEnter();
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    LayerColor* layerColor = LayerColor::create(Color4B(19, 165, 212, 255), visibleSize.width, visibleSize.height);
    this->addChild(layerColor);
    
    // 进度条
    m_pLoadingBar = ui::LoadingBar::create(LOADINGBAR_PNG_FILE);
    m_pLoadingBar->setPosition(Vec2(visibleSize.width / 2.0f, 360.0f));
    addChild(m_pLoadingBar, 3);
    m_pLoadingBar->setPercent(0);
    // 进度条外框
    auto loadingBoxSp = Sprite::create(LOADINGBAR_BOX_FILE);
    loadingBoxSp->setPosition(Vec2(visibleSize.width / 2.0f, 362.0f));
    addChild(loadingBoxSp, 2);
    
    // gaf 动画
    m_currGafFile = m_actorFile[m_actorIndex];
    auto actor = Actor::createWithGafFile(m_currGafFile);
    actor->setSize(m_actorSize[m_actorIndex]);
    actor->setPosAtNode(m_actorNodePos[m_actorIndex]);
    actor->setPosition(visibleSize.width / 2.0f, visibleSize.height / 2.0f - 180.0f);
    addChild(actor, 5);
    actor->setAnchorPoint(Vec2(0.5f, 0.0f));
    m_actorIndex = m_actorIndex < 3 ? m_actorIndex + 1 : 0;
    
    if (hasMissFile())
    {
        if (m_hasMissStageFile)
        {
            downloadStageFile();
        } else {
            downloadAudioFile();
        }
    } else {
        startLoad();
    }
}

//
void LoadingStageScene::onLoadCompletedCB(Texture2D* pTexture2D)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    m_loadedBgWidth += pTexture2D->getContentSize().width;
    m_loadedBgs++;
    
    auto rsp = m_pStageData->getRoundScroll(0);
    
    /*
     *  考虑的内存压力，这里加载的关卡纹理只要超过屏幕宽度就可以了
     */
    if (m_loadedBgWidth >= visibleSize.width + std::abs(rsp.m_scroll))
    {
        // 背景图预加载完毕
        m_imgs = m_pStageData->getPreloadImgs();
        m_gafs = m_pStageData->getPreloadGafs();
        m_preloadImgs = m_imgs.size();m_loadedImgs = 0;
        m_preloadGafs = m_gafs.size();m_loadedGafs = 0;
        m_resNum = m_preloadImgs + m_preloadGafs;
        m_addPercent = 100.0f / m_resNum;
        scheduleUpdate();
    } else {
        m_pLoadingBar->setPercent(m_pLoadingBar->getPercent() + 8.0f);
        // 继续加载背景纹理
        auto pTextureCache = Director::getInstance()->getTextureCache();
        std::vector<BgImgUnit> &bgImgArray = m_pStageData->getBgImgArray();
        auto pOnLoadCompletedCB = std::bind(&LoadingStageScene::onLoadCompletedCB, this, std::placeholders::_1);
        pTextureCache->addImageAsync(bgImgArray[m_loadedBgs].m_file, pOnLoadCompletedCB);
    }
}

void LoadingStageScene::update(float)
{
    if (m_imgs.size() > 0)
    {
        auto textureLoadedCB = [this](Texture2D* pTexture2D) {
            if (m_pLoadingBar->getPercent() >= 100.0f) {
                m_pLoadingBar->setPercent(100.0f);
            } else {
                m_pLoadingBar->setPercent(m_pLoadingBar->getPercent() + m_addPercent);
            }
            if (++m_loadedImgs == m_preloadImgs) {
                m_isLoadedCompleteImg = true;
            }
        };
        
        if (m_willLoadedImg < m_preloadImgs) {
            Director::getInstance()->getTextureCache()->addImageAsync(m_imgs[m_willLoadedImg++], textureLoadedCB);
        }
        
        if (m_isLoadedCompleteImg && !m_isLoadedCompleteGaf) {
            auto gafLoadedCB = [this](gaf::GAFAsset*  pGAFAsset)
            {
                if (m_pLoadingBar->getPercent() >= 100.0f) {
                    m_pLoadingBar->setPercent(100.0f);
                } else
                {
                    m_pLoadingBar->setPercent(m_pLoadingBar->getPercent() + m_addPercent);
                }
                m_loadedGafs++;
            };
            if (m_willLoadedGaf < m_preloadGafs) {
                GafAssetCache::getInstance()->addGafAssetAsync(m_gafs[m_willLoadedGaf++], gafLoadedCB);
            } else if (m_loadedGafs == m_preloadGafs) {
                m_isLoadedCompleteGaf = true;
            }
        }
        if (m_isLoadedCompleteImg && m_isLoadedCompleteGaf)
        {
            unscheduleUpdate();
            enterStage();
        }
    } else {
        if (m_gafs.size() > 0)
        {
            auto gafLoadedCB = [this](gaf::GAFAsset*  pGAFAsset)
            {
                if (m_pLoadingBar->getPercent() >= 100.0f) {
                    m_pLoadingBar->setPercent(100.0f);
                } else {
                    m_pLoadingBar->setPercent(m_pLoadingBar->getPercent() + m_addPercent);
                }
                m_loadedGafs++;
            };
            if (m_willLoadedGaf < m_preloadGafs) {
                GafAssetCache::getInstance()->addGafAssetAsync(m_gafs[m_willLoadedGaf++], gafLoadedCB);
            } else if (m_loadedGafs == m_preloadGafs) {
                m_isLoadedCompleteGaf = true;
            }
            if (m_isLoadedCompleteGaf)
            {
                unscheduleUpdate();
                enterStage();
            }
        } else {
            enterStage();
        }
    }
}

void LoadingStageScene::enterStage()
{
    m_pLoadingBar->setPercent(100);
    scheduleOnce([this](float dt)
    {
        Player::getInstance()->enterStage(m_stageIndex, m_isTrial);
    }, 0.6f, "enterStage");
}

bool LoadingStageScene::hasMissFile()
{
    auto pFileUtil = FileUtils::getInstance();
    do
    {
        if (m_stageIndex < 12) {
            break;
        }
        
        char szBuffer[256];
        sprintf(szBuffer, "config/stage/stageFileList%02d.json", m_stageIndex + 1);
        std::string filePath = pFileUtil->fullPathForFilename(szBuffer);
        std::string contentStr = pFileUtil->getStringFromFile(filePath);
        rapidjson::Document doc;
        doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
        if (doc.HasParseError())
        {
            cocos2d::log("GetParseError %s\n", doc.GetParseError());
            RAPIDJSON_ASSERT(!doc.HasParseError());
        }
        
        const rapidjson::Value &remoteURLValue = doc["remoteURL"];
        const rapidjson::Value &stageRemoteURLValue = remoteURLValue["stage"];
        const rapidjson::Value &audioRemoteURLValue = remoteURLValue["audio"];
        m_stageRemoteURL = stageRemoteURLValue["url"].GetString();
        m_stageRemoteFileMD5 = stageRemoteURLValue["md5"].GetString();
        m_audioRemoteURL = audioRemoteURLValue["url"].GetString();
        m_audioRemoteFileMD5 = audioRemoteURLValue["md5"].GetString();
        
        const rapidjson::Value &filesValue = doc["files"];
        const rapidjson::Value &stageFilesValue = filesValue["stage"];
        for (unsigned int i = 0; i < stageFilesValue.Size(); i++)
        {
            const rapidjson::Value &stageFileValue = stageFilesValue[i];
            if (!pFileUtil->isFileExist(stageFileValue.GetString()))
            {
                m_hasMissStageFile = true;
                break;
            }
        } /*for*/
        
        const rapidjson::Value &audioFilesValue = filesValue["audio"];
        for (unsigned int i = 0; i < audioFilesValue.Size(); i++)
        {
            const rapidjson::Value &audioFileValue = audioFilesValue[i];
            if (!pFileUtil->isFileExist(audioFileValue.GetString()))
            {
                m_hasMissAudioFile = true;
                break;
            }
        } /*for*/
        break;
    } while (false);
    
    return m_hasMissStageFile || m_hasMissAudioFile;
}

void LoadingStageScene::startLoad()
{
    // 获取关卡背景图纹理文件
    std::vector<BgImgUnit> &bgImgArray = m_pStageData->getBgImgArray();
    m_numberOfWantLoadedRes = (unsigned int)bgImgArray.size();
    
    //
    auto pOnLoadCompletedCB = std::bind(&LoadingStageScene::onLoadCompletedCB, this, std::placeholders::_1);
    auto pTextureCache = Director::getInstance()->getTextureCache();
    m_loadedBgs = 0;
    m_loadedBgWidth = 0.0f;
    pTextureCache->addImageAsync(bgImgArray[0].m_file, pOnLoadCompletedCB);
}

void LoadingStageScene::onStageFileDownloadedCB(HttpClient *sender, HttpResponse *response)
{
    if (!response){return;}
    unschedule(CC_SCHEDULE_SELECTOR(LoadingStageScene::downloadScdule));
    
    long statusCode = response->getResponseCode();
    std::string tag = response->getHttpRequest()->getTag();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %ld, tag = %s", statusCode, tag.c_str());
    if (!response->isSucceed())
    {
        log("response code: %ld", statusCode);
        log("response failed, error buffer: %s", response->getErrorBuffer());
        
        cocos2d::MessageBox("下载失败，请检查网络!", "超能宝贝");
        retToSelStage();
        
        return;
    }
    
    // dump data
    std::vector<char> *buffer = response->getResponseData();
    int bufferSize = (int)buffer->size();

    MD5 md5(buffer->data(), bufferSize);
    std::string downloadFileMD5 = md5.toString();
    
//    cocos2d::log("m_stageRemoteFileMD5 = %s", m_stageRemoteFileMD5.c_str());
//    cocos2d::log("stage md5 = %s bufferSize = %d", md5.toString().c_str(), bufferSize);
    
    if (m_stageRemoteFileMD5 == downloadFileMD5)
    {
        std::string writeFile = FileUtils::getInstance()->getWritablePath();
        writeFile += StringUtils::format("theme%02d/", m_themeIndex + 1);
        writeFile += StringUtils::format("res/stage0%02d.zip", m_stageIndex + 1);
        
        FILE *out = fopen(writeFile.c_str(), "wb");
        if (!out)
        {
            CCLOG("can not open destination file %s", writeFile.c_str());
            return;
        }
        fwrite((void*)buffer->data(), bufferSize, 1, out);
        fclose(out);
        
        if (decompress(writeFile))
        {
            if (m_hasMissAudioFile)
            {
                downloadAudioFile();
            } else {
                startLoad();
            }
        }
    } else {
        cocos2d::log("stage md5 checksum fail...");
        // md5 校验失败，重新下载文件
        downloadStageFile();
    }
}

void LoadingStageScene::downloadStageFile()
{
    if (NetUtil::isConnectionAvailable())
    {
        HttpRequest* requestStageFile = new HttpRequest();
        requestStageFile->setUrl(m_stageRemoteURL.c_str());
        requestStageFile->setRequestType(HttpRequest::Type::GET);
        requestStageFile->setTag("requestStageFile");
        requestStageFile->setResponseCallback(CC_CALLBACK_2(LoadingStageScene::onStageFileDownloadedCB,
                                                            this));
        HttpClient::getInstance()->send(requestStageFile);
        requestStageFile->release();
        
        schedule(CC_SCHEDULE_SELECTOR(LoadingStageScene::downloadScdule),
                 0.3f, CC_REPEAT_FOREVER, 0.0f);
    } else {
        /* 网络不通，返回到关卡选择 */
        cocos2d::MessageBox("需联网下载资源，请检查网络!", "超能宝贝");
        retToSelStage();
    }
}

void LoadingStageScene::retToSelStage()
{
    char szBuffer[64];
    sprintf(szBuffer, "config/selStage/selStage%03d.json", m_themeIndex + 1);
    Director::getInstance()->replaceScene(SelStageScene::createWithStageConfig(szBuffer));
}

void LoadingStageScene::onAudioFileDownloadedCB(HttpClient *sender, HttpResponse *response)
{
    if (!response){return;}
    unschedule(CC_SCHEDULE_SELECTOR(LoadingStageScene::downloadScdule));
    
    long statusCode = response->getResponseCode();
    std::string tag = response->getHttpRequest()->getTag();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %ld, tag = %s", statusCode, tag.c_str());
    if (!response->isSucceed())
    {
        log("response code: %ld", statusCode);
        log("response failed, error buffer: %s", response->getErrorBuffer());
        
        cocos2d::MessageBox("下载失败，请检查网络!", "超能宝贝");
        retToSelStage();
        return;
    }
    
    // dump data
    std::vector<char> *buffer = response->getResponseData();
    int bufferSize = (int)buffer->size();
    
    MD5 md5(buffer->data(), bufferSize);
    std::string downloadFileMD5 = md5.toString();
    
//    cocos2d::log("m_audioRemoteFileMD5 = %s", m_audioRemoteFileMD5.c_str());
//    cocos2d::log("audio bufferSize = %d md5 = %s", bufferSize, downloadFileMD5.c_str());
    
    if (m_audioRemoteFileMD5 == downloadFileMD5)
    {
        std::string writeFile = FileUtils::getInstance()->getWritablePath();
        writeFile += StringUtils::format("theme%02d/", m_themeIndex + 1);
        writeFile += StringUtils::format("res/audio/theme%02d/stage0%02d.zip", m_themeIndex + 1, m_stageIndex + 1);
        FILE *out = fopen(writeFile.c_str(), "wb");
        if (!out)
        {
            CCLOG("can not open destination file %s", writeFile.c_str());
            return;
        }
        fwrite((void*)buffer->data(), bufferSize, 1, out);
        fclose(out);
        
        if (decompress(writeFile))
        {
            startLoad();
        }
    } else {
        cocos2d::log("audio md5 checksum fail...");
        downloadAudioFile();
    }
}

void LoadingStageScene::downloadAudioFile()
{
    if (NetUtil::isConnectionAvailable())
    {
        HttpRequest* requestAudioFile = new HttpRequest();
        requestAudioFile->setUrl(m_audioRemoteURL.c_str());
        requestAudioFile->setRequestType(HttpRequest::Type::GET);
        requestAudioFile->setTag("requestAudioFile");
        requestAudioFile->setResponseCallback(CC_CALLBACK_2(LoadingStageScene::onAudioFileDownloadedCB,
                                                            this));
        HttpClient::getInstance()->send(requestAudioFile);
        requestAudioFile->release();
        
        schedule(CC_SCHEDULE_SELECTOR(LoadingStageScene::downloadScdule),
                 0.3f, CC_REPEAT_FOREVER, 0.0f);
    } else {
        /* 网络不通，返回到关卡选择 */
        cocos2d::MessageBox("需联网下载资源，请检查网络!", "超能宝贝");
        retToSelStage();
    }
}

std::string LoadingStageScene::basename(const std::string& path) const
{
    size_t found = path.find_last_of("/\\");
    
    if (std::string::npos != found)
    {
        return path.substr(0, found);
    }
    else
    {
        return path;
    }
}

bool LoadingStageScene::decompress(const std::string &zip)
{
    // Find root path for zip file
    size_t pos = zip.find_last_of("/\\");
    if (pos == std::string::npos)
    {
        CCLOG("LoadingStageScene : no root path specified for zip file %s\n", zip.c_str());
        return false;
    }
    const std::string rootPath = zip.substr(0, pos + 1);
    
    // Open the zip file
    unzFile zipfile = unzOpen(zip.c_str());
    if (!zipfile)
    {
        CCLOG("LoadingStageScene : can not open downloaded zip file %s\n", zip.c_str());
        return false;
    }
    
    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
    {
        CCLOG("LoadingStageScene : can not read file global info of %s\n", zip.c_str());
        unzClose(zipfile);
        return false;
    }
    
    // Buffer to hold data read from the zip file
    char readBuffer[BUFFER_SIZE];
    // Loop to extract all files.
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i)
    {
        // Get info about current file.
        unz_file_info fileInfo;
        char fileName[MAX_FILENAME];
        
        if (unzGetCurrentFileInfo(zipfile,
                                  &fileInfo,
                                  fileName,
                                  MAX_FILENAME,
                                  NULL,
                                  0,
                                  NULL,
                                  0) != UNZ_OK)
        {
            CCLOG("LoadingStageScene : can not read compressed file info\n");
            unzClose(zipfile);
            return false;
        }
        
        const std::string fullPath = rootPath + fileName;
        // Check if this entry is a directory or a file.
        const size_t filenameLength = strlen(fileName);
        if (fileName[filenameLength-1] == '/')
        {
            //There are not directory entry in some case.
            //So we need to create directory when decompressing file entry
            if (!FileUtils::getInstance()->createDirectory(basename(fullPath)))
            {
                // Failed to create directory
                CCLOG("LoadingStageScene : can not create directory %s\n", fullPath.c_str());
                unzClose(zipfile);
                return false;
            }
        }
        else
        {
            // Entry is a file, so extract it.
            // Open current file.
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                CCLOG("LoadingStageScene : can not extract file %s\n", fileName);
                unzClose(zipfile);
                return false;
            }
            
            // Create a file to store current file.
            FILE *out = fopen(fullPath.c_str(), "wb");
            if (!out)
            {
                CCLOG("LoadingStageScene : can not create decompress destination file %s\n", fullPath.c_str());
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            
            // Write current file content to destinate file.
            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
                if (error < 0)
                {
                    CCLOG("LoadingStageScene : can not read zip file %s, error code is %d\n", fileName, error);
                    fclose(out);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    return false;
                }
                
                if (error > 0)
                {
                    fwrite(readBuffer, error, 1, out);
                }
            } while(error > 0);
            fclose(out);
        }
        unzCloseCurrentFile(zipfile);
        
        // Goto next entry listed in the zip file.
        if ((i+1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                CCLOG("AssetsManagerEx : can not read next file for decompressing\n");
                unzClose(zipfile);
                return false;
            }
        }
    }
    
    unzClose(zipfile);
    return true;
}

void LoadingStageScene::downloadScdule(float dt)
{
    m_pLoadingBar->setPercent(m_pLoadingBar->getPercent() + 1.0f);
}



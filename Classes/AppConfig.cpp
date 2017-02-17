//
//  AppConfig.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/04.
//
//

#include "cocos2d.h"
#include "AppConfig.h"
#include "json/rapidjson.h"
#include "json/document.h"

#define CONFIG_DATA_FILE     "config/appConfig.json"

USING_NS_CC;

/*
 *  constructor
 */
AppConfig::AppConfig() {}

/*
 * destructor
 */
AppConfig::~AppConfig() {}

AppConfig* AppConfig::getInstance()
{
    static AppConfig appConfig;
    return &appConfig;
}

void AppConfig::createAssetPath()
{
    //
    std::string audioPath = "";
    std::string resPath = "";
    
    std::string writePath = FileUtils::getInstance()->getWritablePath();

    std::string themeAssetPath = writePath + "theme02/";
    FileUtils::getInstance()->createDirectory(themeAssetPath);
    resPath = themeAssetPath + "res/";
    FileUtils::getInstance()->createDirectory(resPath);
    audioPath = resPath + "audio/theme02/";
    FileUtils::getInstance()->createDirectory(audioPath);
    FileUtils::getInstance()->addSearchPath(themeAssetPath);
    
    themeAssetPath = writePath + "theme03/";
    FileUtils::getInstance()->createDirectory(themeAssetPath);
    resPath = themeAssetPath + "res/";
    FileUtils::getInstance()->createDirectory(resPath);
    audioPath = resPath + "audio/theme03/";
    FileUtils::getInstance()->createDirectory(audioPath);
    FileUtils::getInstance()->addSearchPath(themeAssetPath);
    
    themeAssetPath = writePath + "theme04/";
    FileUtils::getInstance()->createDirectory(themeAssetPath);
    resPath = themeAssetPath + "res/";
    FileUtils::getInstance()->createDirectory(resPath);
    audioPath = resPath + "audio/theme04/";
    FileUtils::getInstance()->createDirectory(audioPath);
    FileUtils::getInstance()->addSearchPath(themeAssetPath);
}

/*
 *  加载配置文件
 */
void AppConfig::loadConfigData()
{
    std::string file_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(CONFIG_DATA_FILE).c_str();
    rapidjson::Document doc;
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);
    
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    RAPIDJSON_ASSERT(doc.IsObject());
    
    m_bgScrollSpeed = (float)doc["bgScrollSpeed"].GetInt();
    m_overRoundCelebrateDurdation = (float)doc["overRoundCelebrateDurdation"].GetInt();
    
    const rapidjson::Value &leadAnsCorJsonValues = doc["ansCorrEffect"];
    if (!leadAnsCorJsonValues.IsNull()) {
        for (unsigned int i = 0; i < leadAnsCorJsonValues.Size(); i++) {
            appendLeadAnsCorEff(leadAnsCorJsonValues[i].GetString());
        }
    }
    
    const rapidjson::Value &leadAnsErrJsonValues = doc["ansErrEffect"];
    if (!leadAnsErrJsonValues.IsNull()) {
        for (unsigned int i = 0; i < leadAnsErrJsonValues.Size(); i++) {
            appendLeadAnsErrEff(leadAnsErrJsonValues[i].GetString());
        }
    }
    
    const rapidjson::Value &leadWalkJsonValues = doc["walkEffect"];
    if (!leadWalkJsonValues.IsNull()) {
        for (unsigned int i = 0; i < leadWalkJsonValues.Size(); i++) {
            appendLeadWalkEff(leadWalkJsonValues[i].GetString());
        }
    }
    
    const rapidjson::Value &leadNpcCollectJsonValues = doc["npcCollectEffect"];
    if (!leadNpcCollectJsonValues.IsNull())
    {
        for (unsigned int i = 0; i < leadNpcCollectJsonValues.Size(); i++)
        {
            appendLeadNpcCollectEff(leadNpcCollectJsonValues[i].GetString());
        }
    }
}

void AppConfig::appendLeadAnsCorEff(const std::string effStr)
{
    m_leadAnsCorEff.push_back(effStr);
}

void AppConfig::appendLeadAnsErrEff(const std::string effStr)
{
    m_leadAnsErrEff.push_back(effStr);
}

void AppConfig::appendLeadWalkEff(const std::string effStr)
{
    m_leadWaikEff.push_back(effStr);
}

void AppConfig::appendLeadNpcCollectEff(const std::string effStr)
{
    m_leadNpcCollectEff.push_back(effStr);
}

std::string AppConfig::getLeadAnsCorEff()
{
    
    if (m_leadAnsCorEff.size() == 0)
        return "";
    
    srand((unsigned)time(0));
    int effIndex = rand()%m_leadAnsCorEff.size();
    return m_leadAnsCorEff[effIndex];
}

std::string AppConfig::getLeadAnsErrEff()
{
    if (m_leadAnsErrEff.size() == 0)
        return "";
    
    srand((unsigned)time(0));
    int effIndex = rand()%m_leadAnsErrEff.size();
    return m_leadAnsErrEff[effIndex];
}

std::string AppConfig::getLeadWalkEff()
{
    if (m_leadWaikEff.size() == 0)
        return "";
    
    srand((unsigned)time(0));
    int effIndex = rand()%m_leadWaikEff.size();
    return m_leadWaikEff[effIndex];
}

//
std::string AppConfig::getLeadNpcCollectEff()
{
    if (m_leadNpcCollectEff.size() == 0)
        return "";
    
    srand((unsigned)time(0));
    int effIndex = rand() % m_leadNpcCollectEff.size();
    return m_leadNpcCollectEff[effIndex];
}

/**/
void AppConfig::setStageClearIndex(const int stageClearIndex)
{
    if (!isDebugMode())
    {
        m_stageClearIndex = stageClearIndex;
    }
}

/*
 *  计算并获取场景水平偏移量
 */
float AppConfig::getSceneOffsetX()
{
    float stageOffsetX = 0.0f;
    auto visibleWidth = cocos2d::Director::getInstance()->getVisibleSize().width;
    auto stageSceneWidth = getDesignResolutionWidth();
    
    if (visibleWidth > stageSceneWidth)
    {
        int subWidth = visibleWidth - stageSceneWidth;
        int mod = subWidth % 2;
        stageOffsetX = (subWidth - mod) / 2;
    }
    return stageOffsetX;
}

bool AppConfig::isStageAssetInited(const int themeIndex)
{
//    if (themeIndex == 0) {
//        return true;
//    }
//    char szBuffer[64];
//    sprintf(szBuffer, "StageAssetInited%02d", themeIndex + 1);
//    return cocos2d::UserDefault::getInstance()->getBoolForKey(szBuffer, false);
    return true;
}

void AppConfig::setStageAssetInited(const int themeIndex)
{
    char szBuffer[64];
    sprintf(szBuffer, "StageAssetInited%02d", themeIndex + 1);
    cocos2d::UserDefault::getInstance()->setBoolForKey(szBuffer, true);
    cocos2d::UserDefault::getInstance()->flush();
}

bool AppConfig::isDebugMode() const
{
    return true;
}




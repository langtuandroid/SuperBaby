//
//  StageDataManager.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/2/28.
//
//

#include "StageDataManager.h"
#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "StageData.h"
#include "AppConfig.h"

#define STAGE_CONFIG_FILE   "config/stageConfig.json"
#define SOCIAL_CONFIG_FILE   "config/socialShare.json"

#define MAX_THEME_INDEX     4           // 最大主题索引
#define MAX_STAGE_INDEX     48          // 最大关卡索引

/*
 *  每个主题的已通关的最大索引号 key 值
 */
static const char* MAX_STAGE_CLEAR_KEY_OF_THEME[] =
{
    "MSCKOT1",
    "MSCKOT2",
    "MSCKOT3",
    "MSCKOT4"
};

/*
 *    获取单例对象
 */
StageDataManager* StageDataManager::getInstance()
{
    static StageDataManager stageDataManager;
    return &stageDataManager;
}

/*
 *  构造函数
 */
StageDataManager::StageDataManager()
{
    m_stageData.clear();
}

/*
 *  析构函数
 */
StageDataManager::~StageDataManager()
{
    destoryStageData();
}

/* 
 *  加载关卡配置文件信息
 */
void StageDataManager::loadStageConfig()
{
    m_stageConfigFiles.clear();
    
    std::string file_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(STAGE_CONFIG_FILE);
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    for (unsigned int i = 0; i < doc.Size(); i++)
    {
        // 获取关卡数据
        const rapidjson::Value &stageConfigFileValue = doc[i];
        m_stageConfigFiles.insert(std::make_pair(stageConfigFileValue["index"].GetInt(),
                                                 stageConfigFileValue["file"].GetString()));
    } /*for*/
}

/* 
 *  加载过关社交共享文本
 */
void StageDataManager::loadSocialShareText()
{
    m_stageSocialTexts.clear();
    
    std::string file_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(SOCIAL_CONFIG_FILE);
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);
    
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    for (unsigned int i = 0; i < doc.Size(); i++)
    {
        const rapidjson::Value &socialValue = doc[i];
        m_stageSocialTexts.insert(std::make_pair(socialValue["si"].GetInt(),
                                                 socialValue["sc"].GetString()));
    } /*for*/
}

/*
 *  根据关卡索引获取过关社交共享文本
 */
std::string StageDataManager::getSocialShareText(const int stageIndex)
{
    std::string str = "share";
    std::map<int, std::string>::iterator iter =  m_stageSocialTexts.find(stageIndex);
    if (iter != m_stageSocialTexts.end())
    {
        str = iter->second;
    }
    return str;
}

/*
 *  获取关卡配置文件
 */
std::string StageDataManager::getStageConfigFile(const int stageIndex)
{
    std::string stageConfigFile = "";
    auto it = m_stageConfigFiles.find(stageIndex);
    if (it != m_stageConfigFiles.end())
    {
        stageConfigFile = it->second;
    }
    return stageConfigFile;
}

/* 
 *  根据关卡数据(json)加载关卡数据
 */
StageData* StageDataManager::loadStageDataWithJson(const std::string& jsonFile)
{
    CCASSERT(!jsonFile.empty(), "invalid stage json file");
    
    std::string file_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(jsonFile);
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);

    rapidjson::Document stageDataDoc;
    stageDataDoc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (stageDataDoc.HasParseError())
    {
        cocos2d::log("GetParseError %s\n", stageDataDoc.GetParseError());
        RAPIDJSON_ASSERT(!stageDataDoc.HasParseError());
    }
    
    // 生成关卡数据
    auto pStageData = new StageData();
    
    // 关卡索引
    auto stageIndex = stageDataDoc["index"].GetInt();
    pStageData->setStageIndex(stageIndex);
    
    // 艾米和蓓蓓初始位置
    pStageData->setAimeePosX((float)stageDataDoc["AimeePosX"].GetInt());
    pStageData->setAimeePosY((float)stageDataDoc["AimeePosY"].GetInt());
    pStageData->setBeBeiPosX((float)stageDataDoc["BebeiPosX"].GetInt());
    pStageData->setBeBeiPosY((float)stageDataDoc["BebeiPosY"].GetInt());
    
    // 有 npc 收集的回合索引
    const rapidjson::Value &npcRoundJsonValues = stageDataDoc["npcRounds"];
    if (!npcRoundJsonValues.IsNull()) {
        for (unsigned int j = 0; j < npcRoundJsonValues.Size(); j++) {
            pStageData->appendNpcRoundIndex(npcRoundJsonValues[j].GetInt());
        } /*for*/
    }
    
    // 答对点击特效文件
    pStageData->setAnswerCorrectEffect(stageDataDoc["djcf"].GetString());
    
    //
    const rapidjson::Value &emptyRoundJsonValues = stageDataDoc["emptyRound"];
    if (!emptyRoundJsonValues.IsNull()) {
        for (unsigned int j = 0; j < emptyRoundJsonValues.Size(); j++) {
            pStageData->appendEmptyRoundIndex(emptyRoundJsonValues[j].GetInt());
        } /*for*/
    }
    
    // 过关社交分享音频
    pStageData->setShareAudio(stageDataDoc["saudio"].GetString());
    const rapidjson::Value &qaudioJsonValues = stageDataDoc["qaudio"];
    for (unsigned int j = 0; j < qaudioJsonValues.Size(); j++) {
        const rapidjson::Value &qaudioJsonValue = qaudioJsonValues[j];
        pStageData->appendQaudioFile(qaudioJsonValue["source"].GetString());
        pStageData->appendQaudioTime(qaudioJsonValue["time"].GetDouble());
    } /*for*/
    
    // 背景图片
    const rapidjson::Value &bgImgArrayData = stageDataDoc["bgImg"];
    for (unsigned int j = 0; j < bgImgArrayData.Size(); j++)
    {
        const rapidjson::Value & biuJsonValue = bgImgArrayData[j];
        BgImgUnit biu;
        if (1 == biuJsonValue["tileOrient"].GetUint()) /*水平*/ {
            biu = BgImgUnit(biuJsonValue["tileOrient"].GetUint(), biuJsonValue["file"].GetString());
        } else /* 垂直 */ {
            biu = BgImgUnit(biuJsonValue["tileOrient"].GetUint(),
                            biuJsonValue["file"].GetString(), biuJsonValue["upDown"].GetInt());
        }
        pStageData->appendBgImg(biu);
    } /*for*/
    
    // 前景图片
    const rapidjson::Value &foreImgArrayData = stageDataDoc["foreImg"];
    if (!foreImgArrayData.IsNull()) {
        for (unsigned int i = 0; i < foreImgArrayData.Size(); i++) {
            const rapidjson::Value &foreJsonValue = foreImgArrayData[i];
            ForeImgUnit fore;
            if (1 == foreJsonValue["tileOrient"].GetUint()) /*水平*/ {
                fore = ForeImgUnit(foreJsonValue["tileOrient"].GetUint(), foreJsonValue["file"].GetString());
            } else /* 垂直 */ {
                fore = ForeImgUnit(foreJsonValue["tileOrient"].GetUint(),
                                   foreJsonValue["file"].GetString(), foreJsonValue["upDown"].GetInt());
            }
            pStageData->appendForeImg(fore);
        }
    }
    
    // 主角移动参数
    const rapidjson::Value &LAMoveParaArray = stageDataDoc["LeadActorScroll"];
    RAPIDJSON_ASSERT(LAMoveParaArray.IsArray());
    for (unsigned int j = 0; j < LAMoveParaArray.Size(); j++)
    {
        const rapidjson::Value &laMoveJSonValue = LAMoveParaArray[j];
        unsigned int orient = laMoveJSonValue["orient"].GetUint();
        float scroll = (float)laMoveJSonValue["scroll"].GetInt();
        
        auto rsp = ScrollPara(orient == 1 ? ScrollOrient::SCROLL_HORIZONTAL : ScrollOrient::SCROLL_VERTICAL, scroll);
        // 保存到关卡数据
        pStageData->appendLAMovePara(rsp);
    } /*for*/
    
    // 回合滚动参数
    const rapidjson::Value &roundScrollArray = stageDataDoc["roundScroll"];
    RAPIDJSON_ASSERT(roundScrollArray.IsArray());
    for (unsigned int j = 0; j < roundScrollArray.Size(); j++)
    {
        const rapidjson::Value &roundScrollJSonValue = roundScrollArray[j];
        unsigned int orient = roundScrollJSonValue["orient"].GetUint();
        float scroll = (float)roundScrollJSonValue["scroll"].GetInt();
        
        auto rsp = ScrollPara(orient == 1 ? ScrollOrient::SCROLL_HORIZONTAL : ScrollOrient::SCROLL_VERTICAL, scroll);
        
        //
        if (2 == orient) /*垂直升降需要知道关联的背景图索引和垂直升降位移量 */ {
            CCASSERT(!roundScrollJSonValue["bgImgIndex"].IsNull(), "invalid data");
            CCASSERT(!roundScrollJSonValue["vscroll"].IsNull(), "invalid data");
            
            rsp.m_bgImgIndex = roundScrollJSonValue["bgImgIndex"].GetInt();
            rsp.m_vscroll = roundScrollJSonValue["vscroll"].GetInt();
        }
        
        // 保存到关卡数据
        pStageData->appendRoundScrollPara(rsp);
    } /*for*/
    
    // 回合文字
    const rapidjson::Value &allRoundTextsArrayData = stageDataDoc["roundText"];
    if (allRoundTextsArrayData.IsArray()) {
        for (unsigned int k = 0; k < allRoundTextsArrayData.Size(); k++)
        {
            const rapidjson::Value &roundTextUnitData = allRoundTextsArrayData[k];
            
            RoundTextUnit rtu;
            rtu.roundIndex = roundTextUnitData["roundIndex"].GetUint();
            rtu.fontSize = roundTextUnitData["fontSize"].GetInt();
            rtu.fontFile = roundTextUnitData["fontFile"].GetString();
            rtu.posX = roundTextUnitData["posX"].GetInt();
            rtu.posY = roundTextUnitData["posY"].GetInt();
            rtu.maxLineWidth = roundTextUnitData["maxLineWidth"].GetInt();
            rtu.text = roundTextUnitData["text"].GetString();
            
            // 保存
            pStageData->appendRoundTextUnit(rtu);
        } /*for*/
    }
    
    // 要预加载的纹理
    const rapidjson::Value &preLoadImgsArray = stageDataDoc["preloadImgs"];
    if (!preLoadImgsArray.IsNull()) {
        for (unsigned int i = 0; i < preLoadImgsArray.Size(); i++) {
            pStageData->appendPreLoadImg(preLoadImgsArray[i].GetString());
        } /*for*/
    }
    
    // 需要预加载的 gaf
    const rapidjson::Value &preLoadGafsArray = stageDataDoc["preloadGafs"];
    if (!preLoadGafsArray.IsNull()) {
        for (unsigned int i = 0; i < preLoadGafsArray.Size(); i++) {
            pStageData->appendPreLoadGaf(preLoadGafsArray[i].GetString());
        } /*for*/
    }
    
    // 保存关卡数据
    auto it = m_stageData.find(stageIndex);
    if  (it != m_stageData.end())
    {
        delete (it->second);        // 清除原有的关卡数据
    }
    m_stageData[stageIndex] = pStageData;
    return pStageData;
}

/*
 *  获取关卡数
 */
unsigned int StageDataManager::getNumberOfStages()
{
    return (unsigned int)m_stageConfigFiles.size();
}

/*
 *  获取关卡数据
 */
StageData* StageDataManager::getStageData(const unsigned int stageIndex)
{
    auto it = m_stageData.find(stageIndex);
    if (it != m_stageData.end())
    {
        return it->second;
    }
    return nullptr;
}

/*
 *  设置关卡数据
 */
void StageDataManager::setStageData(const unsigned int stageIndex, StageData* pStageData)
{
    CCASSERT(pStageData, "invalid stage data");
    m_stageData[stageIndex] = pStageData;
}

/*
 *   清除关卡数据
 */
void StageDataManager::destoryStageData()
{
    for (auto it : m_stageData)
    {
        delete it.second;
    } /*for*/
    m_stageData.clear();
}

/*
 *  返回true 表示指定关卡已被通过关
 */
bool StageDataManager::stageIsClear(const int stageIndex)
{
    CCASSERT(stageIndex < MAX_STAGE_INDEX && stageIndex >= 0, "invalid stage index");
    int stageOfTheme = stageIndex % 12;
    int themeIndex = (stageIndex - stageOfTheme) / 12;
    return stageOfTheme <= getMaxClearStageIndex(themeIndex);
}

/* 
 *  通关
 *  返回true 表示该指定关卡已被通过关
 */
bool StageDataManager::stageClear(const int stageIndex)
{
    CCASSERT(stageIndex < MAX_STAGE_INDEX && stageIndex >= 0, "invalid stage index");
    if (AppConfig::getInstance()->isDebugMode())
    {
        return true;
    }
    
    int stageIndexOfTheme = stageIndex % 12;
    int themeIndex = (stageIndex - stageIndexOfTheme) / 12;
    
    /* 该关卡是否以前已经通过关 */
    bool stageCleared = stageIsClear(stageIndex);
    if (!stageCleared)
    {
        /* 最大通关关卡索引 */
        auto maxClearStageIndex = getMaxClearStageIndex(themeIndex);
        if (stageIndex > maxClearStageIndex)
        {
            cocos2d::UserDefault::getInstance()->setIntegerForKey(MAX_STAGE_CLEAR_KEY_OF_THEME[themeIndex], stageIndexOfTheme);
            cocos2d::UserDefault::getInstance()->flush();
        }
    }
    return stageCleared;
}

/* 
 *  获取指定主题的最大已通关索引号
 */
int StageDataManager::getMaxClearStageIndex(const int themeIndex)
{
    CCASSERT(themeIndex < MAX_THEME_INDEX && themeIndex >= 0, "invalid themeIndex");
    return cocos2d::UserDefault::getInstance()->getIntegerForKey(MAX_STAGE_CLEAR_KEY_OF_THEME[themeIndex], -1);
}

//
void StageDataManager::manualSetMaxClearStageIndex(const int stageIndex)
{
    CCASSERT(stageIndex < MAX_STAGE_INDEX && stageIndex >= 0, "invalid stage index");
    int themeIndex = (stageIndex - stageIndex % 12) / 12;
    cocos2d::UserDefault::getInstance()->setIntegerForKey(MAX_STAGE_CLEAR_KEY_OF_THEME[themeIndex], stageIndex);
    cocos2d::UserDefault::getInstance()->flush();
}



//
//  NpcDataManager.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/4/15.
//
//

#include "NpcDataManager.h"
#include "NpcData.h"
#include "json/rapidjson.h"
#include "json/document.h"

// 可收集 npc 数据
#define NPC_DATA_FILE   "res/npc/npc.json"

/*
 *    获取单例对象
 */
NpcDataManager* NpcDataManager::getInstance()
{
    static NpcDataManager npcDataManager;
    return &npcDataManager;
}

/*
 *  constructor
 */
NpcDataManager::NpcDataManager() {}

/*
 *  destructor
 */
NpcDataManager::~NpcDataManager()
{
    for (auto const& it : m_npcDatas)
    {
        delete it.second;
    } /*for*/
    m_npcDatas.clear();
}

/*
 *  加载玩家收集的 npc 数据
 */
void NpcDataManager::loadNpcData()
{
    m_npcDatas.clear();
    m_vnpcDatas.clear();
    
    std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(NPC_DATA_FILE);
    rapidjson::Document doc;
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    RAPIDJSON_ASSERT(doc.IsArray());
    
    for (unsigned int i = 0; i < doc.Size(); i++)
    {
        const rapidjson::Value &npcDataValue = doc[i];
        std::string key = npcDataValue["key"].GetString();
        NpcData* pNpcData = new NpcData(key, npcDataValue["gafConfig"].GetString());
        if (!npcDataValue["name"].IsNull()) {
            pNpcData->setNpcName(npcDataValue["name"].GetString());
        }
        if (!npcDataValue["gafFile"].IsNull()) {
            pNpcData->setGafFile(npcDataValue["gafFile"].GetString());
        }
        pNpcData->setBossNpc(!npcDataValue["boss"].IsNull());
        
        pNpcData->setThemeIndex(npcDataValue["themeIndex"].GetInt());
        
        pNpcData->setNumber(npcDataValue["number"].GetInt());
        
        m_npcDatas.insert(std::make_pair(key, pNpcData));
        m_vnpcDatas.push_back(pNpcData);
    } /*for*/
    
//    clearAllCollected();
    updateCollectedStatus();
}

//
void NpcDataManager::clearAllCollected()
{
    for (auto const& it : m_npcDatas) {
        it.second->clearCollect();
    } /*for*/
}

//
void NpcDataManager::updateCollectedStatus()
{
    for (auto const& it : m_npcDatas)
    {
        it.second->updateCollectedTimes();
    } /*for*/
}

/* 根据key值获取 npcdata */
NpcData* NpcDataManager::getNpcDataByKey(const std::string& key)
{
    NpcData* pNpcData = nullptr;
    std::unordered_map<std::string, NpcData*>::iterator it = m_npcDatas.find(key);
    if (it != m_npcDatas.end()) {
        pNpcData = it->second;
    }
    return pNpcData;
}

/* 
 *  根据索引值获取 npcdata
 */
NpcData* NpcDataManager::getNpcDataByIndex(const unsigned int index)
{
    NpcData* pNpcData = nullptr;
    if (index < m_vnpcDatas.size())
    {
        pNpcData = m_vnpcDatas[index];
    }
    return pNpcData;
}

/* 获取 npc 数量 */
unsigned long NpcDataManager::getNumberOfThemeNpc(const unsigned long themeIndex)
{
    unsigned long npcNumber = 0;
    for (auto &iter : m_vnpcDatas) {
        unsigned int npcThemeIndex = iter->getThemeIndex();
        if (themeIndex == npcThemeIndex) {
            npcNumber++;
        }
    }
    return npcNumber;
}


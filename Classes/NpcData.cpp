//
//  NpcData.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/4/15
//
//

#include "NpcData.h"
#include "cocos2d.h"

/*
 *  constructor
 */
NpcData::NpcData(const std::string& key,
        const std::string& gafConfig)
{
    m_key = key;
    m_gafConfig = gafConfig;
}

/*
 * destructor
 */
NpcData::~NpcData() {}

/*
 *  NPC 被玩家收集
 */
void NpcData::collect()
{
    m_collectedTimes = cocos2d::UserDefault::getInstance()->getIntegerForKey(m_key.c_str(), 0);
    cocos2d::UserDefault::getInstance()->setIntegerForKey(m_key.c_str(), ++m_collectedTimes);
    cocos2d::UserDefault::getInstance()->flush();
}

//
void NpcData::clearCollect()
{
    cocos2d::UserDefault::getInstance()->setIntegerForKey(m_key.c_str(), 0);
    cocos2d::UserDefault::getInstance()->flush();
    m_collectedTimes = 0;
}

/*
 *  更新收集数量
 */
void NpcData::updateCollectedTimes()
{
    m_collectedTimes = cocos2d::UserDefault::getInstance()->getIntegerForKey(m_key.c_str(), 0);
}

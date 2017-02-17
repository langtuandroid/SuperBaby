//
//  NpcDataManager.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/4/15.
//
//

#ifndef __AbelLLExplorer__NpcDataManager__
#define __AbelLLExplorer__NpcDataManager__

#include <vector>
#include <unordered_map>

class NpcData;
class NpcDataManager
{
public:
    ~NpcDataManager();

private:
    NpcDataManager();
    NpcDataManager(const NpcDataManager&);
    NpcDataManager& operator=(const NpcDataManager&);

public:
    static NpcDataManager* getInstance();

public:
    void loadNpcData();       // 加载 Npc 数据
    
    //
    void clearAllCollected();
    /* 更新已收集标记 */
    void updateCollectedStatus();
    
    /* 根据key值获取 npcdata */
    NpcData* getNpcDataByKey(const std::string&);
    /* 根据索引值获取 npcdata */
    NpcData* getNpcDataByIndex(const unsigned int);
    
    /* 获取 npc 数量 */
    inline unsigned int getNumberOfNpc() {return (int)m_vnpcDatas.size();}
    
    /* 获取 某主题npc 数量 */
    unsigned long getNumberOfThemeNpc(const unsigned long);
    
private:
    std::unordered_map<std::string, NpcData*> m_npcDatas;
    std::vector<NpcData*> m_vnpcDatas;
};

#endif /* defined(__AbelLLExplorer__NpcDataManager__) */

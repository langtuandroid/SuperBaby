//
//  NpcData.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/4/15.
//
//

#ifndef __AbelLLExplorer__NpcData__
#define __AbelLLExplorer__NpcData__

#include "cocos2d.h"

USING_NS_CC;

class NpcActor;
class NpcData
{
public:
    NpcData(const std::string&, const std::string&);
    ~NpcData();

public:
    /* NPC 是否已经被收集 */
    inline bool isCollected() const {return m_collectedTimes > 0;}
    
    /* 获取 npc 收集次数 */
    inline unsigned int getCollectedTimes() const {return m_collectedTimes;}
    
    /* NPC 收集 */
    void collect();

    /* 清除 NPC 收集标记 */
    void clearCollect();
    
    /* 更新收集数量属性 */
    void updateCollectedTimes();
    
    /* 获取和设置是否 boss */
    inline bool isBossNpc() const {return m_isBossNpc;}
    inline void setBossNpc(const bool isBossNpc) {m_isBossNpc = isBossNpc;}
    
    // 获取 gaf config 文件
    inline std::string getGafConfigFile() const {return m_gafConfig;}

    // 获取和设置 gaf 文件
    inline std::string getGafFile() const {return m_gafFile;}
    inline void setGafFile(const std::string &gafFile) {m_gafFile = gafFile;}
    
    // 获取和设置 npc 名称
    inline std::string getNpcName() const {return m_name;}
    inline void setNpcName(const std::string& npcName)  {m_name = npcName;}
    
    /* 获取和设置音频文件 */
    inline std::string getAudioFile() const {return m_audioFile;}
    inline void setAudioFile(const std::string& audioFile ) { m_audioFile = audioFile;}

    /* 获取和设置音频播放时长 */
    inline unsigned int getAudioDuration() const {return m_audioDuration;}
    inline void setAudioDuration(const unsigned int & audioDuration ) { m_audioDuration = audioDuration;}
    
    /* 获取npc所属主题 */
    inline unsigned int getThemeIndex() const {return m_themeIndex;}
    inline void setThemeIndex(const unsigned int & themeIndex) {m_themeIndex = themeIndex;}
    
    /* 获取npc存放编号 */
    inline unsigned int getNumber() const {return m_number;}
    inline void setNumber(const unsigned int & number) {m_number = number;}
private:
    // key 值
    std::string m_key = "";
    // npc 名称
    std::string m_name = "npc";
    // gaf 配置文件
    std::string m_gafConfig = "";
    // gaf 文件
    std::string m_gafFile = "";
    
    // npc audio
    std::string m_audioFile = "";
    // 播放时长
    unsigned int m_audioDuration = 0;
    
    // 收集次数
    unsigned int m_collectedTimes = 0;
    
    // 是否boss npc
    bool m_isBossNpc = false;
    
    // 所属主题
    unsigned int m_themeIndex = 0;
    // npc 顺序编号
    unsigned int m_number = 0;
};

#endif /* defined(__AbelLLExplorer__NpcData__) */

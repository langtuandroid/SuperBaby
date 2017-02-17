//
//  Player.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/03.
//
//  玩家
//

#ifndef __AbelLLExplorer__Player__
#define __AbelLLExplorer__Player__

class LeadingActor;
class StageScene;
class Player
{
public:
    ~Player();

private:
    Player();
    Player(const Player&);
    Player& operator=(const Player&);
    
public:
    // 获取单例对象
    static Player* getInstance();

public:
    // 加载玩家数据
    void loadPlayerData();
    // 保存玩家数据
    void savePlayerData();
    
public:
    inline void setMaxStageIndex(const unsigned int maxStageIndex) {m_maxStageIndex = maxStageIndex;}
    inline unsigned int getMaxStageIndex() const {return m_maxStageIndex;}
    inline void setCurrStageIndex(const unsigned int currStageIndex) {m_currStageIndex = currStageIndex;}
    inline unsigned int getCurrStageIndex() const {return m_currStageIndex;}
    /* 获取当前关卡的当前回合索引 */
    unsigned int getCurrRoundIndex() const;
    
    /* 进入指定索引号的关卡 */
    void enterStage(const unsigned int, const bool isTrial = false);
    /* 切换到下一个关卡 */
    void nextStage();
    /* 获取关联的当前关卡场景对象 */
    StageScene* getAttachStageScene();
    /* 设置关联的当前关卡场景对象 */
    void attachStageScene(StageScene*);
    
    /* 获取艾米 */
    LeadingActor* getAimeeActor();
    /* 获取蓓蓓 */
    LeadingActor* getBebeiActor();
    
private:
    // 玩家已经玩通关的最大关卡索引号(退出程序后需要保存)
    unsigned int m_maxStageIndex = 0;
    // 玩家当前正在玩的关卡索引号
    unsigned int m_currStageIndex = 0;
    
    // 关联的当前关卡场景
    StageScene* m_pAttachStageScene = nullptr;
    
    LeadingActor* m_pAimeeActor = nullptr;        // 艾米
    LeadingActor* m_pBebeiActor = nullptr;        // 蓓蓓
};

#endif /* defined(__AbelLLExplorer__Player__) */

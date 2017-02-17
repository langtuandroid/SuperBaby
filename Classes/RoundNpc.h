//
//  RoundNpc.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//  收集 npc
//

#ifndef __AbelLLExplorer__RoundNpc__
#define __AbelLLExplorer__RoundNpc__

#include "RoundBase.h"

USING_NS_CC;

class NpcActor;
class StageScene;
class RoundNpc : public RoundBase
{
public:
    RoundNpc();
    virtual ~RoundNpc();
    
public:
    bool initWithNpc(const int, const std::string&, const Vec2&, StageScene*, const Vec2& nodePos);
    static RoundNpc* createWithNpc(const int, const std::string&, const Vec2&, StageScene*, const Vec2& nodePos);
    void addBlowNode(Node*, const Vec2&);

public:
    virtual void onEnterRound();
    /*  处理npc收集 */
    virtual void processNpcCollect();
    /*  回合结束回调 */
    virtual void endRound();
    
private:
    std::string m_npcKey;                   // npc key值
    std::string m_npcGafFile;               // npc gaf 文件
    std::string m_npcGafConfig;             // npc gaf config 文件
    
    Vec2 m_npcPos;                          // npc 位置

    NpcActor* m_pNpcActor = nullptr;     // npc

    int m_effectId = 0;                 // 音效play的id
};
#endif /* defined(__AbelLLExplorer__RoundNpc__) */

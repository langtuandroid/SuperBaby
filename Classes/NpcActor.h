//
//  NpcActor.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/12.
//
//  Npc Actor ，可以被收集的 Npc Actor
//

#ifndef __AbelLLExplorer__NpcActor__
#define __AbelLLExplorer__NpcActor__

#include "Actor.h"

class NpcActor : public Actor
{
public:
    NpcActor();
    virtual ~NpcActor();
    
public:
    /* 根据 gaf 配置文件生成 */
    static NpcActor* createWithGafConfigFile(const std::string&, const bool showArrow = false);

public:
    /* 是否可收集 NPC */
    virtual bool isNpcActor() const {return true;}
    
public:
    void sleep();
    void standby(const bool looped = true);
    void trigger(const bool looped = true);
    /* 控制显示箭头 */
    void showNpcArrow(const bool);
    /* 控制显示底座 */
    void showNpcBase(const bool);
    
    void setBaseVerticalOffset(const float);

private:
    void setBasePosition();
    
private:
    // 指示箭头
    Sprite* m_pNpcArrow = nullptr;
    // 底座
    Actor* m_pBaseActor = nullptr;
    
    // 底座垂直偏移缺省值
    float m_baseVerticalOffset;
    
    Vec2 m_originalPosition;
    Vec2 m_originalBasePosition;
};

#endif /* defined(__AbelLLExplorer__NpcActor__) */

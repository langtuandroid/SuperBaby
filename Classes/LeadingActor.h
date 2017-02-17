//
//  LeadingActor.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/12.
//
//  主角类
//

#ifndef __AbelLLExplorer__LeadingActor__
#define __AbelLLExplorer__LeadingActor__

#include "Actor.h"

// 角色状态
enum class ActorState
{
    UNKNOWN,
    WAIT,
    WALK,
    HAPPY,
    UNHAPPY,
    JUMP
};

class LeadingActor : public Actor
{
public:
    LeadingActor();
    virtual ~LeadingActor();
    
public:
    /* 根据 gaf 配置文件生成 */
    static LeadingActor* createWithGafConfigFile(const std::string&);
    /* 根据其它 Leadingactor 生成 */
    static LeadingActor* createWithLeadingActor(LeadingActor*);

public:
    /* 是否主角 */
    virtual bool isLeadingActor() const {return true;}

public:
    void wait();
    void happy(const unsigned int happyTimes = 1);
    void unhappy();
    void walk(const bool looped = true);
    void jump(const std::function<void()>&);
    
    inline bool isWait() const {return m_state == ActorState::WAIT;}
    
private:
    ActorState m_state;
    
    // 庆祝开心的次数
    unsigned int m_happyTimes;
};

#endif /* defined(__AbelLLExplorer__LeadingActor__) */

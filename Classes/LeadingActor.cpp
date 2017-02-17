//
//  LeadingActor.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/12.
//
//  主角类
//

#include "LeadingActor.h"
#include "GAFPlayer/GAF.h"

using namespace gaf;

/*
 *  constuctor
 */
LeadingActor::LeadingActor():
m_state(ActorState::UNKNOWN)
{}

/*
 *  destuctor
 */
LeadingActor::~LeadingActor() {}

/*
 * 根据 gaf 配置文件生成角色对象
 *
 * 参数：
 *      gafConfigFile   gaf 配置文件
 */
LeadingActor* LeadingActor::createWithGafConfigFile(const std::string& gafConfigFile)
{
    LeadingActor* pRet = new LeadingActor();
    if (pRet && pRet->initWithGafConfigFile(gafConfigFile)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

/*
 *  根据其它 LeadingActor 对象生成
 */
LeadingActor* LeadingActor::createWithLeadingActor(LeadingActor* pOtherLeadingActor)
{
    CCASSERT(pOtherLeadingActor, "invalid gafAsset Object");
    LeadingActor* pRet = new LeadingActor();
    if (pRet && pRet->initWithActor(pOtherLeadingActor)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

/* 播放 “等待” 动画 */
void LeadingActor::wait()
{
    m_state = ActorState::WAIT;
    playWithSequenceAliasName("wait", true);
}

/* 播放 “快乐” 动画 */
void LeadingActor::happy(const unsigned int happyTimes)
{
    m_happyTimes = happyTimes;
    m_state = ActorState::HAPPY;
    
    playWithSequenceAliasName("happy", false);
    m_pGafObj->setAnimationFinishedPlayDelegate([this](GAFObject* obj)
    {
        if (--m_happyTimes == 0) {
            wait();
        } else {
            playWithSequenceAliasName("happy", false);
        }
    });
}

/* 播放 "不开心" 动画 */
void LeadingActor::unhappy()
{
    m_state = ActorState::UNHAPPY;
    playWithSequenceAliasName("unhappy", false);
    m_pGafObj->setAnimationFinishedPlayDelegate([this](GAFObject* obj)
    {
        wait();
    });
}

/* 播放 "行走" 动画 */
void LeadingActor::walk(const bool looped)
{
    m_state = ActorState::WALK;
    playWithSequenceAliasName("walk", looped);
}

/* 播放 "跳跃" 动画 */
void LeadingActor::jump(const std::function<void()>& jumpCompletedCB)
{
    m_state = ActorState::JUMP;
    playWithSequenceAliasName("jump", false);
    m_pGafObj->setAnimationFinishedPlayDelegate([this, jumpCompletedCB](GAFObject* obj)
    {
        if (jumpCompletedCB != nullptr) {
            jumpCompletedCB();
        }
    });
}

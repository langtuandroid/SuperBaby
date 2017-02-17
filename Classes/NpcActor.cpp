//
//  NpcActor.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/04/15.
//
//
//

#include "NpcActor.h"

// 箭头资源文件
#define NPC_ARROW_FILE          "res/common/npcArrow.png"

// 底座 gaf 文件
#define BASE_GAF_FILE           "res/npc/NPC-pingtai/NPC-pingtai.gaf"
// 底座垂直偏移缺省值
#define BASE_VERTICAL_OFFSET    -88.0f

/*
 *  constuctor
 */
NpcActor::NpcActor():m_baseVerticalOffset(BASE_VERTICAL_OFFSET)
{}

/*
 *  destuctor
 */
NpcActor::~NpcActor() {}

/*
 * 根据 gaf 配置文件生成角色对象
 *
 * 参数：
 *      gafConfigFile   gaf 配置文件
 */
NpcActor* NpcActor::createWithGafConfigFile(const std::string& gafConfigFile, const bool showArrow)
{
    NpcActor* pNpcActor = new NpcActor();
    if (pNpcActor && pNpcActor->initWithGafConfigFile(gafConfigFile))
    {
        CCASSERT(pNpcActor->getSize().width > 0.0f && pNpcActor->getSize().height > 0.0f, "size must > 0");
        pNpcActor->autorelease();
        pNpcActor->showNpcArrow(showArrow);
        return pNpcActor;
    } else {
        CC_SAFE_DELETE(pNpcActor);
        return nullptr;
    }
}

/* 睡眠 */
void NpcActor::sleep()
{
    playWithSequenceAliasName("sleep", false);
}

/* 播放 “待机” 动画 */
void NpcActor::standby(const bool looped) {
    playWithSequenceAliasName("standby", looped);
}

/* 播放 “触发” 动画 */
void NpcActor::trigger(const bool looped)
{
    playWithSequenceAliasName("trigger", looped);
}

/* 控制显示箭头 */
void NpcActor::showNpcArrow(const bool isShowArrow)
{
    if (isShowArrow)
    {
        if (m_pNpcArrow == nullptr)
        {
            m_pNpcArrow = Sprite::create(NPC_ARROW_FILE);
            m_pNpcArrow->setAnchorPoint(Vec2(0.5f, 0.0f));
            m_pNpcArrow->setPosition(Vec2(m_Size.width / 2.0f, m_Size.height+100));
            addChild(m_pNpcArrow, getGafObjZorder() + 1);
            auto action = RepeatForever::create(Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, -20.0f)),
                                                                 MoveBy::create(0.5f, Vec2(0.0f, 20.0f)),
                                                                 MoveBy::create(0.5f, Vec2(0.0f, 20.0f)),
                                                                 MoveBy::create(0.5f, Vec2(0.0f, -20.0f))
                                                                 , NULL));
            m_pNpcArrow->runAction(action);
        }
    }
    else
    {
        if (m_pNpcArrow != nullptr)
        {
            m_pNpcArrow->removeFromParent();
            m_pNpcArrow = nullptr;
        }
    }
}

//
void NpcActor::setBasePosition()
{
    if (m_pBaseActor != nullptr)
    {
        auto currPosition = getPosition();
        m_pBaseActor->setPosition(Vec2(currPosition.x + m_Size.width / 2.0f,
                                       currPosition.y + m_baseVerticalOffset));
    }
}

/* 控制显示底座 */
void NpcActor::showNpcBase(const bool showBase)
{
    if (showBase)
    {
        if (m_pBaseActor == nullptr && getParent())
        {
            m_pBaseActor = Actor::createWithGafFile(BASE_GAF_FILE);
            m_pBaseActor->setPosAtNode(Vec2(-140.0f, 820.0f));
            m_pBaseActor->setSize(Size(540.0f, 540.0f));
            m_pBaseActor->setAnchorPoint(Vec2(0.5f, 0.0f));
            getParent()->addChild(m_pBaseActor, getLocalZOrder() - 1);
        }
        setBasePosition();
    } else {
        if (m_pBaseActor != nullptr)
        {
            m_pBaseActor->removeFromParent();
            m_pBaseActor = nullptr;
        }
    }
}

//
void NpcActor::setBaseVerticalOffset(const float bvo)
{
    m_baseVerticalOffset = bvo;
    showNpcBase(true);
}

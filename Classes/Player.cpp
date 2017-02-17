//
//  Player.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/3.
//
//

#include "Player.h"
#include "cocos2d.h"
#include "LeadingActor.h"
#include "Stage001Scene.h"
#include "Stage002Scene.h"
#include "Stage003Scene.h"
#include "Stage004Scene.h"
#include "Stage005Scene.h"
#include "Stage006Scene.h"
#include "Stage007Scene.h"
#include "Stage008Scene.h"
#include "Stage009Scene.h"
#include "Stage010Scene.h"
#include "Stage011Scene.h"
#include "Stage012Scene.h"
#include "Stage013Scene.h"
#include "Stage014Scene.h"
#include "Stage015Scene.h"
#include "Stage016Scene.h"
#include "Stage017Scene.h"
#include "Stage018Scene.h"
#include "Stage019Scene.h"
#include "Stage020Scene.h"
#include "Stage021Scene.h"
#include "Stage022Scene.h"
#include "Stage023Scene.h"
#include "Stage024Scene.h"
#include "Stage025Scene.h"
#include "Stage026Scene.h"
#include "Stage027Scene.h"
#include "Stage028Scene.h"
#include "Stage029Scene.h"
#include "Stage030Scene.h"
#include "Stage031Scene.h"
#include "Stage032Scene.h"
#include "Stage033Scene.h"
#include "Stage034Scene.h"
#include "Stage035Scene.h"
#include "Stage036Scene.h"
#include "Stage037Scene.h"
#include "Stage038Scene.h"
#include "Stage039Scene.h"
#include "Stage040Scene.h"
#include "Stage041Scene.h"
#include "Stage042Scene.h"
#include "Stage043Scene.h"
#include "Stage044Scene.h"
#include "Stage045Scene.h"
#include "Stage046Scene.h"
#include "Stage047Scene.h"
#include "Stage048Scene.h"
#include "StageDataManager.h"
#include "MobClickCpp.h"

USING_NS_CC;

#define MAX_STAGE_INDEX_KEY     "MAX_STAGE_INDEX"           // 存放玩家最大玩通关关卡索引号的保存 key 值
#define AIMEE_JSON_FILE         "config/Aimee.json"
#define BEBEI_JSON_FILE         "config/Bebei.json"

/*
 *  获取玩家单例对象
 */
Player* Player::getInstance()
{
    static Player player;
    return &player;
}

/*
 *  加载玩家数据
 */
void Player::loadPlayerData()
{
    m_maxStageIndex = UserDefault::getInstance()->getIntegerForKey(MAX_STAGE_INDEX_KEY, 0);
    m_currStageIndex = 0;
}

/*
 *  保存玩家数据
 */
void Player::savePlayerData()
{
    UserDefault::getInstance()->setIntegerForKey(MAX_STAGE_INDEX_KEY, m_maxStageIndex);
    UserDefault::getInstance()->flush();
}

/*
 *  constructor
 */
Player::Player() {}

/*
 *  destructor
 */
Player::~Player()
{
    /* 释放艾米和蓓蓓 */
    CC_SAFE_RELEASE_NULL(m_pAimeeActor);
    CC_SAFE_RELEASE_NULL(m_pBebeiActor);
}

/*
 *  进入指定索引号的关卡
 */
void Player::enterStage(const unsigned int stageIndex, const bool isTrial)
{
    auto pStageDataManager = StageDataManager::getInstance();
    //    CCASSERT(stageIndex < pStageDataManager->getNumberOfStages(), "invalid stageIndex");
    
    // 更新关卡索引号
    m_currStageIndex = stageIndex;
    
    StageScene* pStageScene = nullptr;
    auto pStageData = pStageDataManager->getStageData(m_currStageIndex);
    if (pStageData == nullptr) {
        std::string stageConfigFile = pStageDataManager->getStageConfigFile(stageIndex);
        pStageData = pStageDataManager->loadStageDataWithJson(stageConfigFile);
    }
    switch (stageIndex)
    {
        case 0:
            pStageScene = Stage001Scene::createWithStageData(pStageData);
            break;
        case 1:
            pStageScene = Stage002Scene::createWithStageData(pStageData);
            break;
        case 2:
            pStageScene = Stage003Scene::createWithStageData(pStageData);
            break;
        case 3:
            pStageScene = Stage004Scene::createWithStageData(pStageData);
            break;
        case 4:
            pStageScene = Stage005Scene::createWithStageData(pStageData);
            break;
        case 5:
            pStageScene = Stage006Scene::createWithStageData(pStageData);
            break;
        case 6:
            pStageScene = Stage007Scene::createWithStageData(pStageData);
            break;
        case 7:
            pStageScene = Stage008Scene::createWithStageData(pStageData);
            break;
        case 8:
            pStageScene = Stage009Scene::createWithStageData(pStageData);
            break;
        case 9:
            pStageScene = Stage010Scene::createWithStageData(pStageData);
            break;
        case 10:
            pStageScene = Stage011Scene::createWithStageData(pStageData);
            break;
        case 11:
            pStageScene = Stage012Scene::createWithStageData(pStageData);
            break;
        case 12:
            pStageScene = Stage013Scene::createWithStageData(pStageData);
            break;
        case 13:
            pStageScene = Stage014Scene::createWithStageData(pStageData);
            break;
        case 14:
            pStageScene = Stage015Scene::createWithStageData(pStageData);
            break;
        case 15:
            pStageScene = Stage016Scene::createWithStageData(pStageData);
            break;
        case 16:
            pStageScene = Stage017Scene::createWithStageData(pStageData);
            break;
        case 17:
            pStageScene = Stage018Scene::createWithStageData(pStageData);
            break;
        case 18:
            pStageScene = Stage019Scene::createWithStageData(pStageData);
            break;
        case 19:
            pStageScene = Stage020Scene::createWithStageData(pStageData);
            break;
        case 20:
            pStageScene = Stage021Scene::createWithStageData(pStageData);
            break;
        case 21:
            pStageScene = Stage022Scene::createWithStageData(pStageData);
            break;
        case 22:
            pStageScene = Stage023Scene::createWithStageData(pStageData);
            break;
        case 23:
            pStageScene = Stage024Scene::createWithStageData(pStageData);
            break;
        case 24:
            pStageScene = Stage025Scene::createWithStageData(pStageData);
            break;
        case 25:
            pStageScene = Stage026Scene::createWithStageData(pStageData);
            break;
        case 26:
            pStageScene = Stage027Scene::createWithStageData(pStageData);
            break;
        case 27:
            pStageScene = Stage028Scene::createWithStageData(pStageData);
            break;
        case 28:
            pStageScene = Stage029Scene::createWithStageData(pStageData);
            break;
        case 29:
            pStageScene = Stage030Scene::createWithStageData(pStageData);
            break;
        case 30:
            pStageScene = Stage031Scene::createWithStageData(pStageData);
            break;
        case 31:
            pStageScene = Stage032Scene::createWithStageData(pStageData);
            break;
        case 32:
            pStageScene = Stage033Scene::createWithStageData(pStageData);
            break;
        case 33:
            pStageScene = Stage034Scene::createWithStageData(pStageData);
            break;
        case 34:
            pStageScene = Stage035Scene::createWithStageData(pStageData);
            break;
        case 35:
            pStageScene = Stage036Scene::createWithStageData(pStageData);
            break;
        case 36:
            pStageScene = Stage037Scene::createWithStageData(pStageData);
            break;
        case 37:
            pStageScene = Stage038Scene::createWithStageData(pStageData);
            break;
        case 38:
            pStageScene = Stage039Scene::createWithStageData(pStageData);
            break;
        case 39:
            pStageScene = Stage040Scene::createWithStageData(pStageData);
            break;
        case 40:
            pStageScene = Stage041Scene::createWithStageData(pStageData);
            break;
        case 41:
            pStageScene = Stage042Scene::createWithStageData(pStageData);
            break;
        case 42:
            pStageScene = Stage043Scene::createWithStageData(pStageData);
            break;
        case 43:
            pStageScene = Stage044Scene::createWithStageData(pStageData);
            break;
        case 44:
            pStageScene = Stage045Scene::createWithStageData(pStageData);
            break;
        case 45:
            pStageScene = Stage046Scene::createWithStageData(pStageData);
            break;
        case 46:
            pStageScene = Stage047Scene::createWithStageData(pStageData);
            break;
        case 47:
            pStageScene = Stage048Scene::createWithStageData(pStageData);
            break;
        default:
            break;
    }
    
    // 是否试玩
    pStageScene->setTrial(isTrial);
    // 加载主角
    pStageScene->loadLeadingActor();
    // 开始第一个回合
    pStageScene->startRound();
    // 将当前关卡场景对象关联到玩家对象
    attachStageScene(pStageScene);
    
    // 友盟统计
    char levelBuffer[16];
    sprintf(levelBuffer, "level-%02d", stageIndex + 1);
    umeng::MobClickCpp::startLevel(levelBuffer);
    
    // 进入关卡场景
    Director::getInstance()->replaceScene(pStageScene);
}

/*
 *  切换到下一个关卡
 */
void Player::nextStage()
{
    if (m_currStageIndex + 1 < StageDataManager::getInstance()->getNumberOfStages()) {
        enterStage(m_currStageIndex + 1);
    } else /* 已经没有下一个关卡，所有关卡都完成 */ {
    }
}

/*
 * 获取关联的当前关卡场景对象
 */
StageScene* Player::getAttachStageScene()
{
    return m_pAttachStageScene;
}

/*
 * 关联当前关卡场景对象
 */
void Player::attachStageScene(StageScene* pAttachStageScene)
{
    m_pAttachStageScene = pAttachStageScene;
}

/*
 *  获取艾米
 */
LeadingActor* Player::getAimeeActor()
{
    if (nullptr == m_pAimeeActor) {
        m_pAimeeActor = LeadingActor::createWithGafConfigFile(AIMEE_JSON_FILE);
        m_pAimeeActor->retain();
    }
    return LeadingActor::createWithLeadingActor(m_pAimeeActor);
}

/*
 *  获取蓓蓓
 */
LeadingActor* Player::getBebeiActor()
{
    if (nullptr == m_pBebeiActor) {
        m_pBebeiActor = LeadingActor::createWithGafConfigFile(BEBEI_JSON_FILE);
        m_pBebeiActor->retain();
    }
    return LeadingActor::createWithLeadingActor(m_pBebeiActor);
}

//
//  RoundBase.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/9.
//
//

#include "RoundBase.h"
#include "StageScene.h"
#include "StageData.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "SelStageScene.h"
#include "GafAssetCache.h"
#include "AppConfig.h"

#define EFFECT_TRUE_FILE                    "res/common/audio/diandui01.mp3"                // 答对音效
#define EFFECT_ERROR_FILE                   "res/common/audio/dacuo01.mp3"                  // 答错音效

/*
 *  constructor
 */
RoundBase::RoundBase()
{
    m_asyncTextures.clear();
    scheduleUpdate();
}

/*
 *  destructor
 */
RoundBase::~RoundBase()
{
    cleanUp();
}

/*
 *  预加载资源完成
 */
void RoundBase::onPreLoadedRes()
{
    m_isAsyncResLoaded = true;
}

/*
 * 预加载资源
 * 返回 true 表示有预加载资源
 * 返回 false 表示没有预加载资源
 */
void RoundBase::preLoadRes()
{
    if (m_asyncTextures.size() > 0) /*存在需要异步加载的纹理*/ {
        auto pTextureCache = Director::getInstance()->getTextureCache();
        auto textureLoadedComplete = [this](Texture2D*) {
            if (++m_loadedTextures == m_asyncTextures.size()) {
                preLoadGaf();
            }
        };
        for (auto &it : m_asyncTextures) {
            pTextureCache->addImageAsync(it, textureLoadedComplete);
        } /*for*/
    } else {
        preLoadGaf();
    }
}

/* 
 *  预加载gaf（异步）
 */
void RoundBase::preLoadGaf()
{
    if (m_asyncGafs.size() > 0) {
        auto gafLoadedComplete = [this](GAFAsset*) {
            if (++m_loadedGafs == m_asyncGafs.size()) {
                onPreLoadedRes();
            }
        };
        for (auto &it : m_asyncGafs) {
            GafAssetCache::getInstance()->addGafAssetAsync(it, gafLoadedComplete);
        } /*for*/
    } else {
        onPreLoadedRes();
    }
}

//
void RoundBase::update(float delta)
{
    if (m_asyncTextures.size() > 0 || m_asyncGafs.size() > 0) /*有需要异步加载的纹理资源*/ {
        if (m_isAsyncResLoaded && m_isEnterRound )
        {
            unscheduleUpdate();
            // 释放看不见的纹理
            m_pStageScene->releaseBgByCurrPos();
            // 进入回合
            onEnterRound();
        }
    } else {
        if (m_isEnterRound )
        {
            unscheduleUpdate();
            // 释放看不见的纹理
            m_pStageScene->releaseBgByCurrPos();
            // 进入回合
            onEnterRound();
        }
    }
}

/*
 *  添加本回合退出后要清理的纹理
 */
void RoundBase::appendRemovedTexture(std::string textureFile) {
    m_removedTextures.push_back(textureFile);
}

/*
 *  清理
 */
void RoundBase::cleanUp()
{
    m_anwerErrorCB = nullptr;
    m_completeCB = nullptr;
    m_pStageScene = nullptr;
    m_celebrateCB = nullptr;
    
    //清除纹理
    auto textureCache = Director::getInstance()->getTextureCache();
    for (auto &it : m_removedTextures)
    {
        textureCache->removeTextureForKey(it);
    } /*for*/
    m_removedTextures.clear();
    
    //清除gaf
    for (auto &it : m_asyncGafs)
    {
        GafAssetCache::getInstance()->removeGafAssetForKey(it);
    }/*for*/
    m_asyncGafs.clear();
}

// 播放答对音效
void RoundBase::playAnswerCorrectEffect()
{
    MyAudioEngine::getInstance()->playAudio(EFFECT_TRUE_FILE);
}

// 播放答错音效
void RoundBase::playAnswerErrorEffect()
{
    MyAudioEngine::getInstance()->playAudio(EFFECT_ERROR_FILE);
}

//答对时主角说话音效
void RoundBase::playLeadAnsCorEff()
{
    std::string effStr = AppConfig::getInstance()->getLeadAnsCorEff();
    m_effIndex = MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}
//答错时主角说话音效
void RoundBase::playLeadAnsErrEff()
{
    std::string effStr = AppConfig::getInstance()->getLeadAnsErrEff();
    m_effIndex = MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}

//主角行走说话音效
void RoundBase::playLeadWalkEff()
{
    std::string effStr = AppConfig::getInstance()->getLeadWalkEff();
    m_effIndex = MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}

//npc收集主角说话音效
void RoundBase::playNpcCollectEff()
{
    std::string effStr = AppConfig::getInstance()->getLeadNpcCollectEff();
    m_effIndex = MyAudioEngine::getInstance()->playAudio(effStr.c_str());
}

//
//  LoadingScene.cpp
//  superBaby
//
//  Created by Administrator on 15/7/10.
//
//

#include "LoadingScene.h"
#include "Player.h"
#include "Actor.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "ThemeSelScene.h"
#include "WelcomeScene.h"

USING_NS_CC;

#define FONT_FILE           "fonts/DFGBHBC_0.TTC"
#define LOADING_GAF_FILE    "res/common/loding02/loding02.gaf"
#define BG_LEFT_IMAGE       "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE      "res/common/bgRight.png"

// 主题选择界面
#define THEME1_IMG          "res/theme/ZTXZ-1-SL.png"       // 主题1:魔法森林
#define THEME2_IMG          "res/theme/ZTXZ-2-XQ.png"       // 主题2:神秘星球
#define THEME3_IMG          "res/theme/ZTXZ-3-HY.png"       // 主题3:海洋
#define THEME4_IMG          "res/theme/ZTXZ-4-GC.png"       // 主题4:文明古国
#define THEME_IMG             "res/theme/zhitixuanze01.png"
#define ZTXZ_1_GAF_FILE     "res/theme/ZTXZ-1/ZTXZ-1.gaf"   //主题一题目gaf
#define ZTXZ_2_GAF_FILE     "res/theme/ZTXZ-2/ZTXZ-2.gaf"   //主题二题目gaf
#define ZTXZ_3_GAF_FILE     "res/theme/ZTXZ-3/ZTXZ-3.gaf"   //主题三题目gaf
#define ZTXZ_4_GAF_FILE     "res/theme/ZTXZ-4/ZTXZ-4.gaf"   //主题四题目gaf

// welcome 界面
#define BN_WELCOME_FILE             "res/common/shouyeIT.png"               //WELCOME的png
#define BG_PNG_FILE                 "res/common/shouye-BG.png"                      // 背景图
#define TITLE_GAF_FILE              "res/common/shouyebiaoti-C+D/shouyebiaoti-C+D.gaf"
#define BEIBEI_GAF_FILE             "res/common/shouye-Bebei/shouye-Bebei.gaf"      // 蓓蓓gaf文件
#define AIMI_GAF_FILE               "res/common/shouye-Aimee/shouye-Aimee.gaf"      // 艾米gaf文件
#define TOUYING_GAF_FILE            "res/common/touying-C+D/touying-C+D.gaf"        // 投影gaf文件

enum SCENEINDEX
{
    WELCOME = 1,
    THEMESEL
};

/*
 *  constuctor
 */
LoadingScene::LoadingScene()
{
}

/*
 *  destructor
 */
LoadingScene::~LoadingScene()
{
    GafAssetCache::getInstance()->removeGafAssetForKey(LOADING_GAF_FILE);
    m_imgs.clear();
    m_gafs.clear();
}

//
Scene* LoadingScene::createWithIndex(const unsigned int index)
{
    auto pRet = new LoadingScene();
    if (pRet && pRet->initWithIndex(index)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

//
bool LoadingScene::initWithIndex(const unsigned int index)
{
    m_index = index;
    
    auto size = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    auto sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
    LayerColor* layerColor = LayerColor::create(Color4B(19, 165, 212, 255), dw, size.height);
    layerColor->setPosition(Vec2(sceneOffset, 0));
    addChild(layerColor);
    
    Label* label = Label::createWithTTF("加载中，请稍等。。。", FONT_FILE, 48);
    label->setPosition(Vec2(dw / 2.0f+sceneOffset, size.height / 2.0f));
    this->addChild(label);
    
    Actor* loadingActor = Actor::createWithGafFile(LOADING_GAF_FILE);
    loadingActor->setPosition(Vec2(dw / 2.0f - 580.0f+sceneOffset, size.height / 2.0f + 220.0f));
    this->addChild(loadingActor);
    return true;
}

void LoadingScene::onEnter()
{
    Scene::onEnter();
    
    if (m_index == SCENEINDEX::WELCOME) {
        
        m_imgs.push_back(BG_PNG_FILE);
        m_imgs.push_back(BG_LEFT_IMAGE);
        m_imgs.push_back(BG_RIGHT_IMAGE);
        m_imgs.push_back(BN_WELCOME_FILE);
        
        m_gafs.push_back(AIMI_GAF_FILE);
        m_gafs.push_back(BEIBEI_GAF_FILE);
        m_gafs.push_back(TOUYING_GAF_FILE);
        m_gafs.push_back(TITLE_GAF_FILE);
        
    } else if (m_index == SCENEINDEX::THEMESEL)
    {
        m_imgs.push_back(BG_LEFT_IMAGE);
        m_imgs.push_back(BG_RIGHT_IMAGE);
        m_imgs.push_back(THEME1_IMG);
        m_imgs.push_back(THEME2_IMG);
        m_imgs.push_back(THEME3_IMG);
        m_imgs.push_back(THEME4_IMG);
        m_imgs.push_back(THEME_IMG);
        
        m_gafs.push_back(ZTXZ_1_GAF_FILE);
        m_gafs.push_back(ZTXZ_2_GAF_FILE);
        m_gafs.push_back(ZTXZ_3_GAF_FILE);
        m_gafs.push_back(ZTXZ_4_GAF_FILE);
    }
    
    m_preloadImgs = m_imgs.size();m_loadedImgs = 0;
    m_preloadGafs = m_gafs.size();m_loadedGafs = 0;
    scheduleUpdate();
    
}

void LoadingScene::update(float)
{
    if (m_imgs.size() > 0) {
        auto textureLoadedCB = [this](Texture2D* pTexture2D) {
            if (++m_loadedImgs == m_preloadImgs) {
                m_isLoadedCompleteImg = true;
            }
        };
        
        if (m_willLoadedImg < m_preloadImgs) {
            Director::getInstance()->getTextureCache()->addImageAsync(m_imgs[m_willLoadedImg++], textureLoadedCB);
        }
        
        if (m_isLoadedCompleteImg && !m_isLoadedCompleteGaf) {
            auto gafLoadedCB = [this](gaf::GAFAsset*  pGAFAsset)
            {
                ++m_loadedGafs;
            };
            if (m_willLoadedGaf < m_preloadGafs) {
                GafAssetCache::getInstance()->addGafAssetAsync(m_gafs[m_willLoadedGaf++], gafLoadedCB);
            } else if (m_loadedGafs == m_preloadGafs)
            {
                m_isLoadedCompleteGaf = true;
            }
        }
        if (m_isLoadedCompleteImg && m_isLoadedCompleteGaf) {
            unscheduleUpdate();
            enterStage();
        }
    } else {
        auto gafLoadedCB = [this](gaf::GAFAsset*  pGAFAsset) {
            ++m_loadedGafs;
        };
        if (m_willLoadedGaf < m_preloadGafs) {
            GafAssetCache::getInstance()->addGafAssetAsync(m_gafs[m_willLoadedGaf++], gafLoadedCB);
        }else if (m_loadedGafs == m_preloadGafs)
        {
            m_isLoadedCompleteGaf = true;
        }
        
        if (m_isLoadedCompleteGaf) {
            unscheduleUpdate();
            enterStage();
        }
    }
}

void LoadingScene::enterStage()
{
    if (m_index == SCENEINDEX::WELCOME)
    {
        Director::getInstance()->replaceScene(WelcomeScene::createWithFirst(false));
    } else if (m_index == SCENEINDEX::THEMESEL)
    {
        Director::getInstance()->replaceScene(ThemeSelScene::create());
    }
    
}
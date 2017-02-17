//
//  WelcomeScene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/03/19.
//
//

#include "WelcomeScene.h"

#include "LoadingScene.h"
#include "NpcBagNode.h"
#include "PreLoadUIManager.h"
#include "GafAssetCache.h"
#include "AppConfig.h"
#include "Actor.h"
#include "GAFPlayer/GAF.h"
#include "LogoScene.h"
#include "CustomMessage.h"
#include "commonFunc.h"
#include "MyAudioEngine.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidRollBackListener.h"
#endif

#define BG_PNG_FILE                 "res/common/shouye-BG.png"                      // 背景图
#define TITLE_GAF_CONFIG_FILE       "config/title.json"                             // 标题gaf配置文件
#define TITLE_GAF_FILE              "res/common/shouyebiaoti-C+D/shouyebiaoti-C+D.gaf"
#define BEIBEI_GAF_FILE             "res/common/shouye-Bebei/shouye-Bebei.gaf"      // 蓓蓓gaf文件
#define AIMI_GAF_FILE               "res/common/shouye-Aimee/shouye-Aimee.gaf"      // 艾米gaf文件
#define TOUYING_GAF_CONFIG_FILE     "config/touying.json"                           // 投影gaf配置文件
#define TOUYING_GAF_FILE            "res/common/touying-C+D/touying-C+D.gaf"        // 投影gaf文件

#define BN_PLIST_FILE               "res/common/shouyeIT.plist"             //BN的plist
#define BN_PNG_FILE                 "res/common/shouyeIT.png"               //BN的png
#define ENTER_BTN_POSY              720.0f                                  //进入按钮y轴坐标
#define BTN_BACK_ZORDER             8                                       //播放按钮底图z值
#define BTN_SCALE_TIME              1.0f                                    //按钮放大时间
#define ROTATE_TIME                 1.0f                                    //旋转时间

#define STARTUP_AUDIO               "res/audio/welcome/shouyeqidong.mp3"
#define STARTUP_AUDIO_DURATION      6.0f
#define BUTTON_AUDIO                "res/audio/welcome/shouyeanniu.mp3"

#define AIMI_INTRO_AUDIO_FILE        "res/common/audio/PB-AM-YX.mp3"     //艾米介绍音效
#define BEIBEI_INTRO_AUDIO_FILE      "res/common/audio/PB-BB-YX.mp3"     //蓓蓓介绍音效

#define BAG_GAF_CONFIG_FILE         "config/bag.json"                       //背包gaf配置
#define BACKGROUND_MUSIC_FILE       "res/audio/welcome/shouye-BGM.mp3"

/*
 *  constructor
 */
WelcomeScene::WelcomeScene()
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(BN_PLIST_FILE);
    
    m_sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
    m_bagPos = Vec2(1898.0f, 132.0f);
    m_playBtnPos = Vec2(166.0f, 158.0f);
}

/*
 *  desstructor
 */
WelcomeScene::~WelcomeScene()
{
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->removeSpriteFramesFromFile(BN_PLIST_FILE);
    auto pTextureCache = Director::getInstance()->getTextureCache();
    pTextureCache->removeTextureForKey(BG_PNG_FILE);
    
    auto pGafAssetCache = GafAssetCache::getInstance();
    pGafAssetCache->removeGafAssetForKey(TITLE_GAF_FILE);
    pGafAssetCache->removeGafAssetForKey(BEIBEI_GAF_FILE);
    pGafAssetCache->removeGafAssetForKey(AIMI_GAF_FILE);
    pGafAssetCache->removeGafAssetForKey(TOUYING_GAF_FILE);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners(NPCBAG_END_CALLBACK);
    
    MyAudioEngine::getInstance()->unloadAudio(AIMI_INTRO_AUDIO_FILE);
    MyAudioEngine::getInstance()->unloadAudio(BEIBEI_INTRO_AUDIO_FILE);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (m_androidRollBackListener != nullptr) {
        delete m_androidRollBackListener;
        m_androidRollBackListener = nullptr;
    }
#endif
}

//
WelcomeScene* WelcomeScene::createWithFirst(const bool first)
{
    WelcomeScene* pWelcomeScene = new WelcomeScene();
    if (pWelcomeScene && pWelcomeScene->initWithFirst(first))
    {
        pWelcomeScene->autorelease();
        return pWelcomeScene;
    } else {
        CC_SAFE_DELETE(pWelcomeScene);
        return nullptr;
    }
}

//
bool WelcomeScene::initWithFirst(const bool first)
{
    m_first = first;
    addLrMask(this);
    
    auto size = Director::getInstance()->getVisibleSize();
    m_pbackground = Sprite::create(BG_PNG_FILE);
    m_pbackground->setPosition(Vec2(size.width / 2, size.height / 2));
    addChild(m_pbackground, 1);
    
    m_pContainerNode = Node::create();
    m_pContainerNode->setPosition(Vec2(m_sceneOffset, 0.0f));
    addChild(m_pContainerNode, 3);
    
    m_ptitleActor = Actor::createWithGafConfigFile(TITLE_GAF_CONFIG_FILE);
    this->addChild(m_ptitleActor, 1);
    m_ptitleActor->setPosition(Vec2(m_sceneOffset + 34.0f, 1604.0f));
    if (m_first)
    {
        m_ptitleActor->playWithSequenceAliasName("standby", false);
        m_ptitleActor->setAnimationFinishedPlayDelegate([this]
        {
            m_ptitleActor->playWithSequenceAliasName("trigger", true);
        });
    } else {
        m_ptitleActor->playWithSequenceAliasName("trigger", true);
    }
    
    // 加载颜色蒙版层
    m_playerColor = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    addChild(m_playerColor, 2);
    m_playerColor->setVisible(false);
    
    // 播放启动音乐
    if (first) {
        MyAudioEngine::getInstance()->playAudio(STARTUP_AUDIO);
        scheduleOnce([this](float dt)
                     {
                         if (!m_isOpenBag) {
                             MyAudioEngine::getInstance()->playAudio(BACKGROUND_MUSIC_FILE,true,true,0.4f);
                         }
                     },STARTUP_AUDIO_DURATION, "playBgAudio");
    } else {
        MyAudioEngine::getInstance()->playAudio(BACKGROUND_MUSIC_FILE,true,true,0.4f);
    }
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener(NPCBAG_END_CALLBACK, [this] (EventCustom *)
   {
       m_isOpenBag = false;
       MyAudioEngine::getInstance()->playAudio(BACKGROUND_MUSIC_FILE,true,true,0.4f);
       restoreUi();
   });
    return true;
}

//
void WelcomeScene::onEnter()
{
    Scene::onEnter();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_androidRollBackListener = AndroidRollBackListener::create(1);
    m_androidRollBackListener->backScene();
#endif
    
    if (m_first)
    {
        // 预加载资源
        auto preloadedCB = [this]
        {
            initUi();
        };
        PreLoadUIManager::getInstance()->startPreloadUI(preloadedCB);
    } else {
        initUi();
    }
}

//
void WelcomeScene::moveOutUi()
{
    m_pEnterBtn->setPosition(Vec2(-10000.0f, 0.0f));
    m_pplayBack->setPosition(Vec2(-10000.0f, 0.0f));
    m_pBagActor->setPosition(Vec2(-10000.0f, 0.0f));
    m_pPlayBtn->setPosition(Vec2(-10000.0f, 0.0f));
}

//
void WelcomeScene::restoreUi()
{
    float centerX = AppConfig::getInstance()->getDesignResolutionWidth() / 2.0f;
    m_pEnterBtn->setPosition(Vec2(centerX+m_sceneOffset, ENTER_BTN_POSY));
    m_pplayBack->setPosition(Vec2(m_playBtnPos.x - 2.0f, m_playBtnPos.y - 2.0f));
    m_pBagActor->setPosition(m_bagPos);
    m_pPlayBtn->setPosition(m_playBtnPos);
}

//
void WelcomeScene::onExit()
{
    Scene::onExit();
}

/*
 *  初始化按钮菜单
 */
void WelcomeScene::initUi()
{
    
    if (m_first) {
        //
        auto pGafAssetCache = GafAssetCache::getInstance();
        auto touyingGafLoadedCB = [this] (GAFAsset* pGAFAsset) {
            if (++m_touyingResLoaded == 3) {
                m_ptouyingActor = Actor::createWithGafConfigFile(TOUYING_GAF_CONFIG_FILE);
                m_pContainerNode->addChild(m_ptouyingActor, 7);
                
                m_paiMiActor = Actor::createWithGafFile(AIMI_GAF_FILE, false);
                m_paiMiActor->setPosition(Vec2(-10000.0f, 0.0f));
                m_pContainerNode->addChild(m_paiMiActor, 8);
                m_paiMiActor->stop();
                m_pbeiBeiActor = Actor::createWithGafFile(BEIBEI_GAF_FILE, false);
                m_pbeiBeiActor->setPosition(Vec2(-1000.0f, 0.0f));
                m_pContainerNode->addChild(m_pbeiBeiActor, 9);
                m_pbeiBeiActor->stop();
                m_touyingResReady = true;
            }
        };
        pGafAssetCache->addGafAssetAsync(BEIBEI_GAF_FILE, touyingGafLoadedCB);
        pGafAssetCache->addGafAssetAsync(AIMI_GAF_FILE, touyingGafLoadedCB);
        pGafAssetCache->addGafAssetAsync(TOUYING_GAF_FILE, touyingGafLoadedCB);
    }else
    {
        m_ptouyingActor = Actor::createWithGafConfigFile(TOUYING_GAF_CONFIG_FILE);
        m_pContainerNode->addChild(m_ptouyingActor, 7);
        
        m_paiMiActor = Actor::createWithGafFile(AIMI_GAF_FILE, false);
        m_paiMiActor->setPosition(Vec2(-10000.0f, 0.0f));
        m_pContainerNode->addChild(m_paiMiActor, 8);
        m_paiMiActor->stop();
        m_pbeiBeiActor = Actor::createWithGafFile(BEIBEI_GAF_FILE, false);
        m_pbeiBeiActor->setPosition(Vec2(-1000.0f, 0.0f));
        m_pContainerNode->addChild(m_pbeiBeiActor, 9);
        m_pbeiBeiActor->stop();
        m_touyingResReady = true;
    }
    
    float centerX = AppConfig::getInstance()->getDesignResolutionWidth() / 2.0f;
    m_pEnterBtn = cocos2d::ui::Button::create("kaishianniu.png","kaishianniu02.png","kaishianniu.png",cocos2d::ui::Widget::TextureResType::PLIST);
    m_pEnterBtn->setPosition(Vec2(centerX+m_sceneOffset, ENTER_BTN_POSY));
    m_pEnterBtn->setEnabled(false);
    this->addChild(m_pEnterBtn, 1);
    if (m_first) {
        m_pEnterBtn->setScale(0.0f);
    }
    m_pEnterBtn->addTouchEventListener([this](Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
    {
       if (cocos2d::ui::Widget::TouchEventType::ENDED == type)
       {
           MyAudioEngine::getInstance()->playAudio(BUTTON_AUDIO);
           MyAudioEngine::getInstance()->stopAudio(0,true);
           m_isOpenBag = true;
           // 进入冒险模式主题选择
           Director::getInstance()->replaceScene(LoadingScene::createWithIndex(2));
           m_pEnterBtn->setEnabled(false);
       }
    });
    
    // 播放按钮
    m_pplayBack = ImageView::create("dibu01.png", cocos2d::ui::Widget::TextureResType::PLIST);
    m_pplayBack->setPosition(Vec2(m_playBtnPos.x - 2.0f, m_playBtnPos.y - 2.0f));
    m_pContainerNode->addChild(m_pplayBack, BTN_BACK_ZORDER);
    m_pplayBack->setVisible(false);
    
    m_pPlayBtn = cocos2d::ui::Button::create("fangyingji01.png","fangyingji03.png","fangyingji01.png",cocos2d::ui::Widget::TextureResType::PLIST);
    m_pPlayBtn->setPosition(m_playBtnPos);
    m_pPlayBtn->setScale(0.0f);
    m_pContainerNode->addChild(m_pPlayBtn, 10);
    
    // NPC 背包按钮
    m_pBagActor = Actor::createWithGafConfigFile(BAG_GAF_CONFIG_FILE);
    m_pBagActor->playWithSequenceAliasName("standby", true);
    
    m_pBagActor->setPosition(m_bagPos);
    if (m_first) {
        m_pBagActor->setScale(0.0f);
    }
    m_pBagActor->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pContainerNode->addChild(m_pBagActor, 10);
    
    // enter按钮动作
    enterBtnAction();
}

// enter按钮动作
void WelcomeScene::enterBtnAction()
{
    m_pEnterBtn->runAction(Sequence::create(DelayTime::create(1.0f),EaseBackOut::create(ScaleTo::create(1.0f, BTN_SCALE_TIME)),CallFunc::create([this]{
        m_pEnterBtn->setEnabled(true);
        RepeatForever* repeat = RepeatForever::create(Sequence::create(ScaleTo::create(BTN_SCALE_TIME, 0.8f),ScaleTo::create(BTN_SCALE_TIME, 1.0f), NULL));
        m_pEnterBtn->runAction(repeat);
        bagBtnActions();          // 背包按钮动作
    }), NULL));
    
    //
    playBtnActions();
}

/*
 *  投影播放按钮动作
 */
void WelcomeScene::playBtnActions()
{

    auto scaleCompletedCB = CallFunc::create([this] {
        m_pPlayBtn->addTouchEventListener([this](Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
        {
            if (cocos2d::ui::Widget::TouchEventType::BEGAN == type) {
                if (m_isplayActor) {
                    m_pplayBack->loadTexture("dibu02.png",cocos2d::ui::Widget::TextureResType::PLIST);   // 更换播放按钮底图
                } else
                {
                    m_pplayBack->loadTexture("dibu01.png",cocos2d::ui::Widget::TextureResType::PLIST);   // 更换播放按钮底图
                }
            } else if (cocos2d::ui::Widget::TouchEventType::MOVED == type)
            {
                m_pplayBack->loadTexture("dibu01.png",cocos2d::ui::Widget::TextureResType::PLIST);   // 更换播放按钮底图
              
            } else if (cocos2d::ui::Widget::TouchEventType::ENDED == type)
            {
                if (!m_touyingResReady) /*投影相关资源还未准备好*/  {
                  return;
                }
                MyAudioEngine::getInstance()->playAudio(BUTTON_AUDIO);
                if (m_isplayActor)
                {
                  MyAudioEngine::getInstance()->stopAudio(m_effectId);      //停止介绍音效
                  m_pplayBack->loadTexture("dibu02.png",cocos2d::ui::Widget::TextureResType::PLIST);   // 更换播放按钮底图
                  m_pPlayBtn->loadTextures("fangyingji01.png","fangyingji03.png","fangyingji01.png",cocos2d::ui::Widget::TextureResType::PLIST);                 // 播放完恢复到未播放状态图
                  m_playerColor->setVisible(false);                                 // 隐藏颜色蒙版层
                  m_pPlayBtn->stopActionByTag(5000);                   // 停止play按钮滚动动作
                  m_pPlayBtn->setScale(0.0f);
                  m_pPlayBtn->runAction(Spawn::create(RotateTo::create(ROTATE_TIME, 0),EaseBackOut::create(ScaleTo::create(BTN_SCALE_TIME, 1.0f)), NULL));
                  m_pEnterBtn->setEnabled(true);
                  m_ptouyingActor->setPosition(Vec2(-10000.0f, 0.0f));
                  m_ptouyingActor->stop();
                  m_paiMiActor->setPosition(Vec2(-10000.0f, 0.0f));
                  m_paiMiActor->stop();
                  m_pbeiBeiActor->setPosition(Vec2(-10000.0f, 0.0f));
                  m_pbeiBeiActor->stop();
                  m_isplayActor = false;
                  m_pplayBack->setVisible(false);
                } else {
                  m_isplayActor = true;
                  m_pPlayBtn->loadTextures("lunzi01.png","lunzi02.png","lunzi01.png", cocos2d::ui::Widget::TextureResType::PLIST);   //录制状态图
                  RepeatForever* repeat = RepeatForever::create(RotateBy::create(ROTATE_TIME, 360));
                  repeat->setTag(5000);
                  m_pPlayBtn->runAction(repeat);
                  m_pplayBack->setVisible(true);
                  m_pEnterBtn->setEnabled(false);
                  addTouyingActor();                        //加载投影gaf
                }
            }
        });
    });
    m_pPlayBtn->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(BTN_SCALE_TIME, 1.0f)), scaleCompletedCB, NULL));
}

// 背包按钮动作
void WelcomeScene::bagBtnActions()
{
    auto bagListenCallFunc = CallFunc::create( [this]
    {
        auto eventListenForBag = EventListenerTouchOneByOne::create();
        eventListenForBag->onTouchBegan = [this] (Touch* pTouch, Event *pEvent) ->bool
        {
          if (m_isplayActor) {
              return false;
          }
          auto pTarget = (Actor*)pEvent->getCurrentTarget();
          auto touchPoint = pTarget->convertTouchToNodeSpace(pTouch);
          auto touchArea = Rect(0.0f, 0.0f, pTarget->getSize().width, pTarget->getSize().height);
          if (touchArea.containsPoint(touchPoint) )
          {
              m_pBagActor->setScale(1.2f);
              return true;
          }
          return false;
        };
        eventListenForBag->onTouchEnded = [this](Touch* pTouch, Event* pEvent)
        {
          m_pBagActor->setScale(1.0f);
          MyAudioEngine::getInstance()->playAudio(BUTTON_AUDIO);
          if (m_first) {
              if (m_effectId != -1) {
                  MyAudioEngine::getInstance()->stopAudio(m_effectId);
              }
          }
          MyAudioEngine::getInstance()->stopAudio(0,true);
          m_isOpenBag = true;
          moveOutUi();
          auto npcBagNode = NpcBagNode::create();
          npcBagNode->setPosition(Vec2(0.0f + m_sceneOffset, 0.0f));
          addChild(npcBagNode, 100);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            m_androidRollBackListener->setNpcBagNode(npcBagNode);
            m_androidRollBackListener->setIsNpcBagNode(true);
#endif
        };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListenForBag, m_pBagActor);
    });
    m_pBagActor->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(BTN_SCALE_TIME, 1.0f)), bagListenCallFunc, NULL));
}

// 投影
void WelcomeScene::addTouyingActor()
{
    // 显示颜色蒙版层
    m_playerColor->setVisible(true);
    m_ptouyingActor->setPosition(Vec2(-30.0f, 1490.0f));
    m_ptouyingActor->playWithSequenceAliasName("trigger", false);
    m_ptouyingActor->setAnimationFinishedPlayDelegate([this]
                                                      {
                                                          m_ptouyingActor->playWithSequenceAliasName("standby", true);
                                                      });
    
    m_paiMiActor->setPosition(Vec2(330.0f, 1300.0f));
    m_paiMiActor->start();
    playEffectAimi();       //播放艾米介绍音效
    m_paiMiActor->setAnimationFinishedPlayDelegate([this]
    {
       m_paiMiActor->setPosition(Vec2(-10000.0f, 0.0f));
       MyAudioEngine::getInstance()->stopAudio(m_effectId);       //停止艾米介绍音效
       m_pbeiBeiActor->start();
       m_pbeiBeiActor->setAnimationFinishedPlayDelegate([this] {
           m_pbeiBeiActor->setPosition(Vec2(-10000.0f, 0.0f));
           m_ptouyingActor->setPosition(Vec2(-10000.0f, 0.0f));
           MyAudioEngine::getInstance()->stopAudio(m_effectId);          //停止蓓蓓介绍音效
           m_pplayBack->setVisible(false);               //隐藏播放按钮底图
           m_pPlayBtn->loadTextures("fangyingji01.png","fangyingji03.png","fangyingji01.png", cocos2d::ui::Widget::TextureResType::PLIST);  //播放完恢复到未播放状态图
           m_pplayBack->loadTexture("dibu01.png",cocos2d::ui::Widget::TextureResType::PLIST);   // 更换播放按钮底图
           m_playerColor->setVisible(false);                   //隐藏颜色蒙版层
           m_pPlayBtn->stopActionByTag(5000);
           m_pPlayBtn->setScale(0.0f);
           m_pPlayBtn->runAction(Spawn::create(RotateTo::create(ROTATE_TIME, 0),EaseBackOut::create(ScaleTo::create(BTN_SCALE_TIME, 1.0f)), NULL));
           
           m_pPlayBtn->setEnabled(true);
           m_pEnterBtn->setEnabled(true);
           m_isplayActor = false;
       });
       playEffectBeibei();   //播放蓓蓓介绍音效
       m_pbeiBeiActor->setPosition(Vec2(298.0f, 1245.0f));
    });
}

// 播放艾米介绍音效
void WelcomeScene::playEffectAimi()
{
    m_effectId = MyAudioEngine::getInstance()->playAudio(AIMI_INTRO_AUDIO_FILE);
}

// 播放蓓蓓介绍音效
void WelcomeScene::playEffectBeibei()
{
    m_effectId = MyAudioEngine::getInstance()->playAudio(BEIBEI_INTRO_AUDIO_FILE);
}


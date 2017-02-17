//
//  UIBase.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 2015/04/16.
//
//

#include "UIBase.h"
#include "Actor.h"
#include "MyAudioEngine.h"
#include "AppConfig.h"
#include "SelStageScene.h"
#include "ThemeSelScene.h"
#include "NpcBagNode.h"
#include "NpcCollectCompleteNode.h"
#include "StageDataManager.h"
#include "Player.h"
#include "StageData.h"
#include "StageScene.h"
#include "CustomMessage.h"
#include "MobClickCpp.h"
#include "LoadingScene.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidRollBackListener.h"
#endif
using namespace std;

#define RETURN_BTN_POS                      Vec2(131, 1409)                                  //返回按钮坐标
#define MUSIC_BTN_POS                       Vec2(1918, 1409)                                //声音按钮坐标
#define UI_BN_PLIST_FILE                    "res/common/ui/zhujiemian.plist"                //BN的plist
#define BAG_GAF_CONFIG_FILE                 "config/bag.json"                               //背包gaf配置文件
#define BAG_GAF_POS                         Vec2(1898.0f, 131.0f)                           //背包gaf的坐标
#define BAG_GAF_ZORDER                      2                                               //背包gaf的z值
#define STANDBY                             "standby"                                       //待机
#define TRIGERONE                           "triggerone"                                     //触发1抖动
#define TRIGERTWO                           "triggertwo"                                     //触发2跳跃
#define BTN_EFFECT_FILE                     "res/audio/ui/UIanniu01.mp3"
#define BAG_CLICK_MUSIC_FILE                "res/audio/ui/beibaodianji.mp3"
#define CLICK_GAF_FILE                      "res/common/SL-dianjichufa/SL-dianjichufa.gaf"   //点击gaf效果
#define BAG_JUMP_MUSIC_FILE                 "res/audio/ui/beibaoJUMP01.mp3"

/*
 *  constructor
 */
UIBase::UIBase() {}

/*
 *  destructor
 */
UIBase::~UIBase()
{
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pBagActor);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeCustomEventListeners(NPCBAG_END_CALLBACK);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_androidRollBackListener = nullptr;
#endif
}

void UIBase::onEnter()
{
    //////
    Node::onEnter();
}

//
UIBase * UIBase::create(unsigned int sceneType, const bool isSceneOffset)
{
    UIBase * ret = new UIBase();
    if (ret && ret->initUILayer(sceneType, isSceneOffset))
    {
        ret->autorelease();
    } else {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

//
bool UIBase::initUILayer(const unsigned int scenetype, const bool isSceneOffset)
{
    m_isSceneOffset = isSceneOffset;
    //BN
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(UI_BN_PLIST_FILE);
    //触摸响应层
    m_pSuperTouchLayer = Layer::create();
    addChild(m_pSuperTouchLayer);
    
    //返回按钮
    m_pReturnBtn = ui::Button::create("UI-fanhui01.png","UI-fanhui02.png","UI-fanhui01.png",ui::Widget::TextureResType::PLIST);
    m_pReturnBtn->setPosition(RETURN_BTN_POS);
    m_pReturnBtn->setScale(0.0f);
    m_pReturnBtn->runAction(ScaleTo::create(0.5f, 1.0f));
    m_pSuperTouchLayer->addChild(m_pReturnBtn);
    m_pReturnBtn->addTouchEventListener([this,scenetype](Ref* sender, ui::Widget::TouchEventType type)
    {
     if (type == ui::Widget::TouchEventType::ENDED)
     {
         m_pReturnBtn->setEnabled(false);
         if (scenetype == 0) {      //从关卡选择返回主题选择
             // 返回主题选择
             AppConfig::getInstance()->setPassTimes(0);     //退出关卡选择，就不能是连续通过关卡，将通过关卡数重置为0（用于动动操播放）
             Director::getInstance()->replaceScene(LoadingScene::createWithIndex(2));
         } else if(scenetype == 1)   //从回合返回关卡选择
         {
             // 友盟过关失败统计
             char level[16];
             sprintf(level, "level-%02d",m_curLevel+1);
             umeng::MobClickCpp::failLevel(level);
             
             // 返回关卡选择场景
             if (m_topicAudioId != 0) {
                 MyAudioEngine::getInstance()->stopAudio(m_topicAudioId);     //停止音效
                 m_topicAudioId = 0;
             }
             if (m_pStageScene->isTrial())
             {
                 Director::getInstance()->replaceScene(LoadingScene::createWithIndex(2));
             } else {
                 // 返回
                 returnStage();
             }
         }
         MyAudioEngine::getInstance()->playAudio(BTN_EFFECT_FILE);       //按钮音效
     }
    });
    
    m_pMusicSwitchBtn = ui::Button::create("UI-yinyue01.png","UI-yinyue03.png","UI-yinyue02.png",ui::Widget::TextureResType::PLIST);
    auto musicPos = MUSIC_BTN_POS;
    if (m_isSceneOffset)
    {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        musicPos.x += visibleSize.width - AppConfig::getInstance()->getDesignResolutionWidth();
    }
    m_pMusicSwitchBtn->setPosition(musicPos);
    m_pSuperTouchLayer->addChild(m_pMusicSwitchBtn);
    
    updateMusicOpenBtn();
    //背景音乐开关
    m_pMusicSwitchBtn->addTouchEventListener([this](Ref* sender, ui::Widget::TouchEventType type)
    {
     if (type == ui::Widget::TouchEventType::ENDED)
     {
         MyAudioEngine::getInstance()->playAudio(BTN_EFFECT_FILE);          //按钮音效
         if (AppConfig::getInstance()->getBackMusicOpen())
         {
             MyAudioEngine::getInstance()->pauseAudio(0,true);        //暂停背景音乐
         }else
         {
             MyAudioEngine::getInstance()->resumeAudio(0,true);       //恢复背景音乐
         }
         AppConfig::getInstance()->setBackMusicOpen(!AppConfig::getInstance()->getBackMusicOpen());
         updateMusicOpenBtn();
     }
    });
    
    // 背包gaf
    m_pBagActor = Actor::createWithGafConfigFile(BAG_GAF_CONFIG_FILE);
    m_pBagActor->setAnchorPoint(Vec2(0.5f, 0.5f));
    auto bagPos = BAG_GAF_POS;
    if (m_isSceneOffset)
    {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        bagPos.x += visibleSize.width - AppConfig::getInstance()->getDesignResolutionWidth();
    }
    m_pBagActor->setPosition(bagPos);
    m_pSuperTouchLayer->addChild(m_pBagActor, BAG_GAF_ZORDER);
    m_pBagActor->playWithSequenceAliasName(STANDBY,true);
    
    if (scenetype == 0) {
        openBagTouch();     //开启触摸
    }
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->addCustomEventListener(NPCBAG_END_CALLBACK, [this] (EventCustom *)
   {
       Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(m_pBagActor);
       char yyBuffer[64];
       auto themeIndex = UserDefault::getInstance()->getIntegerForKey("curThemeIndex");
       sprintf(yyBuffer, "config/selStage/selStage00%d.json",themeIndex+1);
       std::string bgMusic = getBackMusic(yyBuffer);
       MyAudioEngine::getInstance()->playAudio(bgMusic.c_str(),true,true);
   });
    
    return true;
}

//开启背包触摸
void UIBase::openBagTouch()
{
    //注册触摸监听
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listenerDrag = EventListenerTouchOneByOne::create();
    //    listenerDrag->setSwallowTouches(true);
    listenerDrag->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        if (rect.containsPoint(touchPoint)) {
            if (m_isBagEnabled) {
                m_pBagActor->setScale(1.2f);
            }            
            return true;
        }
        return false;
    };
    listenerDrag->onTouchEnded = [this](Touch* touch, Event* event)
    {
        auto target = static_cast<Node*>(event->getCurrentTarget());
        auto touchPoint = target->convertTouchToNodeSpace(touch);
        Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
        m_pBagActor->setScale(1.0f);
        if (rect.containsPoint(touchPoint))
        {
            if (m_isBagEnabled)
            {
                MyAudioEngine::getInstance()->playAudio(BAG_CLICK_MUSIC_FILE);
                Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(m_pBagActor);
                //进入背包界面
                auto npcBagNode = NpcBagNode::create();
                npcBagNode->setPosition(Vec2::ZERO);
                addChild(npcBagNode, 100);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (m_androidRollBackListener != nullptr) {
        m_androidRollBackListener->setNpcBagNode(npcBagNode);
        m_androidRollBackListener->setIsNpcBagNode(true);
    }
#endif
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerDrag, m_pBagActor);
}

//
std::string UIBase::getBackMusic(const std::string& stageConfigFile)
{
    std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(stageConfigFile);
    rapidjson::Document doc;
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    // 背景音乐
    std::string bgMusic = doc["bgMusic"].GetString();
    return bgMusic;
}

//更新背景音乐开关按钮状态
void UIBase::updateMusicOpenBtn()
{
    if (m_pMusicSwitchBtn != nullptr) {
        if (AppConfig::getInstance()->getBackMusicOpen())
        {
            m_pMusicSwitchBtn->loadTextures("UI-yinyue01.png","UI-yinyue03.png","UI-yinyue02.png",ui::Widget::TextureResType::PLIST);
        } else {
            m_pMusicSwitchBtn->loadTextures("UI-yinyue02.png","UI-yinyue03.png","UI-yinyue01.png",ui::Widget::TextureResType::PLIST);
        }
    }
}

//改变背包gaf动作
void UIBase::changeBagGafActor(unsigned int index)
{
    if (index == 1) {
        m_pBagActor->playWithSequenceAliasName(TRIGERONE);
        m_pBagActor->setAnimationFinishedPlayDelegate([this]
        {
            m_pBagActor->playWithSequenceAliasName(STANDBY, true);
        });
    } else if (index == 2) {
        MyAudioEngine::getInstance()->playAudio(BAG_JUMP_MUSIC_FILE);
        m_pBagActor->playWithSequenceAliasName(TRIGERTWO);
        m_pBagActor->setAnimationFinishedPlayDelegate([this]
        {
            m_pBagActor->playWithSequenceAliasName(STANDBY,true);
            addCollectNpcNode();     //显示收集npc介绍界面
        });
    }
}

//得到收集npc的key
void UIBase::setCollectNpcKey(std::string key)
{
    m_npcKey = key;
}

//添加npc收集展示界面
void UIBase::addCollectNpcNode()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    
    m_pNpcCollectNode = NpcCollectCompleteNode::createWithNpcKey(m_npcKey);
    m_pNpcCollectNode->setScale(0.0f);
    m_pNpcCollectNode->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pNpcCollectNode->setPosition(Vec2(dw / 2.0f, visibleSize.height / 2.0f));
    addChild(m_pNpcCollectNode, 10);
}

//
Vec2 UIBase::getNpcBagPos()
{
    Vec2 bagPosition = BAG_GAF_POS;
    if (m_pBagActor != nullptr)
    {
        bagPosition = m_pBagActor->getPosition();
        bagPosition.x += m_pBagActor->getSize().width / 2.0f;
        bagPosition.y += m_pBagActor->getSize().height / 2.0f;
    }
    return bagPosition;
}

//
void UIBase::returnStage()
{
    auto stageIndex = StageDataManager::getInstance()->getStageData(Player::getInstance()->getCurrStageIndex())->getStageIndex();
    auto themeIndex = (stageIndex - (stageIndex % 12)) / 12;
    
    // 返回大地图
    char szBuffer[64];
    sprintf(szBuffer, "config/selStage/selStage%03d.json", themeIndex + 1);
    Director::getInstance()->replaceScene(SelStageScene::createWithStageConfig(szBuffer));
}

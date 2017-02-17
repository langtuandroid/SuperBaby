//
//  SelStageScene.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/5/15.
//
//

#include "SelStageScene.h"
#include "Actor.h"
#include "LoadingStageScene.h"
#include "MyAudioEngine.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "UIBase.h"
#include "AppConfig.h"
#include "StageDataManager.h"
#include "GafAssetCache.h"
#include "LoadingNode.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidRollBackListener.h"
#endif

#define UI_CONTAINER_ZORDER                 101
#define AIMEE_FRAME                         "SL-MAP-LOGO.png"
#define POINT_FRAME                         "SL-MAP-DIAN.png"
#define HEAD_ZORDER                         10

#define LIGHT_GAF                           "res/selStage/SL-MAP-guang/SL-MAP-guang.gaf"

#define HEAD_UP_OFFSET                      70.0f
#define HEAD_MOVE_DURATION                  0.8f

#define DONGDONGCAO_ZORDER                  102
#define TOUCHOFFSETX                        20.0f

#define DONGDONGCAO_GAF_FILE                "res/common/ce2-ddc/ce2-ddc.gaf"
#define DONGDONGCAO_AUDIO_FILE              "res/common/cs-ddc.mp3"

#define DONGDONGCAO_MP4_FILE                "res/video/dongdongcao.mp4"

#define CLICK_MUSIC_FILE                    "res/audio/stageSel/ditudianji.mp3"

#define BG_LEFT_IMAGE               "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE              "res/common/bgRight.png"

/*
 *  constructor
 */
SelStageScene::SelStageScene():
m_pStageDataManager(StageDataManager::getInstance()),
m_pGafAssetCache(GafAssetCache::getInstance())
{
    m_sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
}

/*
 *  desstructor
 */
SelStageScene::~SelStageScene()
{
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    for (auto &it : m_SpriteFrameFiles) {
        spriteFrameCache->removeSpriteFramesFromFile(it);
    }
    auto textureCache = Director::getInstance()->getTextureCache();
    for (auto &it : m_preloadTextures) {
        textureCache->removeTextureForKey(it);
    } /*for*/
    for (auto &it : m_preloadGafs) {
        m_pGafAssetCache->removeGafAssetForKey(it);
    } /*for*/
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if (m_androidRollBackListener != nullptr) {
        delete m_androidRollBackListener;
        m_androidRollBackListener = nullptr;
    }
#endif
}

//
Scene* SelStageScene::createWithStageConfig(const std::string& stageConfigFile)
{
    SelStageScene* pScene = new SelStageScene();
    if (pScene && pScene->initWithStageConfig(stageConfigFile))
    {
        pScene->autorelease();
        return pScene;
    } else {
        CC_SAFE_DELETE(pScene);
        return nullptr;
    }
}

//
bool SelStageScene::initWithStageConfig(const std::string& stageConfigFile)
{
    std::string filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(stageConfigFile);
    m_contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);
    
    
    auto sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
    m_pLoadingNode = LoadingNode::create();
    m_pLoadingNode->setPosition(Vec2(sceneOffset, 0.0f));
    addChild(m_pLoadingNode, 100);
    
    // 左右遮罩
    if (m_sceneOffset > 0.0f)
    {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        
        auto bgLeftSp = Sprite::create(BG_LEFT_IMAGE);
        bgLeftSp->setAnchorPoint(Vec2(1.0f, 0.0f));
        bgLeftSp->setPosition(Vec2(m_sceneOffset, 0.0f));
        bgLeftSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgLeftSp->setScaleX(m_sceneOffset / bgLeftSp->getContentSize().width);
        addChild(bgLeftSp, 1000);
        
        auto bgRightSp = Sprite::create(BG_RIGHT_IMAGE);
        bgRightSp->setAnchorPoint(Vec2(0.0f, 0.0f));
        bgRightSp->setPosition(Vec2(m_sceneOffset + AppConfig::getInstance()->getDesignResolutionWidth(), 0.0f));
        bgRightSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgRightSp->setScaleX(m_sceneOffset / bgLeftSp->getContentSize().width);
        addChild(bgRightSp, 1000);
        
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        
        listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
        {
            auto target = static_cast<Sprite*>(event->getCurrentTarget());
            auto touchPoint = target->convertTouchToNodeSpace(touch);
            Rect rect = Rect(0.0f, 0.0f, target->getContentSize().width, target->getContentSize().height);
            if (rect.containsPoint(touchPoint)) {
                return true;
            }
            return false;
        };
        
        dispatcher->addEventListenerWithSceneGraphPriority(listener, bgLeftSp);
        auto lis1 = listener->clone();
        dispatcher->addEventListenerWithSceneGraphPriority(lis1, bgRightSp);
        dispatcher->setPriority(listener, -130);
        dispatcher->setPriority(lis1, -130);
    }
    
    //
    scheduleUpdate();
    return true;
}

//
void SelStageScene::onEnter()
{
    ////////////////
    Scene::onEnter();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    m_androidRollBackListener = AndroidRollBackListener::create(3);
    m_androidRollBackListener->backScene();
#endif
    
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(m_contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    // 关卡所属主题索引号
    m_themeIndex = doc["themeIndex"].GetInt();
    
    m_stageClearIndex = AppConfig::getInstance()->getStageClearIndex();
    m_maxStageClearIndex = m_pStageDataManager->getMaxClearStageIndex(m_themeIndex);
    
//    log("m_maxStageClearIndex = %d m_stageClearIndex = %d", m_maxStageClearIndex, m_stageClearIndex);
    
    if (m_stageClearIndex >= 0) /*有通关*/
    {
        AppConfig::getInstance()->setStageClearIndex(-1);
        m_hasStageClear = true;
    }
    
    if (m_maxStageClearIndex < 0)
    {
        m_headIndex = 0;
    } else {
        m_headIndex = m_maxStageClearIndex % 12;
        if (m_hasStageClear) /* 有过关 */
        {
            if (11 == m_headIndex)
            {
                m_needPopStageClearAward = true;
            } else {
                m_needMoveHead = true;
            }
        } else {
            if (m_headIndex < 11) {
                m_headIndex++;
            }
        }
    }
    
    rapidjson::Value &preloadValue = doc["preload"];
    rapidjson::Value &preloadImgsValue = preloadValue["imgs"];
    for (int i = 0; i < preloadImgsValue.Size(); i++)
    {
        m_preloadTextures.push_back(preloadImgsValue[i].GetString());
    } /**/
    rapidjson::Value &preloadPlistsValue = preloadValue["plists"];
    for (int i = 0; i < preloadPlistsValue.Size(); i++)
    {
        m_SpriteFrameFiles.push_back(preloadPlistsValue[i].GetString());
    } /**/
    rapidjson::Value &preloadGafValue = preloadValue["gafs"];
    for (int i = 0; i < preloadGafValue.Size(); i++)
    {
        m_preloadGafs.push_back(preloadGafValue[i].GetString());
    } /**/
    
    rapidjson::Value &stageAwardGaf = doc["awardGaf"];
    m_stageClearAwardTriggerGafConfig = stageAwardGaf["trigger"].GetString();
    
    auto textureLoadedCB = [this] (Texture2D* pTexture2D)
    {
        if (++m_numberOfPreLoadedTextures == m_preloadTextures.size())
        {
            auto spriteFrameCache = SpriteFrameCache::getInstance();
            for (auto &it : m_SpriteFrameFiles) {
                spriteFrameCache->addSpriteFramesWithFile(it);
            }
            preloadGafs();
        }
    };
    auto textureCache = Director::getInstance()->getTextureCache();
    for (auto &it : m_preloadTextures) {
        textureCache->addImageAsync(it, textureLoadedCB);
    } /*for*/
}

//
void SelStageScene::preloadGafs()
{
    auto loadedGafsCB = [this] (GAFAsset* pGAFAsset)
    {
        if (++m_numberOfPreLoadedGafs == m_preloadGafs.size())
        {
            m_preloaded = true;
        }
    };
    auto pGafAssetCache = GafAssetCache::getInstance();
    for (auto &it : m_preloadGafs)
    {
        pGafAssetCache->addGafAssetAsync(it, loadedGafsCB);
    } /*for*/
}

//
void SelStageScene::update(float dt)
{
    if (!m_preloaded) return;
    unscheduleUpdate();
    
    //
    updateUi();
    
    if (m_pLoadingNode != nullptr)
    {
        m_pLoadingNode->removeFromParent();
        m_pLoadingNode = nullptr;
    }
    
    if (m_hasStageClear) /* 有通关 */
    {
        if (m_needMoveHead) {
            startMoveHead();
        } else if (m_needPopStageClearAward) {
            popStageClearAward();
        }
    } else {
        // 如果连续玩了两关，需要播放动动操
        if (AppConfig::getInstance()->getPassTimes() == 2)
        {
            AppConfig::getInstance()->setPassTimes(0);    // 重置为0
            playDongDongCao();
        } else {
            bindBtnsEvent();
        }
    }
}

//
void SelStageScene::updateUi()
{
    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(m_contentStr.c_str());
    if (doc.HasParseError()) {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    m_contentStr = "";
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    
    m_pScrollView = ui::ScrollView::create();
    m_pScrollView->setDirection(ui::ScrollView::Direction::HORIZONTAL);
    m_pScrollView->setAnchorPoint(Vec2::ZERO);
    m_pScrollView->setPosition(Vec2(m_sceneOffset, 0.0f));
    m_pScrollView->setContentSize(Size(dw, visibleSize.width));
    
    auto ui = UIBase::create(0, false);
    ui->setPosition(Vec2(m_sceneOffset, 0.0f));
    addChild(ui, UI_CONTAINER_ZORDER);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    ui->setAndroidRollBackListener(m_androidRollBackListener);
#endif
    
    // 背景音乐
    m_bgMusic = doc["bgMusic"].GetString();
    m_bgMusicVolume = (float)doc["bgMusicVolume"].GetDouble();

    if (!MyAudioEngine::getInstance()->isPlaying()) {
        if (AppConfig::getInstance()->getBackMusicOpen()) {
            MyAudioEngine::getInstance()->playAudio(m_bgMusic.c_str(),true,true,m_bgMusicVolume);
        }
    }
    
    // 背景图
    std::string bgImg = doc["bgImg"].GetString();
    m_pBgSprite = Sprite::create(bgImg);
    m_pBgSprite->setAnchorPoint(Vec2::ZERO);
    m_pBgSprite->setPosition(Vec2::ZERO);
    m_pScrollView->addChild(m_pBgSprite, 1);
    m_pScrollView->setInnerContainerSize(m_pBgSprite->getContentSize());
    addChild(m_pScrollView, 1);
    
    // 包含头像和按钮大图
    rapidjson::Value &mapFileValue = doc["mapFile"];
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(mapFileValue["mapPlist"].GetString());
    m_pSbn = SpriteBatchNode::create(mapFileValue["mapPng"].GetString());
    m_pBgSprite->addChild(m_pSbn, mapFileValue["zorder"].GetInt());
    
    // 加载 gaf
    rapidjson::Value &gafObjs = doc["gafObjs"];
    for (unsigned int i = 0; i < gafObjs.Size(); i++)
    {
        rapidjson::Value &gafObj = gafObjs[i];
        auto actor = Actor::createWithGafFile(gafObj["gafFile"].GetString());
        actor->setPosition(Vec2(gafObj["posx"].GetInt(), gafObj["posy"].GetInt()));
        if (!gafObj["width"].IsNull() && !gafObj["height"].IsNull())
        {
            actor->setSize(Size(gafObj["width"].GetInt(), gafObj["height"].GetInt()));
        }
        if (!gafObj["posAtNodeX"].IsNull() && !gafObj["posAtNodeY"].IsNull())
        {
            actor->setPosAtNode(Vec2(gafObj["posAtNodeX"].GetInt(), gafObj["posAtNodeY"].GetInt()));
        }
        m_pBgSprite->addChild(actor, gafObj["zorder"].GetInt());
        
    } /*for*/
    
    // 过关奖励 gaf
    rapidjson::Value &stageClearAwardGaf = doc["awardGaf"];
    
    if (!stageClearAwardGaf["standby"].IsNull())
    {
        rapidjson::Value &stageClearAwardStandbyGaf = stageClearAwardGaf["standby"];
        m_awordStandByActor = Actor::createWithGafFile(stageClearAwardStandbyGaf["gafFile"].GetString());
        m_awordStandByActor->setPosition(Vec2(stageClearAwardStandbyGaf["posx"].GetInt(),
                                              stageClearAwardStandbyGaf["posy"].GetInt()));
        m_awordStandByActor->setSize(Size(stageClearAwardStandbyGaf["width"].GetInt(),
                                          stageClearAwardStandbyGaf["height"].GetInt()));
        m_awordStandByActor->setPosAtNode(Vec2(stageClearAwardStandbyGaf["posAtNodeX"].GetInt(),
                                               stageClearAwardStandbyGaf["posAtNodeY"].GetInt()));
        m_pBgSprite->addChild(m_awordStandByActor, stageClearAwardStandbyGaf["zorder"].GetInt());
    }
    
    // 加载图像
    rapidjson::Value &imgs = doc["imgs"];
    for (unsigned int i = 0; i < imgs.Size(); i++)
    {
        rapidjson::Value &imgValue = imgs[i];
        auto imgSp = Sprite::create(imgValue["file"].GetString());
        imgSp->setPosition(Vec2(imgValue["posx"].GetInt(), imgValue["posy"].GetInt()));
        m_pBgSprite->addChild(imgSp, imgValue["zorder"].GetInt());
    } /*for*/
    
    ///////////////////
    // 关卡数字按钮
    ///////////////////
    rapidjson::Value &btnFrames = doc["btnFrames"];
    for (unsigned int i = 0; i < btnFrames.Size(); i++)
    {
        rapidjson::Value &btnFrame = btnFrames[i];
        std::map<std::string, std::string> btnImgsMap;
        btnImgsMap.insert(std::pair<std::string, std::string>("normal", btnFrame["normal"].GetString()));
        btnImgsMap.insert(std::pair<std::string, std::string>("disable", btnFrame["disable"].GetString()));
        btnImgsMap.insert(std::pair<std::string, std::string>("highLight", btnFrame["highLight"].GetString()));
        m_btnImgMap[btnFrame["shapeKey"].GetString()] = btnImgsMap;
    } /*for*/
    
    rapidjson::Value &btns = doc["btns"];
    RAPIDJSON_ASSERT(btns.Size() == 12);
    for (unsigned int i = 0; i < btns.Size(); i++)
    {
        rapidjson::Value &btn = btns[i];
        auto btnSp = Sprite::createWithSpriteFrameName(btn["frame"].GetString());
        btnSp->setPosition(Vec2(btn["posx"].GetInt(), btn["posy"].GetInt()));
        m_pSbn->addChild(btnSp, 2);
        
        BtnUnit bu;
        bu.shapeKey = btn["bgShapeKey"].GetString();
        bu.normalImgFrame = getBtnImgFrame(bu.shapeKey, "normal");
        bu.disableImgFrame = getBtnImgFrame(bu.shapeKey, "disable");
        bu.highLightImgFrame = getBtnImgFrame(bu.shapeKey, "highLight");
        bu.btnBgPos = Vec2(btnSp->getPosition().x + btn["bgOffsetX"].GetInt(),
                           btnSp->getPosition().y + btn["bgOffsetY"].GetInt());
        m_btnUnit[i] = bu;
        
    } /*for*/
    
    ////////
    // 关卡轨迹点
    rapidjson::Value &trackPoints = doc["pts"];
    for (unsigned int i = 0; i < trackPoints.Size(); i++)
    {
        rapidjson::Value &trackPoint = trackPoints[i];
        std::vector<Vec2> trackPointVec;
        for (unsigned int j = 0; j < trackPoint.Size(); j++)
        {
            rapidjson::Value &point = trackPoint[j];
            trackPointVec.push_back(Vec2(point["x"].GetInt(), point["y"].GetInt()));
        } /*for*/
        m_trackPoints.push_back(trackPointVec);
    } /*for*/
    
    //
    updateMenuButtonStage();
    
    //
    initHead();
}

/*
 *  获取关卡按钮图片帧
 */
std::string SelStageScene::getBtnImgFrame(const std::string& key, const std::string& btnState)
{
    return m_btnImgMap[key][btnState];
}

//
void SelStageScene::updateMenuButtonStage()
{
    for (long i = 0; i < 12; i++)
    {
        long stageIndex = m_themeIndex * 12 + i;
        std::string frameKey = "";
        if (m_pStageDataManager->stageIsClear((int)stageIndex)) /*已经通关*/
        {
            frameKey =  m_btnUnit[i].normalImgFrame;
        } else {
            frameKey =  m_btnUnit[i].disableImgFrame;
        }
        
        m_btnUnit[i].btnBgSp = Sprite::createWithSpriteFrameName(frameKey);
        m_btnUnit[i].btnBgSp->setUserData((void*)i);
        m_pSbn->addChild(m_btnUnit[i].btnBgSp, 1);
        m_btnUnit[i].btnBgSp->setPosition(m_btnUnit[i].btnBgPos);
    } /*for*/
}

//
void SelStageScene::initHead()
{
    float width = 320.0f, height = 320.0f;
    
    m_headNode = Node::create();
    m_headNode->setAnchorPoint(Vec2(0.5f, 0.5f));
    m_pBgSprite->addChild(m_headNode, HEAD_ZORDER);
    m_headNode->setContentSize(Size(width, height));
    auto headSprite = Sprite::createWithSpriteFrameName(AIMEE_FRAME);
    headSprite->setPosition(width / 2.0f, height + HEAD_UP_OFFSET);
    m_headNode->addChild(headSprite, 2);
    
    auto seq = Sequence::create(MoveBy::create(0.8f, Vec2(0.0f, -50.0f)),
                                MoveBy::create(0.8f, Vec2(0.0f, 50.0f)), NULL);
    headSprite->runAction(RepeatForever::create(seq));
    auto actor = Actor::createWithGafFile(LIGHT_GAF);
    actor->setSize(Size(320.0f, 320.0f));
    actor->setPosition(Vec2(0.0f, 50.0f));
    actor->setPosAtNode(Vec2(-150.0f, 450.0f));
    m_headNode->addChild(actor, 1);
    
    if (m_headIndex > 0)
    {
        SpriteFrame* pointFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(POINT_FRAME);
        for (int i = 0; i < m_headIndex; i++)
        {
            auto pts = m_trackPoints[i];
            for (const auto& it : pts)
            {
                auto spPoint = Sprite::createWithSpriteFrame(pointFrame);
                spPoint->setPosition(it);
                m_pSbn->addChild(spPoint);
            } /*for*/
        } /*for*/
    }
    
    // 更新头像位置
    auto headPos = m_btnUnit[m_headIndex].btnBgPos;
    m_headNode->setPosition(headPos);
    // 更新关卡按钮背景
    updateBtnBg(m_headIndex, 2);
    
    // 根据头像位置判断是否要滚动 scrollview
    float maxHeadx = headPos.x;
    if (m_needMoveHead) {
        auto headPos1 = m_btnUnit[m_headIndex + 1].btnBgPos;
        if (headPos1.x > maxHeadx) {
            maxHeadx = headPos1.x;
        }
    }
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    auto dx = dw - 240.0f;
    if (maxHeadx >= dx)
    {
        m_pScrollView->scrollToRight(1.0f, true);
    }
}

/*
 *  更新数字按钮背景
 *  stage 1:普通 2:高亮
 */
void SelStageScene::updateBtnBg(const unsigned int btnIndex, const unsigned int stage)
{
    if (m_btnUnit[btnIndex].btnBgSp != nullptr)
    {
        m_btnUnit[btnIndex].btnBgSp->removeFromParent();
        m_btnUnit[btnIndex].btnBgSp = nullptr;
    }
    std::string frameKey = "";
    if (stage == 1) /*普通*/
    {
        frameKey =  m_btnUnit[btnIndex].normalImgFrame;
    } else {
        frameKey =  m_btnUnit[btnIndex].highLightImgFrame;
    }
    m_btnUnit[btnIndex].btnBgSp = Sprite::createWithSpriteFrameName(frameKey);
    m_btnUnit[btnIndex].btnBgSp->setUserData((void*)btnIndex);
    m_pSbn->addChild(m_btnUnit[btnIndex].btnBgSp, 1);
    m_btnUnit[btnIndex].btnBgSp->setPosition(m_btnUnit[btnIndex].btnBgPos);
}

//
void SelStageScene::moveHeadSchedule()
{
    // 增加白点
    auto spPoint = Sprite::createWithSpriteFrameName(POINT_FRAME);
    spPoint->setPosition(*m_headMoveIter);
    m_pSbn->addChild(spPoint);
    
    if (++m_headMoveIter != m_headMove.end())
    {
        auto moveTo = MoveTo::create(HEAD_MOVE_DURATION, *m_headMoveIter);
        auto cb = CallFunc::create(std::bind(&SelStageScene::moveHeadSchedule, this));
        m_headNode->runAction(Sequence::create(moveTo, cb, NULL));
    } else {
        auto moveTo = MoveTo::create(HEAD_MOVE_DURATION, m_btnUnit[++m_headIndex].btnBgPos);
        auto cb = CallFunc::create(std::bind(&SelStageScene::moveHeadEnd, this));
        m_headNode->runAction(Sequence::create(moveTo, cb, NULL));
    }
}

//
void SelStageScene::moveHeadEnd()
{
    m_headIsMove = false;
    updateBtnBg(m_headIndex, 2);
    
    //如果连续玩了两关，播放动动操
    if (AppConfig::getInstance()->getPassTimes() == 2)
    {
        AppConfig::getInstance()->setPassTimes(0);    //重置为0
        playDongDongCao();
    } else {
        bindBtnsEvent();
    }
}

/*
 *  移动头像
 */
void SelStageScene::startMoveHead()
{
    if (m_headIndex < 11)
    {
        m_headMove = m_trackPoints[m_headIndex];
        m_headMoveIter = m_headMove.begin();
        m_headPos = m_headNode->getPosition();
        updateBtnBg(m_headIndex, 1);
        auto moveTo = MoveTo::create(HEAD_MOVE_DURATION, *m_headMoveIter);
        auto cb = CallFunc::create(std::bind(&SelStageScene::moveHeadSchedule, this));
        m_headIsMove = true;
        m_headNode->runAction(Sequence::create(moveTo, cb, NULL));
    }
}

/*播放动动操*/
void SelStageScene::playDongDongCao()
{
//    auto size = Director::getInstance()->getVisibleSize();
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//    m_pVideoPlay = cocos2d::experimental::ui::VideoPlayer::create();
//    m_pVideoPlay->setPosition(Vec2(size.width/2, size.height/2));
//    m_pVideoPlay->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
//    auto width = AppConfig::getInstance()->getDesignResolutionWidth();
//    m_pVideoPlay->setContentSize(Size(width, size.height));
//    addChild(m_pVideoPlay, DONGDONGCAO_ZORDER);
//    if (m_pVideoPlay) {
//        m_pVideoPlay->setFileName(DONGDONGCAO_MP4_FILE);
//        m_pVideoPlay->play();
//        SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
//        
//        
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//        /////////////////
//        // 按钮事件绑定
//        auto dispatcher = Director::getInstance()->getEventDispatcher();
//        auto actorEvent = EventListenerTouchOneByOne::create();
//        actorEvent->setSwallowTouches(true);
//        actorEvent->onTouchBegan = [this](Touch* pTouch, Event* pEvent) ->bool
//        {
//            auto actor = static_cast<Sprite*>(pEvent->getCurrentTarget());
//            auto touchPoint = actor->convertToNodeSpace(pTouch->getLocation());
//            Rect touchRect = Rect(0.0f, 0.0f, actor->getContentSize().width, actor->getContentSize().height);
//            if (touchRect.containsPoint(touchPoint))
//            {
//                return true;
//            }
//            return false;
//        };
//        actorEvent->onTouchEnded = [this](Touch* pTouch, Event* pEvent) {};
//        dispatcher->addEventListenerWithSceneGraphPriority(actorEvent, m_pVideoPlay);
//        dispatcher->setPriority(actorEvent, -130);
//#endif
//        
//        m_pVideoPlay->addEventListener([this](Ref* ref, cocos2d::experimental::ui::VideoPlayer::EventType type){
//            if (type == cocos2d::experimental::ui::VideoPlayer::EventType::COMPLETED) {
//                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pVideoPlay);
//                m_pVideoPlay->removeFromParent();
//                m_pVideoPlay = nullptr;
//                SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
//            }
//        });
//    }
//#endif
    auto size = Director::getInstance()->getVisibleSize();
    m_pDongdongCaoActor = Actor::createWithGafFile(DONGDONGCAO_GAF_FILE, false);
    m_pDongdongCaoActor->setAutoReleaseAsset(true);
    m_pDongdongCaoActor->setPosition(Vec2(m_sceneOffset, 0.0f));
    addChild(m_pDongdongCaoActor, DONGDONGCAO_ZORDER);
    m_pDongdongCaoActor->setPosAtNode(Vec2(0, size.height));
    m_pDongdongCaoActor->setAnimationFinishedPlayDelegate([this]
    {
        Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_pDongdongCaoActor);
        this->scheduleOnce(CC_SCHEDULE_SELECTOR(SelStageScene::onDongdongcalCompleted), 0.3f);
    });
    MyAudioEngine::getInstance()->playAudio(DONGDONGCAO_AUDIO_FILE,false,true);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_pDongdongCaoActor);
    dispatcher->setPriority(listener, -130);
}

//
void SelStageScene::onDongdongcalCompleted(float dt)
{
    m_pDongdongCaoActor->removeFromParent();
    m_pDongdongCaoActor = nullptr;
    bindBtnsEvent();
    
    MyAudioEngine::getInstance()->playAudio(m_bgMusic.c_str(),true,true,m_bgMusicVolume);
}

/*
 *
 */
void SelStageScene::bindBtnsEvent()
{
    for (unsigned int i = 0; i < 12; i++)
    {
        bindBtnEvent(i);
    } /*for*/
}

/*
 *  绑定按钮事件
 */
void SelStageScene::bindBtnEvent(const unsigned int index)
{
    /////////////////
    // 按钮事件绑定
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto eventByBtn = EventListenerTouchOneByOne::create();
    eventByBtn->onTouchBegan = [this](Touch* pTouch, Event* pEvent) ->bool
    {
        auto spNumber = static_cast<Sprite*>(pEvent->getCurrentTarget());
        auto touchPoint = spNumber->convertToNodeSpace(pTouch->getLocation());
        Rect touchRect = Rect(0.0f, 0.0f, spNumber->getContentSize().width+30, spNumber->getContentSize().height+30);
        if (touchRect.containsPoint(touchPoint))
        {
            spNumber->setScale(1.1f);
            m_originalPoint = spNumber->getPosition();
            m_touchBeganPoint = pTouch->getLocation();
            return true;
        }
        if (m_headIsMove) {
            return false;
        }
        return false;
    };
    eventByBtn->onTouchEnded = [this, dispatcher](Touch* pTouch, Event* pEvent)
    {
        auto spNumber = static_cast<Sprite*>(pEvent->getCurrentTarget());
        spNumber->setScale(1.0f);
        MyAudioEngine::getInstance()->playAudio(CLICK_MUSIC_FILE);
        bool canEnterStage = false;
        auto touchPoint = spNumber->convertToNodeSpace(pTouch->getLocation());
        Rect touchRect = Rect(0.0f, 0.0f, spNumber->getContentSize().width+30, spNumber->getContentSize().height+30);
        
        Vec2 touchEndPoint = pTouch->getLocation();
        Vec2 m_offSetPos;
        
        m_offSetPos.x = touchEndPoint.x - m_touchBeganPoint.x;
        m_offSetPos.y = touchEndPoint.y - m_touchBeganPoint.y;
        
        if (fabsf(m_offSetPos.y) < TOUCHOFFSETX)
        {
            if (touchRect.containsPoint(touchPoint))
            {
                // 获取选择的关卡号
                int selStageIndex = static_cast<int>((long)spNumber->getUserData() + m_themeIndex * 12);
                
                if (AppConfig::getInstance()->isDebugMode()) {
                    canEnterStage = true;
                } else {
                    if (m_pStageDataManager->stageIsClear(selStageIndex)) /*已经通关*/
                    {
                        canEnterStage = true;
                    } else {
                        auto maxStageClearIndex = m_pStageDataManager->getMaxClearStageIndex(m_themeIndex);
                        if ( std::abs(selStageIndex % 12 - maxStageClearIndex) == 1)
                        {
                            canEnterStage = true;
                        }
                    }
                }
                if (canEnterStage && !m_isPopAward)
                {
                    dispatcher->removeEventListenersForTarget(spNumber);
                    Director::getInstance()->replaceScene(LoadingStageScene::createWithStageIndex((unsigned int)selStageIndex, m_themeIndex));
                    // 为npc界面设置当前默认主题
                    UserDefault::getInstance()->setIntegerForKey("themeIndex", UserDefault::getInstance()->getIntegerForKey("curThemeIndex"));
                }
            }
        }
    };
    dispatcher->addEventListenerWithSceneGraphPriority(eventByBtn, m_btnUnit[index].btnBgSp);
}

/*
 *  弹出通关奖励
 */
void SelStageScene::popStageClearAward()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    m_awordTriggerActor = Actor::createWithGafConfigFile(m_stageClearAwardTriggerGafConfig);
    
    m_LayerColorAward = LayerColor::create(Color4B(0, 0, 0, 155), visibleSize.width, visibleSize.height);
    addChild(m_LayerColorAward, 102);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, m_LayerColorAward);
    dispatcher->setPriority(listener, -130);
    
    float cx = visibleSize.width / 2.0f - m_awordTriggerActor->getSize().width / 2.0f;
    float cy = visibleSize.height / 2.0f - m_awordTriggerActor->getSize().height / 2.0f;
    m_awordTriggerActor->setPosition(Vec2(cx, cy));
    addChild(m_awordTriggerActor, 200);
    m_isPopAward = true;
    m_awordTriggerActor->playWithSequenceAliasName("trigger", false);
    auto aniCompletedCB = [this]
    {
        m_awordTriggerActor->playWithSequenceAliasName("standby", true);
    };
    m_awordTriggerActor->setAnimationFinishedPlayDelegate(aniCompletedCB);
    scheduleOnce([this] (float dt)
    {
        m_isPopAward = false;
        if (m_LayerColorAward != nullptr) {
            m_LayerColorAward->removeFromParent();
            m_LayerColorAward = nullptr;
        }
        
        m_awordTriggerActor->stop();
        m_awordTriggerActor->removeFromParent();
        m_awordTriggerActor = nullptr;
        bindBtnsEvent();
     }, 3.0f, "collect");
}

/* 
 *  显示所有轨迹点，调试用
 */
void SelStageScene::showAllTrackPoint()
{
    SpriteFrame* pointFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(POINT_FRAME);
    for (int i = 0; i < 11; i++)
    {
        auto pts = m_trackPoints[i];
        for (const auto& it : pts)
        {
            auto spPoint = Sprite::createWithSpriteFrame(pointFrame);
            spPoint->setPosition(it);
            m_pSbn->addChild(spPoint,5);
        } /*for*/
    } /*for*/
}

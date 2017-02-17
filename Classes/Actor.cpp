//
//  Actor.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/6.
//
//  角色实现文件
//

#include "Actor.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "GafAssetCache.h"
#include "GAFPlayer/GAF.h"
#include "AppConfig.h"
using namespace gaf;

#define GAF_OBJ_ZORDER      10

/*
 *  constuctor
 */
Actor::Actor()
{
    setPosition(Vec2::ZERO);
}

/*
 *  destuctor
 */
Actor::~Actor()
{
    if (m_autoreleaseAsset) {
        GafAssetCache::getInstance()->removeGafAssetForKey(m_gafFile);
    }
}

/* 
 * 根据 gaf 文件生成角色
 */
Actor* Actor::createWithGafFile(const std::string& gafFile, const bool looped,
                                const float scaleFactor, const Vec2& posAtNode)
{
    Actor* pRet = new Actor();
    if (pRet && pRet->initWithGafFile(gafFile, looped, scaleFactor, posAtNode)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

/*
 * 根据 gaf 文件初始化
 */
bool Actor::initWithGafFile(const std::string& gafFile, const bool looped,
                            const float scaleFactor, const Vec2& posAtNode)
{
    // 加载 gafAsset 文件
    GAFAsset* pGafAssetObj = GafAssetCache::getInstance()->getGafAssetForKey(gafFile);
    if (pGafAssetObj != nullptr) {
        m_pGafAsset = pGafAssetObj;
    } else {
        m_pGafAsset = GafAssetCache::getInstance()->addGafAsset(gafFile);
    }
    
    m_gafFile = gafFile;
    m_looped = looped;
    m_aliasForSequence.clear();
    m_scaleFactor = scaleFactor;
    m_posAtNode = posAtNode;
    
    _generateGafObject();
    
    return true;
}

/*
 * 根据 gaf 配置文件生成角色对象
 *
 * 参数：
 *      gafConfigFile   gaf 配置文件
 */
Actor* Actor::createWithGafConfigFile(const std::string& gafConfigFile)
{
    Actor* pRet = new Actor();
    if (pRet && pRet->initWithGafConfigFile(gafConfigFile))
    {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

/*
 * 根据 gaf 配置文件初始化
 */
bool Actor::initWithGafConfigFile(const std::string& gafConfigFile)
{
    std::string file_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(gafConfigFile).c_str();
    std::string contentStr = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseDefaultFlags>(contentStr.c_str());
    if (doc.HasParseError()) /*解析错误*/ {
        cocos2d::log("GetParseError %s\n", doc.GetParseError());
        RAPIDJSON_ASSERT(!doc.HasParseError());
    }
    
    if (!doc["key"].IsNull()) /*存在key值*/  {
        m_key = doc["key"].GetString();
    }
    
    m_gafFile = doc["gafFile"].GetString();
    rapidjson::Value &sequenceAliasJsonValue = doc["sequenceAlias"];
    if (sequenceAliasJsonValue.IsArray() ) /*存在动作别名列表*/ {
        m_aliasForSequence.clear();
        for (unsigned int i = 0; i < sequenceAliasJsonValue.Size(); i++) {
            const rapidjson::Value& sajv = sequenceAliasJsonValue[i];
            std::string alias = sajv["alias"].GetString();
            std::string sequence = sajv["sequence"].GetString();
            m_aliasForSequence.insert(std::make_pair(alias, sequence));
        } /*for*/
    }
    m_scaleFactor = (float)doc["scaleFactor"].GetDouble();
    m_posAtNode = Vec2((float)doc["posX"].GetInt(), (float)doc["posY"].GetInt());

    if (!doc["size"].IsNull())
    {
        const rapidjson::Value &areaSizeValue = doc["size"];
        m_Size = Size(areaSizeValue["w"].GetInt(), areaSizeValue["h"].GetInt());
        setContentSize(m_Size);
    }
    
    // 加载 gafAsset 文件
    GAFAsset* pGafAssetObj = GafAssetCache::getInstance()->getGafAssetForKey(m_gafFile);
    if (pGafAssetObj != nullptr)
    {
        m_pGafAsset = pGafAssetObj;
    } else {
        m_pGafAsset = GafAssetCache::getInstance()->addGafAsset(m_gafFile);
    }
    
    _generateGafObject();
    
    return true;
}

/*
 * 根据另外一个 Actor 对象生成角色
 */
Actor* Actor::createWithActor(Actor* pOtherActor, const bool looped)
{
    CCASSERT(pOtherActor, "invalid gafAsset Object");
    Actor* pRet = new Actor();
    if (pRet && pRet->initWithActor(pOtherActor, looped)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}

/*
 * 根据另外一个 Actor 对象初始化角色
 */
bool Actor::initWithActor(Actor* pOtherActor, const bool looped)
{
    // 共享
    m_pGafAsset = pOtherActor->getGAFAsset();
    
    const std::map<std::string, std::string> &aliasForSequence = pOtherActor->getAliasForSequence();
    if (aliasForSequence.size() > 0) /*存在动作别名*/ {
        /* 复制动作别名列表 */
        m_aliasForSequence.clear();
        for (auto &it : aliasForSequence)
        {
            m_aliasForSequence.insert(std::make_pair(it.first, it.second));
        } /*for*/
    }
    
    m_gafFile = pOtherActor->getGafFile();
    m_looped = looped;
    m_scaleFactor = pOtherActor->getScaleFactor();
    m_posAtNode = pOtherActor->getPosAtNode();
    
    _generateGafObject();
    return true;
}

//
void Actor::_generateGafObject()
{
    if (m_pGafAsset != nullptr)
    {
        if (m_aliasForSequence.size() > 0) {
            m_pGafObj = m_pGafAsset->createObject();
        } else /*没有动作别名列表，直接播放默认动画*/ {
            m_pGafObj = m_pGafAsset->createObjectAndRun(m_looped);
        }
        m_pGafObj->setLocalZOrder(0);
        m_pGafObj->setAnchorPoint(Vec2::ZERO);
        addChild(m_pGafObj, GAF_OBJ_ZORDER);
        
        m_pGafObj->setScale(m_scaleFactor);
        m_pGafObj->setPosition(m_posAtNode);
    }
    //
    drawNode();
}

/**/
void Actor::drawNode()
{
    if (AppConfig::getInstance()->gafDebugDraw() && m_Size.width > 0.0f && m_Size.height > 0.0f)
    {
        if (m_pDebugDrawNode != nullptr) {
            m_pDebugDrawNode->removeFromParent();
            m_pDebugDrawNode = nullptr;
        }
        m_pDebugDrawNode = DrawNode::create();
        addChild(m_pDebugDrawNode, 10);
        
        Vec2 point[4];
        point[0] = Vec2(0.0f, 0.0f);
        point[1] = Vec2(0.0f, m_Size.height);
        point[2] = Vec2(m_Size.width, m_Size.height);
        point[3] = Vec2(m_Size.width, 0.0f);
        m_pDebugDrawNode->drawPolygon(point, 4, Color4F(1, 0, 0, 0), 1, Color4F(0, 1, 0, 1));
    }
}

/*
 *  根据动作序列别名播放相应的序列动画
 */
void Actor::playWithSequenceAliasName(const std::string& sequenceAliasName, const bool looped)
{
    std::map<std::string, std::string>::iterator iter = m_aliasForSequence.find(sequenceAliasName);
    if (m_pGafObj && iter != m_aliasForSequence.end()) {
        m_pGafObj->playSequence(iter->second, looped);
        m_pGafObj->start();
    }
}

/* 根据动作序列组别名按顺序播放相应的序列组动画,此情况，每个动作都是不循环的（只播一次） */
void Actor::playWithSequenceAliasName(const std::vector<std::string>& strsSequence, const bool lastLooped, const unsigned int loopNum, const bool isRepeatForever, CALLBACKFUNCTION callBackFunction)
{
    m_curIndex = 0;
    m_curRepeatNum = 0;
    //播放第一个序列动作组第一个动作
    auto iter = m_aliasForSequence.find(strsSequence[m_curIndex]);
    if (m_pGafObj && iter != m_aliasForSequence.end()) {
        m_pGafObj->playSequence(iter->second, false);
        m_pGafObj->start();
    }
    
    auto function = [lastLooped,this,strsSequence] {
        if (m_curIndex < strsSequence.size()) {
            auto iter = m_aliasForSequence.find(strsSequence[m_curIndex]);
            if (m_pGafObj && iter != m_aliasForSequence.end()) {
                if (m_curIndex == strsSequence.size() - 1) {
                    m_pGafObj->playSequence(iter->second, lastLooped);
                } else
                {
                    m_pGafObj->playSequence(iter->second, false);
                }
                m_pGafObj->start();
            }
        }
    };
    this->schedule([this,isRepeatForever,function,strsSequence,loopNum,callBackFunction](float dt){
        if (m_pGafObj->getIsCurrentFrameLastInSequence())
        {
            m_curIndex++;
            if (m_curIndex == strsSequence.size()) {
                if (isRepeatForever) {
                    m_curIndex = 0;
                    function();
                } else
                {
                    m_curRepeatNum++;
                    if (m_curRepeatNum == loopNum) {
                        unschedule("sequence");
                        if (callBackFunction != nullptr) {
                            callBackFunction();     //播完动作回调
                        }
                    } else
                    {
                        m_curIndex = 0;
                        function();
                    }
                }
                return;
            }
            function();
        }
    },1.0f/m_pGafObj->getFps(), "sequence");
}

/*
 *  开始播放动画
 */
void Actor::start()
{
    if (m_pGafObj) {
        m_pGafObj->start();
    }
}

/*
 *  停止播放动画
 */
void Actor::stop()
{
    if (m_pGafObj) {
        m_pGafObj->stop();
    }
}

/*
 *  设置缩放因子 
 */
void Actor::setScaleFactor(const float scaleFactor)
{
    m_scaleFactor = scaleFactor;
    if (m_pGafObj) {
        m_pGafObj->setScale(scaleFactor);
    }
}

/*
 *  设置 gaf 动画在容器节点中的位置
 */
void Actor::setPosAtNode(const Vec2& posAtNode)
{
    m_posAtNode = posAtNode;
    if (m_pGafObj) {
        m_pGafObj->setPosition(posAtNode);
    }
}

//
const Size Actor::getActorContentSize() const
{
    cocos2d::Size size(0.0f, 0.0f);
    if (m_pGafObj) {
        size = m_pGafObj->getContentSize();
    }
    return size;
}

/* 
 *  是否动画播放中
 */
bool Actor::IsAnimationRunning()
{
    return m_pGafObj && m_pGafObj->getIsAnimationRunning();
}

void Actor::setGafObjVisible(const bool visible) {
    if (m_pGafObj) {
        m_pGafObj->setVisible(visible);
    }
}

//
void Actor::setSize(const cocos2d::Size& size)
{
    setContentSize(m_Size = size);
    drawNode();
}

//
int Actor::getGafObjZorder()
{
    return GAF_OBJ_ZORDER;
}

//
void Actor::setAnimationFinishedPlayDelegate(const std::function<void()>& animationFinishedPlayCB)
{
    m_AnimationFinishedPlayCB = animationFinishedPlayCB;
    if (m_AnimationFinishedPlayCB != nullptr)
    {
        m_pGafObj->setAnimationFinishedPlayDelegate(std::bind(&Actor::onAnimationFinished, this,
                                                               std::placeholders::_1));
    } else {
        m_AnimationFinishedPlayCB = nullptr;
        m_pGafObj->setAnimationFinishedPlayDelegate(nullptr);
    }
}

//
void Actor::onAnimationFinished(gaf::GAFObject* pGafObj)
{
    if (m_AnimationFinishedPlayCB != nullptr) {
        pGafObj->setAnimationFinishedPlayDelegate(nullptr);
        m_AnimationFinishedPlayCB();
        m_AnimationFinishedPlayCB = nullptr;
    }
}

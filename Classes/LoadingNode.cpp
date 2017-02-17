//
//  LoadingNode.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/11.
//
//

#include "LoadingNode.h"
#include "Actor.h"
#include "GafAssetCache.h"
#include "AppConfig.h"

#define FONT_FILE           "fonts/DFGBHBC_0.TTC"
#define LOADING_GAF_FILE    "res/common/loding02/loding02.gaf"

//
LoadingNode::LoadingNode() {}

//
LoadingNode::~LoadingNode()
{
    GafAssetCache::getInstance()->removeGafAssetForKey(LOADING_GAF_FILE);
}

//
LoadingNode* LoadingNode::create()
{
    auto pLoadingNode = new LoadingNode();
    if (pLoadingNode && pLoadingNode->init()) {
        pLoadingNode->autorelease();
    } else {
        CC_SAFE_DELETE(pLoadingNode);
    }
    return pLoadingNode;
}

//
bool LoadingNode::init()
{
    auto size = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    LayerColor* layerColor = LayerColor::create(Color4B(19, 165, 212, 255), dw, size.height);
    addChild(layerColor);
    
    Label* label = Label::createWithTTF("加载中，请稍等。。。", FONT_FILE, 48);
    label->setPosition(Vec2(dw / 2.0f, size.height / 2.0f));
    this->addChild(label);
    
    Actor* loadingActor = Actor::createWithGafFile(LOADING_GAF_FILE);
    loadingActor->setPosition(Vec2(dw / 2.0f - 580.0f, size.height / 2.0f + 220.0f));
    this->addChild(loadingActor);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    dispatcher->setPriority(listener, -130);
    
    return true;
}

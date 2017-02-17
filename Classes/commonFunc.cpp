//
//  commonFunc.cpp
//  superBaby
//
//  Created by adinnet on 15/6/11.
//
//

#include "commonFunc.h"
#include "AppConfig.h"

#define BG_LEFT_IMAGE               "res/common/bgLeft.png"
#define BG_RIGHT_IMAGE              "res/common/bgRight.png"

void addLrMask(Node* pParentNode)
{
    CCASSERT(pParentNode != nullptr, "invalid node");
    
    auto sceneOffset = AppConfig::getInstance()->getSceneOffsetX();
    // 左右遮罩
    if (sceneOffset > 0.0f)
    {
        auto dWidth = AppConfig::getInstance()->getDesignResolutionWidth();
        auto visibleSize = Director::getInstance()->getVisibleSize();
        
        auto bgLeftSp = Sprite::create(BG_LEFT_IMAGE);
        bgLeftSp->setAnchorPoint(Vec2(1.0f, 0.0f));
        bgLeftSp->setPosition(Vec2(sceneOffset, 0.0f));
        bgLeftSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgLeftSp->setScaleX(sceneOffset / bgLeftSp->getContentSize().width);
        pParentNode->addChild(bgLeftSp, 1000);
        
        auto bgRightSp = Sprite::create(BG_RIGHT_IMAGE);
        bgRightSp->setAnchorPoint(Vec2(0.0f, 0.0f));
        bgRightSp->setPosition(Vec2(sceneOffset + dWidth, 0.0f));
        bgRightSp->setScaleY(visibleSize.height / bgLeftSp->getContentSize().height);
        bgRightSp->setScaleX(sceneOffset / bgLeftSp->getContentSize().width);
        pParentNode->addChild(bgRightSp, 1000);
    }
}

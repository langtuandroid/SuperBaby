//
//  NpcIntroduceNode.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/23.
//
//

#include "NpcIntroduceNode.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "NpcActor.h"
#include "NpcBagNode.h"
//#include "adjustPosNode.h"
#include "GafAssetCache.h"
#include "MyAudioEngine.h"
#include "AppConfig.h"

#define YELLOWLIGHT_PNG_FILE   "res/common/chongwuguang.png"
#define OPEN_MUSIC_FILE        "res/audio/npcbag/NPC-tanchu.mp3"
#define FONT_FILE              "fonts/DFGBHBC_0.TTC"

NpcIntroduceNode::NpcIntroduceNode()
{
    
}

NpcIntroduceNode::~NpcIntroduceNode()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(YELLOWLIGHT_PNG_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(m_npcPath);
}

NpcIntroduceNode* NpcIntroduceNode::createWithNpcKey(std::string key)
{
    NpcIntroduceNode* ret = new NpcIntroduceNode();
    if (ret&&ret->initWithNpcKey(key)) {
        ret->autorelease();
    }else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

bool NpcIntroduceNode::initWithNpcKey(std::string key)
{
    
    char npcBuffer[64];
    sprintf(npcBuffer, "res/npc/%s-D+C/%s-D+C.gaf",key.c_str(),key.c_str());
    m_npcPath = npcBuffer;
    auto size = Director::getInstance()->getVisibleSize();
    auto dw = AppConfig::getInstance()->getDesignResolutionWidth();
    auto colorLayer = LayerColor::create(Color4B(0, 0, 0, 150), size.width, size.height);
    this->addChild(colorLayer);
    
    Sprite* yellowLight = Sprite::create(YELLOWLIGHT_PNG_FILE);
    yellowLight->setPosition(Vec2(dw/2, size.height/2));
    this->addChild(yellowLight);
    yellowLight->runAction(RepeatForever::create(RotateBy::create(2.0f, 90)));
    yellowLight->setVisible(false);
    
    m_pBackImage = ImageView::create("beibaotankuan.png",ui::Widget::TextureResType::PLIST);
    m_pBackImage->setPosition(Vec2(dw/2, size.height/2));
    this->addChild(m_pBackImage);
    m_pBackImage->setScale(0.0f);
    
    auto imageSize = m_pBackImage->getContentSize();
    
    NpcData* npcData = NpcDataManager::getInstance()->getNpcDataByKey(key);
    m_npcStr = npcData->getGafConfigFile();
    
    NpcActor* npcActor = NpcActor::createWithGafConfigFile(m_npcStr);
    npcActor->trigger(false);
    auto actorSize = npcActor->getSize();
    npcActor->setPosition(Vec2((imageSize.width-actorSize.width)/2-30, (imageSize.height-actorSize.height)/2-50));
    m_pBackImage->addChild(npcActor);
    npcActor->setScale(1.2f);
    npcActor->setAnimationFinishedPlayDelegate([this,npcActor,yellowLight,colorLayer]
    {
        MyAudioEngine::getInstance()->stopAudio(m_effectId);
        yellowLight->runAction(Spawn::create(ScaleTo::create(0.5f, 0.0f),FadeOut::create(0.3f), NULL));
        colorLayer->runAction(FadeOut::create(0.5f));
        m_pBackImage->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
            this->removeFromParent();
            Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
            m_pNpcBagNode->resumeTouch();
        }), NULL));
    });
    
//    auto anode = adjustPosNode::createWithNode(npcActor);
//    addChild(anode, 100);
    
    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = FONT_FILE;
    ttfConfig.fontSize = 42;
    ttfConfig.distanceFieldEnabled = false;
    ttfConfig.outlineSize = 0;
    ttfConfig.glyphs = GlyphCollection::DYNAMIC;
    ttfConfig.customGlyphs = nullptr;
    
    std::string npcName = npcData->getNpcName();
    unsigned int npcNum = npcData->getCollectedTimes();
    char npcNumBuffer[32];
    if (npcNum > 99) {
        sprintf(npcNumBuffer, "%d+",npcNum);
    } else
    {
        sprintf(npcNumBuffer, "%d",npcNum);
    }

    
    char infoBuffer[64];
    sprintf(infoBuffer, "%s X %s ",npcName.c_str(),npcNumBuffer);
    // 使用TTFConfig配置，来创建TTF
    auto nameLabel = Label::createWithTTF(ttfConfig,
                                         infoBuffer,
                                         TextHAlignment::LEFT,
                                         300);
    nameLabel->setPosition(Vec2(480, 200));
    nameLabel->setColor(Color3B(130, 191, 100));
    m_pBackImage->addChild(nameLabel);
    
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool
    {
        return true;
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    dispatcher->setPriority(listener, -130);
    
    auto closeBtn = ui::Button::create("ZTXZ-guanbi01.png","ZTXZ-guanbi02.png","ZTXZ-guanbi01.png",ui::Widget::TextureResType::PLIST);
    closeBtn->setPosition(Vec2(imageSize.width/2+399, imageSize.height/2+395));
    m_pBackImage->addChild(closeBtn);
    
    closeBtn->addTouchEventListener([this,yellowLight,colorLayer,closeBtn](Ref* ref, ui::Widget::TouchEventType type){
        if (ui::Widget::TouchEventType::ENDED == type) {
            closeBtn->setEnabled(false);
            MyAudioEngine::getInstance()->stopAudio(m_effectId);
            yellowLight->runAction(Spawn::create(ScaleTo::create(0.5f, 0.0f),FadeOut::create(0.3f), NULL));
            colorLayer->runAction(FadeOut::create(0.5f));
            m_pBackImage->runAction(Sequence::create(Spawn::create(EaseBackIn::create(ScaleTo::create(0.3f, 0.0f)),FadeOut::create(0.5f), NULL),CallFunc::create([this]{
                this->removeFromParent();
                Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
                m_pNpcBagNode->resumeTouch();
            }), NULL));
        }
    });
    
    //播放打开弹框音效
    MyAudioEngine::getInstance()->playAudio(OPEN_MUSIC_FILE);
    char yxBuffer[64];
    sprintf(yxBuffer, "res/audio/npc/%s.mp3",key.c_str());
    m_effectId = MyAudioEngine::getInstance()->playAudio(yxBuffer);
    
    m_pBackImage->runAction(Spawn::create(EaseBackOut::create(ScaleTo::create(0.3f, 1.0f)),Sequence::create(DelayTime::create(0.15f),CallFunc::create([this,yellowLight]{
        yellowLight->setVisible(true);
    }), NULL), NULL));

    
    return true;
}





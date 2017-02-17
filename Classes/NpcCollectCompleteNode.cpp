//
//  NpcCollectCompleteNode.cpp
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/23.
//
//

#include "NpcCollectCompleteNode.h"
#include "NpcDataManager.h"
#include "NpcData.h"
#include "NpcActor.h"
#include "CustomMessage.h"
#include "GafAssetCache.h"
#include "MyAudioEngine.h"

#define BACKSP_PNG_FILE        "res/common/NPC-diban.png"
#define YELLOWLIGHT_PNG_FILE   "res/common/chongwuguang.png"
#define COLLECT_MUSIC_FILE     "res/audio/npccollect/NPCshouji.mp3"
#define COLLECT_SUCCEED_MUSIC_FILE  "res/audio/npccollect/AM-SJ-XY2.mp3"
#define FONT_FILE              "fonts/DFGBHBC_0.TTC"

NpcCollectCompleteNode::NpcCollectCompleteNode()
:m_opacityNumber(0)
{
    
}

NpcCollectCompleteNode::~NpcCollectCompleteNode()
{
    Director::getInstance()->getTextureCache()->removeTextureForKey(YELLOWLIGHT_PNG_FILE);
    Director::getInstance()->getTextureCache()->removeTextureForKey(BACKSP_PNG_FILE);
    GafAssetCache::getInstance()->removeGafAssetForKey(m_npcPath);
}

NpcCollectCompleteNode* NpcCollectCompleteNode::createWithNpcKey(std::string key)
{
    NpcCollectCompleteNode* ret = new NpcCollectCompleteNode();
    if (ret&&ret->initWithNpcKey(key)) {
        ret->autorelease();        
    }else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

bool NpcCollectCompleteNode::initWithNpcKey(std::string key)
{
    char npcBuffer[64];
    sprintf(npcBuffer, "res/npc/%s-D+C/%s-D+C.gaf",key.c_str(),key.c_str());
    m_npcPath = npcBuffer;
    auto backSp = Sprite::create(BACKSP_PNG_FILE);
    this->addChild(backSp);
    
//    auto nameLabel = Label::createWithBMFont(, );
    
    auto yellowLight = Sprite::create(YELLOWLIGHT_PNG_FILE);
    this->addChild(yellowLight);
    yellowLight->runAction(RepeatForever::create(RotateBy::create(2.0f, 90)));
    
    NpcData* npcData = NpcDataManager::getInstance()->getNpcDataByKey(key);
    m_npcStr = npcData->getGafConfigFile();
    
    TTFConfig ttfConfig;
    ttfConfig.fontFilePath = FONT_FILE;
    ttfConfig.fontSize = 42;
    ttfConfig.distanceFieldEnabled = false;
    ttfConfig.outlineSize = 0;
    ttfConfig.glyphs = GlyphCollection::DYNAMIC;
    ttfConfig.customGlyphs = nullptr;
    
    std::string npcName = npcData->getNpcName();
    char infoBuffer[64];
    sprintf(infoBuffer, "%s",npcName.c_str());
    // 使用TTFConfig配置，来创建TTF
    auto nameLabel = Label::createWithTTF(ttfConfig,
                                          infoBuffer,
                                          TextHAlignment::LEFT,
                                          300);
    nameLabel->setPosition(Vec2(455, 160));
    nameLabel->setColor(Color3B(130, 191, 0));
    backSp->addChild(nameLabel);
    
    auto collectActor = NpcActor::createWithGafConfigFile(m_npcStr);
    auto actorSize = collectActor->getSize();
    collectActor->setPosition(Vec2(-actorSize.width/2, -actorSize.height/2-70));
    collectActor->trigger();
    this->addChild(collectActor);
    scheduleOnce([this](float dt){
        //弹框音效
        MyAudioEngine::getInstance()->playAudio(COLLECT_MUSIC_FILE);
        MyAudioEngine::getInstance()->playAudio(COLLECT_SUCCEED_MUSIC_FILE);
        this->runAction(Sequence::create(EaseBackOut::create(ScaleTo::create(0.5f, 1.0f)),CallFunc::create([this]{
            //播放相关介绍音效
            scheduleOnce([this](float dt){
                this->runAction(Sequence::create(ScaleTo::create(0.5f, 0.0f),CallFunc::create([this]{
                    this->removeFromParent();
                    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(NPC_COLLECTED_PROMPT_END);      //发通知离开回合
                }), NULL));
            },5,"end");
        }), NULL));
    }, 0, "action");
    
    return true;
}




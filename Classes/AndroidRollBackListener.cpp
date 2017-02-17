//
//  AndroidRollBackListener.cpp
//  superBaby
//
//  Created by Administrator on 15/7/16.
//
//

#include "AndroidRollBackListener.h"
#include "NpcBagNode.h"
#include "SelStageScene.h"
#include "LoadingScene.h"
#include "Player.h"
#include "StageDataManager.h"
#include "StageData.h"
#include "WelcomeScene.h"
#include "cocos2d.h"
#include "CustomMessage.h"
USING_NS_CC;

AndroidRollBackListener::AndroidRollBackListener()
{
}

AndroidRollBackListener::~AndroidRollBackListener()
{
    Director::getInstance()->getEventDispatcher()->removeEventListenersForType(EventListener::Type::KEYBOARD);
}

AndroidRollBackListener* AndroidRollBackListener::create(const unsigned int view)
{
    AndroidRollBackListener* ret = new (std::nothrow) AndroidRollBackListener();
    if (ret) {
        ret->init(view);
    }
    return ret;
}

bool AndroidRollBackListener::init(const unsigned int view)
{
    m_view = view;
    return true;
}

void AndroidRollBackListener::backScene()
{
    auto listen = EventListenerKeyboard::create();
    listen->onKeyReleased = [this, listen](EventKeyboard::KeyCode key, Event* event){
        if(key == EventKeyboard::KeyCode::KEY_BACK){
            auto director = Director::getInstance();
            
            if (VIEWS::WELCOME == m_view) {
                if (!m_isNpcBagNode) {
                    director->end();
                }else
                {
                    if (m_pNpcBagNode != nullptr) {
                        m_pNpcBagNode->removeFromParent();
                        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(NPCBAG_END_CALLBACK);
                    }
                }
            } else if (VIEWS::THEMESEL == m_view)
            {
                director->replaceScene(LoadingScene::createWithIndex(1));
            } else if (VIEWS::SELSTAGE == m_view)
            {
                if (!m_isNpcBagNode) {
                    director->replaceScene(LoadingScene::createWithIndex(2));
                }else
                {
                    if (m_pNpcBagNode != nullptr) {
                        m_pNpcBagNode->removeFromParent();
                        m_pNpcBagNode = nullptr;
                        m_isNpcBagNode = false;
                        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(NPCBAG_END_CALLBACK);
                    }
                }
            } else if (VIEWS::PLAYSCENE == m_view)
            {
                if (!m_isNpcBagNode) {
                    auto stageIndex = StageDataManager::getInstance()->getStageData(Player::getInstance()->getCurrStageIndex())->getStageIndex();
                    auto themeIndex = (stageIndex - (stageIndex % 12)) / 12;
                    
                    // 返回大地图
                    char szBuffer[64];
                    sprintf(szBuffer, "config/selStage/selStage%03d.json", themeIndex + 1);
                    Director::getInstance()->replaceScene(SelStageScene::createWithStageConfig(szBuffer));
                } else
                {
                    if (m_pNpcBagNode != nullptr) {
                        m_pNpcBagNode->removeFromParent();
                        m_pNpcBagNode = nullptr;
                        m_isNpcBagNode = false;
                        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(NPCBAG_END_CALLBACK);
                    }
                }
            }
        }
        else if(key == EventKeyboard::KeyCode::KEY_MENU){
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(listen, 1);
}

void AndroidRollBackListener::setNpcBagNode(NpcBagNode* npcBagNode)
{
    m_pNpcBagNode = nullptr;
    m_pNpcBagNode = npcBagNode;
}

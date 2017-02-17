//  UpdateAppDialog.cpp
//  superBaby
//
//  Created by adinnet2d on 2015/7/7.
//
//

#include "UpdateAppDialog.h"
#include "UpdateManager.h"
#include "CommonWaitingLayer.h"
#include "LogoScene.h"
#include "cocostudio/Cocostudio.h"

USING_NS_CC;

cocos2d::Scene* UpdateAppDialog::createScene()
{
    auto scene = Scene::create();
    auto layer = UpdateAppDialog::create();
    scene->addChild(layer);
    return scene;
}

bool UpdateAppDialog::init()
{
    if(!LayerColor::initWithColor(Color4B(0, 0, 0, 200)))
    {
        return false;
    }
    
    // Register Touch Event
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [](Touch* touch, Event* event){ return true; };
    listener->onTouchMoved = [](Touch* touch, Event* event){};
    listener->onTouchEnded = [](Touch* touch, Event* event){};
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto visiableOrigin = Director::getInstance()->getVisibleOrigin();
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto pLayer = CSLoader::createNode("res/ui/UpdateLayer.csb" );
    pLayer->ignoreAnchorPointForPosition( false );
    pLayer->setAnchorPoint( Vec2::ANCHOR_MIDDLE );
    pLayer->setPosition( Vec2(visiableOrigin.x+visibleSize.width*0.5f, visiableOrigin.y+visibleSize.height*0.5f) );
    this->addChild( pLayer );
    
    m_pHint = dynamic_cast<ui::Text*>( ui::Helper::seekNodeByName(pLayer, "LoadHint") );
    m_pOkBtn = dynamic_cast<ui::Button*>( ui::Helper::seekNodeByName(pLayer, "OkButton") );
    m_pCancelBtn = dynamic_cast<ui::Button*>( ui::Helper::seekNodeByName(pLayer, "CancelButton") );
    m_pLoadingBar = dynamic_cast<ui::LoadingBar*>( ui::Helper::seekNodeByName(pLayer, "LoadingBar") );
    m_pLoadingBarBg = dynamic_cast<ui::ImageView*>( ui::Helper::seekNodeByName(pLayer, "LoadingBarBg") );
    
    if( m_pHint )
    {
        m_pHint->setString( "正在检测新版本..." );
    }
    
    if( m_pOkBtn )
    {
        m_pOkBtn->setVisible( false );
        m_pOkBtn->setTitleFontName( "fonts/Hiragino Sans GB W3.otf" );
        m_pOkBtn->addClickEventListener( [this](Ref* pSender){ this->okUpdateApp(); } );
    }
    
    if( m_pCancelBtn )
    {
        m_pCancelBtn->setVisible( false );
        m_pCancelBtn->setTitleFontName( "fonts/Hiragino Sans GB W3.otf" );
        m_pCancelBtn->addClickEventListener( [this](Ref* pSender){ this->cancelUpdateApp(); } );
    }
    
    if( m_pLoadingBar )
    {
        m_pLoadingBar->setVisible( false );
    }
    
    if( m_pLoadingBarBg )
    {
        m_pLoadingBarBg->setVisible( false );
    }
    
    auto pWaiting = CommonWaitingLayer::create();
    pWaiting->setScale( 0.2f );
    pWaiting->setOpacity( 255 );
    pWaiting->setColor( Color3B(100, 100, 100) );
    pWaiting->setBackgroundColor( Color4B(10, 10, 10, 50) );
    this->addChild( pWaiting, 0, 100 );

    UpdateManager::getInstance()->chkSvrUpt([=](bool hasNew, const std::string info) {
        pWaiting->removeFromParent();
        if (hasNew)
        {
            m_pHint->setString("亲，检测到新版本，是否更新？");
            m_pOkBtn->setVisible(true);
            m_pCancelBtn->setVisible(true);
        } else {
            m_pHint->setString(info);
            m_pCancelBtn->setTitleText("确定");
            m_pCancelBtn->setVisible(true);
            m_pCancelBtn->loadTextures("res/ui/button-01.png", "res/ui/button-01.png", "res/ui/button-01.png");
            m_pCancelBtn->setPosition( Vec2(m_pOkBtn->getPositionX()*0.5f + m_pCancelBtn->getPositionX()*0.5f, m_pOkBtn->getPositionY()));
            
            this->scheduleOnce([ this ](float delta ) {
                cancelUpdateApp();
            }, 2.0f, "");
        }
    });
    return  true;
}

void UpdateAppDialog::okUpdateApp( void )
{
    if( m_pOkBtn )
    {
        m_pOkBtn->setVisible(false);
        m_pCancelBtn->setVisible(false);
        if( m_pHint )
        {
            m_pHint->setString( "下载中\%0" );
        }
        m_pLoadingBar->setVisible(true);
        m_pLoadingBarBg->setVisible(true);
        UpdateManager::getInstance()->apkUptFromSvr([this] (bool downloadCompleted)
        {
            if (downloadCompleted) {
                if (m_pHint != nullptr)
                {
                    m_pHint->setString( "下载中\%100" );
                }
                if( m_pLoadingBar )
                {
                    m_pLoadingBar->setPercent(100.0f);
                }
                this->removeFromParent();
                UpdateManager::getInstance()->installNewApk();
            } else {
                if (m_pHint != nullptr) {
                    m_pHint->setString("文件下载失败");
                }
                m_pCancelBtn->setTitleText("确定");
                m_pCancelBtn->setVisible(true);
                m_pCancelBtn->loadTextures("res/ui/button-01.png", "res/ui/button-01.png", "res/ui/button-01.png");
                m_pCancelBtn->setPosition( Vec2(m_pOkBtn->getPositionX()*0.5f + m_pCancelBtn->getPositionX()*0.5f, m_pOkBtn->getPositionY()));
                
                this->scheduleOnce([ this ](float delta ) {
                    cancelUpdateApp();
                }, 2.0f, "downFail");
            }
        }, [this](float percent) {
            if (m_pLoadingBar)
            {
                m_pLoadingBar->setPercent(percent >= 100.0f ? 100.0f : percent);
                if( m_pHint )
                {
                    m_pHint->setString(StringUtils::format("下载中 %.2f", percent) + "%");
                }
            }
        });
    }
}

//
void UpdateAppDialog::cancelUpdateApp( void )
{
    // 进入欢迎界面
    Director::getInstance()->replaceScene(LogoScene::create());
}


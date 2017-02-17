//
//  CommonWaitingLayer.cpp
//  yanxispace
//
//  Created by adinnet2d on 15/6/25.
//
//

#include "CommonWaitingLayer.h"

USING_NS_CC;

#define TAG_LOAD 100
#define TIME_OF_2T 1.5f

CommonWaitingLayer* CommonWaitingLayer::create( float time, std::function<void()> callback)
{
    CommonWaitingLayer *pRet = new(std::nothrow) CommonWaitingLayer();
    pRet->m_callback = callback;
    if (pRet && pRet->init( time ))
    {
        
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool CommonWaitingLayer::init( )
{
    if( !LayerColor::initWithColor( Color4B(0, 0,  0, 0) ) )
    {
        return false;
    }
    this->createLoading( );
    this->setTouchProperty();
    
    return true;
}

bool CommonWaitingLayer::init( float time )
{
    if( !LayerColor::initWithColor( Color4B(0, 0,  0, 200) ) || m_callback==nullptr )
    {
        return false;
    }
    
    this->init();
    
    this->scheduleOnce( schedule_selector(CommonWaitingLayer::stopWaitingAction), time );
    
    return true;
}

void CommonWaitingLayer::stopWaitingAction( float delta )
{
    //log( "stop all action... " );
    if ( m_callback )
    {
        if( m_pLoading )
        {
            m_pLoading->stopAllActions();
        }
        m_callback();
    }
}

void CommonWaitingLayer::startLoadingAnim()
{
    if (m_pLoading)
    {
        m_pLoading->setRotation( ((int)m_pLoading->getRotation() + 360/12) % 360 );
    }
}

void CommonWaitingLayer::stopLoadingAnim()
{
    if( m_pLoading )
    {
        m_pLoading->stopAllActions();
    }
}

void CommonWaitingLayer::setScale( float scale )
{
    if( m_pLoading )
    {
        m_pLoading->setScale( scale );
    }
}

void CommonWaitingLayer::setColor( const cocos2d::Color3B& color )
{
    if( m_pLoading )
    {
        m_pLoading->setColor( color );
    }
}

void CommonWaitingLayer::setOpacity(GLubyte op)
{
    if( m_pLoading )
    {
        m_pLoading->setOpacity( op );
    }
}

void CommonWaitingLayer::setBackgroundColor( const cocos2d::Color4B& color )
{
    this->initWithColor( color );
}

void CommonWaitingLayer::createLoading()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
    
    auto pLoading = Sprite::create("res/ui/loding.png");
    m_pLoading = pLoading;
    pLoading->setPosition(visibleOrigin.x + visibleSize.width * 0.5f, visibleOrigin.y + visibleSize.height * 0.5f );
    this->addChild(pLoading, TAG_LOAD);
    
    pLoading->runAction( RepeatForever::create( Sequence::create( CallFunc::create( CC_CALLBACK_0(CommonWaitingLayer::startLoadingAnim, this) ), DelayTime::create( TIME_OF_2T/12 ), NULL)  ) );
}

void CommonWaitingLayer::setTouchProperty()
{
    // 事件派发
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 答题监听器
    auto listenerRight = EventListenerTouchOneByOne::create();
    listenerRight->setSwallowTouches(true);
    
    listenerRight->onTouchBegan = [&](Touch* touch, Event* event)->bool
    {
        return true;
    };
    dispatcher->addEventListenerWithSceneGraphPriority(listenerRight, this);
}





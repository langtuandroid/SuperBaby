//
//  CommonWaitingLayer.h
//  yanxispace
//
//  Created by adinnet2d on 15/6/25.
//
//

#ifndef __yanxispace__CommonWaitingLayer__
#define __yanxispace__CommonWaitingLayer__

#include "cocos2d.h"

class CommonWaitingLayer : public cocos2d::LayerColor
{
public:
    CREATE_FUNC(CommonWaitingLayer);
    
    static CommonWaitingLayer* create( float time, std::function<void()> callback );
    
    virtual bool init( );
    virtual bool init( float time );
    
    virtual void setScale(float scale);
    virtual void setColor( const cocos2d::Color3B& color );
    virtual void setOpacity(GLubyte op);
    virtual void setBackgroundColor( const cocos2d::Color4B& color );
    
    virtual void stopLoadingAnim();
protected:
    void createLoading();
    void setTouchProperty();
    
    void stopWaitingAction( float delta );
    void startLoadingAnim();
    
    std::function<void()> m_callback = nullptr;
    
    cocos2d::Sprite* m_pLoading = nullptr;
};

#endif /* defined(__yanxispace__CommonWaitingLayer__) */

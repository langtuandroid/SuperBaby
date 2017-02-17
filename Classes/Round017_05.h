//
//  Round017_05.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/18.
//
//

#ifndef __AbelLLExplorer__Round017_05__
#define __AbelLLExplorer__Round017_05__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round017_05 : public RoundBase
{
public:
    Round017_05();
    virtual ~Round017_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round017_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer();
public:
    virtual void onEnterRound();
private:
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round017_05__) */

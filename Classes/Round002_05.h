//
//  Round002_05.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/3/26.
//
//

#ifndef __AbelLLExplorer__Round002_05__
#define __AbelLLExplorer__Round002_05__

#include "RoundBase.h"

USING_NS_CC;

class StageScene;
class Round002_05 : public RoundBase
{
public:
    Round002_05();
    virtual ~Round002_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round002_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void onEnterRound();
    virtual void celebrate(const float);
    
    void stageOver(float);
    //渐变变黑退出
    void fadeOutLayer(float);
private:
    void treeDownComplete();
private:
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round002_05__) */

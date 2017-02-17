//
//  Round015_05.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/14.
//
//

#ifndef __AbelLLExplorer__Round015_05__
#define __AbelLLExplorer__Round015_05__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round015_05 : public RoundBase
{
public:
    Round015_05();
    virtual ~Round015_05();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round015_05* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    //移动主角退出
    void moveActor();
    //打开门
    void openDoor();
    //渐变变黑退出
    void fadeOutLayer(float);
public:
    virtual void onEnterRound();
private:
    SpriteBatchNode* m_pBN = nullptr;
    Sprite* m_pDoorSP;
    Sprite* m_pDoorBeforeSp;
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
};

#endif /* defined(__AbelLLExplorer__Round015_05__) */

//
//  Round044_04.h
//  superBaby
//
//  Created by Administrator on 15/8/3.
//
//

#ifndef __superBaby__Round044_04__
#define __superBaby__Round044_04__

#include "RoundBase.h"
USING_NS_CC;
class Actor;
class StageScene;
class Round044_04 : public RoundBase
{
public:
    Round044_04();
    virtual ~Round044_04();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round044_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    virtual void onPreLoadedRes();
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变答案大小
    void changeScaleAnswersSchedule(float);
    
    //分享回调
    void shareCallback();
    
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //移动主角退出
    void moveActor();
    //渐变变黑退出
    void fadeOutLayer(float);
    void drawtest(const Rect& rect);
private:
    Sprite* m_pAnswersSp[6];            //答案图片
    Vec2 m_answersPos[6];               //答案坐标
    Vec2 m_answersEndPos[6];
    int m_effectId = 0;                //音效play的id
    int m_InitAniIndex = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    GLubyte m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
    Rect m_iceBoxRect;
};


#endif /* defined(__superBaby__Round044_04__) */

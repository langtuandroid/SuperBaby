//
//  Round032_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/9.
//
//

#ifndef __AbelLLExplorer__Round032_03__
#define __AbelLLExplorer__Round032_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round032_03 : public RoundBase
{
public:
    Round032_03();
    virtual ~Round032_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round032_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变答案大小以提示
    void changeScaleAnswersSchedule(float);
    
    void changePlatform();
    
private:
    Sprite* m_pCoralsSp;            //提珊瑚图片
    Sprite* m_pAnswersSp[4];
    Vec2 m_answersPos[4];
    Image* m_pCoralImage = nullptr;
    Image* m_pAnswersImage[4];
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex = -1;              //
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    unsigned int m_curCorrectNumer = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curRandNum = 0;
    int m_overedIndex = -1;
};

#endif /* defined(__AbelLLExplorer__Round032_03__) */

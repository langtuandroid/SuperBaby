//
//  Round031_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/8.
//
//

#ifndef __AbelLLExplorer__Round031_03__
#define __AbelLLExplorer__Round031_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round031_03 : public RoundBase
{
public:
    Round031_03();
    virtual ~Round031_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round031_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //改变提示大小
    void changeScaleHintsSchedule(float);
    
    void changePlatform();
    
private:
    Sprite* m_pHintsSp[3];            //提示图片
    Vec2 m_hintsPos[3];               //提示坐标
    Sprite* m_pHighLightHintSp[3];
    Sprite* m_pPlatformSp = nullptr;
    Sprite* m_pHighLightPlatformSp = nullptr;
    Sprite* m_pAnswersSp[3];
    Vec2 m_answersPos[3];
    Vec2 m_answersEndPos[3];
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex;                  //选中树索引
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pPaopaoActor = nullptr;
    Actor* m_pWhiteLightActor = nullptr;
    unsigned int m_curCorrectNum = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};


#endif /* defined(__AbelLLExplorer__Round031_03__) */

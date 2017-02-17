//
//  Round026_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/6/1.
//
//

#ifndef __AbelLLExplorer__Round026_03__
#define __AbelLLExplorer__Round026_03__

#include "RoundBase.h"

USING_NS_CC;

class Actor;
class StageScene;
class Round026_03 : public RoundBase
{
public:
    Round026_03();
    virtual ~Round026_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round026_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //上升答案
    void upMoveAnswersSchedule(float);
    
    //放大答案以提示
    void changeScaleTopic();
    
    //答对后下降答案
    void downMoveAnswersTopicSchedule(float);
    
private:
    Sprite* m_pAnswersSp[3];            //答案图片
    Vec2 m_answersPos[3];               //答案坐标
    Sprite* m_pTopicSp = nullptr;       //题目
    int m_effectId = 0;                 //音效play的id
    int m_curSelIndex;                  //选中树索引
    int m_InitAniIndex = 0;
    bool m_opentouched = false;
    bool m_jumped = false;
    Actor* m_pBoguangActor = nullptr;

};

#endif /* defined(__AbelLLExplorer__Round026_03__) */

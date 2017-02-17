//
//  Round016_03.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/15.
//
//

#ifndef __AbelLLExplorer__Round016_03__
#define __AbelLLExplorer__Round016_03__

#include "RoundBase.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;

class Actor;
class StageScene;
class Round016_03 : public RoundBase
{
public:
    Round016_03();
    virtual ~Round016_03();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round016_03* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //渐变显示答案
    void fadeInAnswers();
    
    //依次放大答案，以提示
    void changeScaleAnswersSchedule(float);
    
    //延伸玻璃桥
    void moveGlassBoard(float);
    
    //改变提示问号的大小以提示
    void changeScaleTarget();
    
    //答对后隐藏剩余答案
    void fadeOutAnswers();
    
private:
    SpriteBatchNode* m_pBN = nullptr;
    
    Sprite* m_pAnswerNumbers[3];            //答案数字
    
    Vec2 m_answerNumbersPos[3];
    Vec2 m_answerNumbersEndPos[4];            //答案随机后最终坐标
    
    ImageView* m_pTopicImageView;                 //题目
    
    Sprite* m_pGlassBoardSp;            //玻璃板
    Sprite* m_pRoadSp;                  //路
    
    Sprite* m_pTargetSp;                //目标sp
    
    int m_InitAniIndex;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    int m_effectId = 0;                //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
};


#endif /* defined(__AbelLLExplorer__Round016_03__) */

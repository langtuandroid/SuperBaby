//
//  Round011_04.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/9.
//
//

#ifndef __AbelLLExplorer__Round011_04__
#define __AbelLLExplorer__Round011_04__

#include "RoundBase.h"
#include <vector>
USING_NS_CC;

class Actor;
class SocialShareNode;
class StageScene;
class Round011_04 : public RoundBase
{
public:
    Round011_04();
    virtual ~Round011_04();
    
public:
    bool initWithRoundIndex(const int,StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round011_04* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    virtual void celebrate(const float);
    virtual void onEnterRound();
    virtual void onPreLoadedRes();
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();

    //移动主角退出
    void moveActor(float);
    
    //主角移动完成
    void moveActorComplete(float);
    
    //回合开始，等式一从上部出现
    void downMoveEquationSchedule(float);
    
    //回合开始，答案右边出现
    void leftMoveAnswersSchedule(float);
    
    //答对改变犀牛actor动作
    void changeHippoActor();
    
    //回合结束，上升完成的等式
    void upMoveEquationSchedule(float);
    //回合结束分享调用
    void roundEndShareCallBack(bool isShare);
    
    //分享回调
    void shareCallback();
    
    //渐变变黑退出
    void fadeOutLayer(float);
    
    //随机
    int randomIndex(int,int);
    //判断产生的随机数是否重复
    bool numberIsRepeat(std::vector<int>& ,int);
    //得到一个不重复的随机树
    int getRandomNumber(std::vector<int>&,int,int);
        
private:
    SpriteBatchNode* m_pBN = nullptr;     //BN
    Sprite* m_pEquation[2];     //未完成的等式
    Sprite* m_pPassEquation[2];  //答对的等式
    Sprite* m_pAnswers[2];      //答案组
    
    Actor* m_pHippoActor = nullptr;       //河马actor

    Vec2 m_answersPos[2];          //答案坐标组
    Vec2 m_answerEndPos[2];
    
    Rect m_rect[2];                 //触摸响应区
    Vec2 m_centerRectsPos[2];       //触摸区中点坐标组
    
    SocialShareNode* m_pSSNode = nullptr;        // 社交分享
    
    Actor* m_pCelebrateActor = nullptr;
    Actor* m_pCelebrateBgActor = nullptr;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;
    int m_InitAniIndex = 0;
    unsigned int m_correctNumber = 0;
    std::vector<int> m_pCreatedIndexs;
    int m_effectId = 0;
    
    int m_opacity = 0;
    LayerColor* m_LayerColor;
    bool m_opentouched = false;
    bool m_jumped = false;
};

#endif /* defined(__AbelLLExplorer__Round011_04__) */

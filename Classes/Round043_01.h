//
//  Round043_01.h
//  superBaby
//
//  Created by Administrator on 15/7/31.
//
//

#ifndef __superBaby__Round043_01__
#define __superBaby__Round043_01__

#include "RoundBase.h"
#include <ui/CocosGUI.h>
USING_NS_CC;

class Actor;
class StageScene;
class Round043_01 : public RoundBase
{
public:
    Round043_01();
    virtual ~Round043_01();
    
public:
    bool initWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    static Round043_01* createWithRoundIndex(const int, StageScene*, const Vec2& pos = Vec2::ZERO);
    
public:
    void cleanUp();
    virtual void onEnterRound();
    virtual void celebrate(const float);
    /* 开启触摸 */
    virtual void openTouch();
    /*  回合结束回调 */
    virtual void endRound();
    
    //上升颜色板
    void upColorsBoard();
    
    //依次放大颜色
    void changeScaleColorsSchedule(float);
    
    //显示高亮提示
    void showHintLightSp();
    //下降颜色板
    void downColorsBoard();
    
private:
    Sprite* m_pDoorsSp[2];
    Vec2 m_doorsPos[2];
    Sprite* m_pLeftColorsSp[3];
    Vec2 m_leftColorsPos[3];
    ui::ImageView* m_pHintsImageView[3];
    Vec2 m_hintsPos[3];
    Sprite* m_pHintLightSp = nullptr;
    Sprite* m_pColorBoard = nullptr;     //颜色底板
    Sprite* m_pColors[3];                //颜色组
    Vec2 m_colorsPos[3];                 //颜色位置组
    Vec2 m_colorsEndPos[3];
    Image* m_pImages[3];
    std::string m_colorsStr[3];
    int m_InitAniIndex = 0;
    int m_effectId = 0;                  //音效play的id
    bool m_opentouched = false;
    bool m_jumped = false;
    int m_curCorrectNum = 0;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
};


#endif /* defined(__superBaby__Round043_01__) */

//
//  UpdateAppDialog.h
//  yanxispace
//
//  Created by adinnet2d on 2015/7/7.
//
//

#ifndef __yanxispace__UpdateAppDialog__
#define __yanxispace__UpdateAppDialog__

#include "cocos2d.h"
#include "ui/cocosGUI.h"

class UpdateAppDialog : public cocos2d::LayerColor
{
public:
    static cocos2d::Scene* createScene();
    
    CREATE_FUNC(UpdateAppDialog);
    virtual bool init();
    
    void okUpdateApp();
    void cancelUpdateApp();

protected:
    cocos2d::ui::LoadingBar* m_pLoadingBar = nullptr;
    cocos2d::ui::ImageView* m_pLoadingBarBg = nullptr;
    cocos2d::ui::Button* m_pOkBtn = nullptr;
    cocos2d::ui::Button* m_pCancelBtn = nullptr;
    cocos2d::ui::Text* m_pHint = nullptr;
};

#endif /* defined(__yanxispace__UpdateAppDialog__) */

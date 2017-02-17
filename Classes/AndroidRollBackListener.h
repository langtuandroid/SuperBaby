//
//  AndroidRollBackListener.h
//  superBaby
//
//  Created by Administrator on 15/7/16.
//
//

#ifndef __superBaby__AndroidRollBackListener__
#define __superBaby__AndroidRollBackListener__

enum VIEWS
{
    WELCOME = 1,
    THEMESEL,
    SELSTAGE,
    PLAYSCENE
};

class NpcBagNode;
class AndroidRollBackListener
{
public:
    AndroidRollBackListener();
    virtual ~AndroidRollBackListener();
    static AndroidRollBackListener* create(const unsigned int);
    bool init(const unsigned int);
    void backScene();
    void setNpcBagNode(NpcBagNode* npcBagNode);
    void setIsNpcBagNode(bool isNpcBagNode){m_isNpcBagNode = isNpcBagNode;}
private:
    NpcBagNode* m_pNpcBagNode= nullptr;
    unsigned int m_clickNum = 0;
    unsigned int m_view = 0;
    bool m_isNpcBagNode = false;
};
#endif /* defined(__superBaby__AndroidRollBackListener__) */

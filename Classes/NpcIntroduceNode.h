//
//  NpcIntroduceNode.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/23.
//
//

#ifndef __AbelLLExplorer__NpcIntroduceNode__
#define __AbelLLExplorer__NpcIntroduceNode__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
using namespace ui;
class RoundUINode;
class NpcBagNode;
class NpcIntroduceNode : public Node
{
public:
    NpcIntroduceNode();
    virtual ~NpcIntroduceNode();
    static NpcIntroduceNode* createWithNpcKey(std::string);
    bool initWithNpcKey(std::string);
    //关联背包节点
    void attachNpcBagNode(NpcBagNode* npcBagNode){m_pNpcBagNode = npcBagNode;}
private:
    NpcBagNode* m_pNpcBagNode;
    ImageView* m_pBackImage;
    int m_effectId = 0;                //音效play的id
    std::string m_npcStr;
    std::string m_npcPath;
};

#endif /* defined(__AbelLLExplorer__NpcIntroduceNode__) */

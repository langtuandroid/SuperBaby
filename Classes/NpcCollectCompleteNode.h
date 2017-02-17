//
//  NpcCollectCompleteNode.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/4/23.
//
//

#ifndef __AbelLLExplorer__NpcCollectCompleteNode__
#define __AbelLLExplorer__NpcCollectCompleteNode__

#include "cocos2d.h"

USING_NS_CC;
class NpcCollectCompleteNode : public Node
{
public:
    NpcCollectCompleteNode();
    virtual ~NpcCollectCompleteNode();
    static NpcCollectCompleteNode* createWithNpcKey(std::string);
    bool initWithNpcKey(std::string);
private:
    unsigned int m_opacityNumber;
    std::string m_npcStr;
    std::string m_npcPath;
};

#endif /* defined(__AbelLLExplorer__NpcCollectCompleteNode__) */

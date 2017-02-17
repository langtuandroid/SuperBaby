//
//  LoadingNode.h
//  AbelLLExplorer
//
//  Created by Administrator on 15/5/11.
//
//

#ifndef __AbelLLExplorer__LoadingNode__
#define __AbelLLExplorer__LoadingNode__

#include "cocos2d.h"

USING_NS_CC;

class LoadingNode : public Node
{
public:
    LoadingNode();
    virtual ~LoadingNode();
    
public:
    static LoadingNode* create();
    bool init();
    
private:
    Node* m_pNode = nullptr;
};

#endif /* defined(__AbelLLExplorer__LoadingNode__) */

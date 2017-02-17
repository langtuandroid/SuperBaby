//
//  adjustPosNode.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/5/4.
//
//

#ifndef __AbelLLExplorer__adjustPosNode__
#define __AbelLLExplorer__adjustPosNode__

#include "cocos2d.h"

USING_NS_CC;

class adjustPosNode : public Node
{
public:
    adjustPosNode();
    virtual ~adjustPosNode();
    
public:
    static adjustPosNode* createWithNode(Node*);
    bool initWithNode(Node*);
    
    void update( float );
    
protected:
    void setNewPos( const Vec2& pos );
    void setGafNewPos( const Vec2& pos );
    
    void updateArrawDirection();
private:
    Node* m_pNode = nullptr;
    Vec2 m_originalPoint;
    Vec2 m_touchBeganPoint;         // 拖放开始位置
    unsigned int m_curSelectBtn = 0;
    float m_fPressedTime = 0.0f;
};

#endif /* defined(__AbelLLExplorer__adjustPosNode__) */

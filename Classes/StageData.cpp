//
//  StageData.cpp
//  AbelLLExplorer
//
//  Created by adinnet on 2015/2/28.
//
//

#include "StageData.h"

/*
 *  constructor
 */
StageData::StageData()
{
    m_roundScroll.clear();
    m_LeadActorMove.clear();
    m_bgImgArray.clear();
    m_roundTexts.clear();
    m_foreImgArray.clear();
}

/*
 *  destructor
 */
StageData::~StageData()
{
    destroyData();
}

// 添加背景图片
void StageData::appendBgImg(BgImgUnit& biu)
{
    m_bgImgArray.push_back(biu);
}

// 添加前景图片
void StageData::appendForeImg(ForeImgUnit & foreImg)
{
    m_foreImgArray.push_back(foreImg);
}

/*
 *  添加回合滚动参数
 */
void StageData::appendRoundScrollPara(const ScrollPara& pRsp)
{
    m_roundScroll.push_back(pRsp);
}

/*
 *  获取回合滚动参数
 *
 * roundIndex:回合索引(基数为 0)
 */
ScrollPara StageData::getRoundScroll(const unsigned int roundIndex)
{
    return roundIndex < m_roundScroll.size() ?
    m_roundScroll[roundIndex] : ScrollPara(ScrollOrient::SCROLL_HORIZONTAL, 0.0f);
}

/* 
 *  添加主角移动参数
 */
void StageData::appendLAMovePara(const ScrollPara& laMove)
{
    m_LeadActorMove.push_back(laMove);
}

/*
 *  获取主角移动参数
 */
ScrollPara StageData::getLAMovePara(const unsigned int roundIndex /*回合参数*/) {
    return roundIndex < m_LeadActorMove.size() ?
    m_LeadActorMove[roundIndex] : ScrollPara(ScrollOrient::SCROLL_HORIZONTAL, 0.0f);
}

/*
 *  判断回合索引是否有效
 *
 * roundIndex:回合索引(基数为 0)
 */
bool StageData::hasRound(const unsigned int roundIndex)
{
    return roundIndex < m_roundScroll.size();
}

/*
 *  获取指定回合索引的所有回合文字
 * 
 * 参数：
 *      roundIndex  回合索引(基数为 0)
 */
void StageData::getRoundText(const unsigned int roundIndex, std::vector<RoundTextUnit>& roundTexts)
{
    roundTexts.clear();
    for (auto &it : m_roundTexts) {
        if (it.roundIndex == roundIndex) {
            roundTexts.push_back(it);
        }
    } /*for*/
}

/*
 * 释放数据
 */
void StageData::destroyData()
{
    m_roundScroll.clear();
    m_LeadActorMove.clear();
    m_bgImgArray.clear();
    m_roundTexts.clear();
    m_foreImgArray.clear();
}

//
void StageData::appendEmptyRoundIndex(const unsigned int roundIndex)
{
    m_emptyRoundIndexs.push_back(roundIndex);
}

//
void StageData::appendNpcRoundIndex(const unsigned int roundIndex)
{
    m_npcRoundIndexs.push_back(roundIndex);
}

//
bool StageData::isEmptyRound(const unsigned int roundIndex)
{
    bool empytRound = false;
    for (auto const &it : m_emptyRoundIndexs) {
        if (it == roundIndex) {
            empytRound = true;
            break;
        }
    } /*for*/
    return empytRound;
}

/*
 * 是否是有 npc 需要收集的回合
 */
bool StageData::isNpcRound(const unsigned int roundIndex)
{
    bool npcRound = false;
    for (auto const &it : m_npcRoundIndexs)
    {
        if (it == roundIndex) {
            npcRound = true;
            break;
        }
    } /*for*/
    return npcRound;
}

//
//  StageData.h
//  AbelLLExplorer
//
//  Created by adinnet on 2015/2/28.
//
//  关卡数据
//

#ifndef __AbelLLExplorer__StageData__
#define __AbelLLExplorer__StageData__

#include <vector>
#include <map>

#define RESOURCE_TYPE_PNG   1       // png 文件类型
#define RESOURCE_TYPE_GAF   2       // gaf 动画文件类型

// 背景图片
typedef struct BgImgUnit
{
    BgImgUnit()
    {
        m_file = "";
        m_tileOrient = 1;           // 1:水平 2:垂直
        m_upDown = -1;
    }
    BgImgUnit(unsigned int tileOrient, std::string file,
              int upDonw = -1)
    {
        m_tileOrient = tileOrient;
        m_file = file;
        m_upDown = upDonw;
    }
    std::string m_file;             // 图片文件完整路径
    unsigned int m_tileOrient;      // 方向
    int m_upDown;                   // 方向为垂直的话 1: 上升 2:下降
} BgImgUnit;

// 前景图片
typedef struct ForeImgUnit
{
    ForeImgUnit()
    {
        m_file = "";
        m_tileOrient = 1;
        m_upDown = -1;
    }
    ForeImgUnit(unsigned int tileOrient, std::string file, int upDown = -1)
    {
        m_tileOrient = tileOrient;
        m_file = file;
        m_upDown = upDown;
    }
    std::string m_file;
    unsigned int m_tileOrient;
    int m_upDown;
}ForeImgUnit;

// 滚动方向
enum class ScrollOrient
{
    SCROLL_HORIZONTAL,
    SCROLL_VERTICAL
};

// 滚动参数
typedef struct ScrollPara
{
    ScrollPara()
    {
        m_rso = ScrollOrient::SCROLL_HORIZONTAL;
        m_scroll = 0.0f;
        m_vscroll = 0.0f;
        m_bgImgIndex = -1;
    }
    ScrollPara(ScrollOrient rso, float scroll, int bgImgIndex = -1, float vscroll = 0.0f)
    {
        m_rso = rso;
        m_scroll = scroll;
        m_bgImgIndex = bgImgIndex;
        m_vscroll = vscroll;
    }
    ScrollOrient m_rso;             // 滚动方向
    float m_scroll;                 // 切换回合时候的水平滚动距离
    float m_vscroll;                // 垂直升降距离
    /*
     *  关联的关卡背景图索引号
     *  该成员变量只在滚动方向为垂直才有意义
     *  该背景图应该是屏幕高度的两倍
     */
    int m_bgImgIndex;
} ScrollPara;

// 回合文字单元
typedef struct RoundTextUnit
{
    unsigned int roundIndex;        // 文字所属回合索引
    float posX;
    float posY;
    unsigned int fontSize;
    unsigned int maxLineWidth;
    std::string fontFile;
    std::string text;
} RoundTextUnit;

class StageData
{
public:
    StageData();
    virtual ~StageData();

public:
    /* 获取和设置关卡索引 */
    inline void setStageIndex(const unsigned int stageIndex) {m_index = stageIndex;}
    inline unsigned int getStageIndex() const {return m_index;}
    
    // 添加背景图片
    void appendBgImg(BgImgUnit&);
    // 获取背景图片数组
    inline std::vector<BgImgUnit>& getBgImgArray() {return m_bgImgArray;}
    
    // 添加前景图片
    void appendForeImg(ForeImgUnit&);
    inline std::vector<ForeImgUnit>& getForeImgArray() {return m_foreImgArray;}
    
    // 添加回合滚动参数
    void appendRoundScrollPara(const ScrollPara&);
    // 获取回合滚动参数
    ScrollPara getRoundScroll(const unsigned int);
    
    // 添加主角移动参数
    void appendLAMovePara(const ScrollPara&);
    // 获取主角移动参数
    ScrollPara getLAMovePara(const unsigned int);
    
    // 判断回合索引是否有效
    bool hasRound(const unsigned int roundIndex);
    
    /* 获取回合数量 */
    inline unsigned int getNumberOfRounds() {return (unsigned int)m_roundScroll.size();}
    
    // 添加回合文字
    inline void appendRoundTextUnit(const RoundTextUnit& roundTextUnit)
        {m_roundTexts.push_back(roundTextUnit);}
    /* 获取回合文字 */
    void getRoundText(const unsigned int, std::vector<RoundTextUnit>&);
    
    // 释放数据
    void destroyData();

    // 获取和设置艾米初始位置
    inline float getAimeePosX() const {return m_AimeePosX;}
    inline float getAimeePosY() const {return m_AimeePosY;}
    inline void setAimeePosX(const float posX) {m_AimeePosX = posX;}
    inline void setAimeePosY(const float posY) {m_AimeePosY = posY;}
    // 获取和设置蓓蓓初始位置
    inline float getBeBeiPosX() const {return m_BebeiPosX;}
    inline float getBeBeiPosY() const {return m_BebeiPosY;}
    inline void setBeBeiPosX(const float posX) {m_BebeiPosX = posX;}
    inline void setBeBeiPosY(const float posY) {m_BebeiPosY = posY;}
    
    // 获取和设置过关分享音频
    inline void setShareAudio(const std::string& saudio)  { m_saudio = saudio;}
    inline std::string getShareAudio() const { return m_saudio;}
    
    // 题目音频
    inline void appendQaudioFile(const std::string& saudioFile) {m_qaudioFiles.push_back(saudioFile);}
    inline void appendQaudioTime(const float saudioTime) {m_qaudioTime.push_back(saudioTime);}
    inline std::string getQaudioFile(const unsigned int roundIndex) {return m_qaudioFiles[roundIndex];}
    inline float getQaudioTime(const unsigned int roundIndex) {return m_qaudioTime[roundIndex];}
    
    // 获取和设置答对点击触发特效
    std::string getAnswerCorrectEffect() const {return m_answerCorrectEffect;}
    inline void setAnswerCorrectEffect(const std::string& effectFile) {m_answerCorrectEffect = effectFile;};
    
    void appendEmptyRoundIndex(const unsigned int);
    bool isEmptyRound(const unsigned int);
    
    void appendNpcRoundIndex(const unsigned int);
    bool isNpcRound(const unsigned int);

    /* 添加和获取预加载资源 */
    inline void appendPreLoadImg(const std::string& imgFile) {m_preloadImgs.push_back(imgFile);}
    inline void appendPreLoadGaf(const std::string& gafFile) {m_preloadGafs.push_back(gafFile);}
    std::vector<std::string>& getPreloadImgs() {return m_preloadImgs;}
    std::vector<std::string>& getPreloadGafs() {return m_preloadGafs;}
    
private:
    unsigned int m_index = 0;           // 关卡索引号
    
    // 背景图片数组
    std::vector<BgImgUnit> m_bgImgArray;
    
    //前景图片数组
    std::vector<ForeImgUnit> m_foreImgArray;

    // 回合滚动
    std::vector<ScrollPara> m_roundScroll;

    // 主角移动
    std::vector<ScrollPara> m_LeadActorMove;
    
    // 回合文字
    std::vector<RoundTextUnit> m_roundTexts;
    
    // 空回合
    std::vector<unsigned int> m_emptyRoundIndexs;
    
    // 过关分享音频
    std::string m_saudio = "";
    
    // 题目音频
    std::vector<std::string> m_qaudioFiles; // 文件
    std::vector<float> m_qaudioTime;  // 时长
    
    // 有 npc 收集需要的回合索引
    std::vector<unsigned int> m_npcRoundIndexs;
    
    // 艾米初始位置
    float m_AimeePosX = 0.0f;
    float m_AimeePosY = 0.0f;
    // 蓓蓓初始位置
    float m_BebeiPosX = 0.0f;
    float m_BebeiPosY = 0.0f;
    
    // 答题正确特效
    std::string m_answerCorrectEffect = "";
    
    // 要预加载的 gaf
    std::vector<std::string> m_preloadGafs;
    
    // 要预加载的 png
    std::vector<std::string> m_preloadImgs;
    
    /* 是否需要初始化资源 */
    bool m_needInitAsset = false;
    /* 是否已经初始化完成 */
    bool m_initAsseted = false;
};

#endif /* defined(__AbelLLExplorer__StageData__) */

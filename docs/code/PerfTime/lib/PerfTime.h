#ifndef PERFTIME_H
#define PERFTIME_H

#include <string>
#include <vector>
#include <functional>


namespace OwO
{
    const std::string KW_MAX_START = "max_start_time"; // 最大开始时间，StartHit 记录的所有开始时间的最大值
    const std::string KW_MIN_START = "min_start_time"; // 最小开始时间，StartHit 记录的所有开始时间的最小值
    const std::string KW_MAX_END = "max_end_time"; // 最大结束时间，EndHit 记录的所有开始时间的最大值
    const std::string KW_MIN_END = "min_end_time"; // 最小结束时间，EndHit 记录的所有开始时间的最小值
    const std::string KW_TOTAL_TIME = "total_time"; // 总时间， (EndHit - StartHit) 记录的时间差总和
    const std::string KW_COUNT = "count"; // 调用次数， (EndHit - StartHit) 调用次数
    const std::string KW_VALID = "valid"; // 有效标志，0：EndHit 与 StartHit 配对，结果有效；否则无效
    const std::string KW_POSITION = "position"; // 标记所在位置
    const std::string SEP_SIGN = "  "; // 分隔符 

    const int FORMAT_PLACEHOLDER_NUM = 16;

    // 自定义报告生成回调函数
    class Dataframe;
    using FcnOutputCallback_t = std::vector<double>(const std::string & strMark, const OwO::Dataframe & dataFrame);
} // namespace OwO


#define _TO_STR(x)  (#x)
#define _MACRO_STR(x) _TO_STR(x)

/* 获取源文件名 */
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)

/* 获取位置，例如 test.cpp:100 */
#define OwO_POS OwO::GetPosition(__FILENAME__, _MACRO_STR(__LINE__))

/* 自动添加 OwO_POS */
#define OwO_START(strMark,...) OwO::StartHit(strMark, OwO_POS , ##__VA_ARGS__)
#define OwO_END(strMark) OwO::EndHit(strMark)
#define OwO_BLOCK_MARK(strMark,...) OwO::HitBlock __clsBlock(strMark, OwO_POS , ##__VA_ARGS__)
#define OwO_BLOCK(strMark,...)  for(int __i = (OwO::HitBlock(strMark, OwO_POS , ##__VA_ARGS__),1); __i == 1 ; __i--)


namespace OwO
{
    std::string GetPosition(const std::string & strFile, const std::string & strLine);

    /**
     * @brief 标记点开始记时
     * @param strMark 标记名称
     * @param strPos 标记名称
     * @param vecSection 标记对应的分组，默认分组 default
     */
    void StartHit(const std::string & strMark, const std::string & strPos, const std::vector<std::string> & vecSection = std::vector<std::string>());
    
    /**
     * @brief 标记点记时结束
     * @param strMark 标记名称
     */
    void EndHit(const std::string & strMark);

    /**
     * @brief 自定义报告输出列
     * @param vecHeader 自定义列名
     * @param fcn 自定义输出回调
     */
    void AppendReport(const std::vector<std::string> & vecHeader, std::function<FcnOutputCallback_t> fcn);

    /**
     * @brief 输出指定标记
     * @param strMark 标记名
     * @return 标记对应数据
     */
    std::string Show(const std::string & strMark);

    /**
     * @brief 报告生成。会清除历史数据
     * @param vecSection 需要输出的组。若不指定，默认输出全部组
     * @return 报告
     */
    std::string Report(const std::vector<std::string> & vecSection = std::vector<std::string>());

    /**
     * @brief 报告生成。会清除全部历史数据
     * @param strPath 保存 csv 的路径
     * @param vecSection 需要输出的组。若不指定，默认输出全部组
     * @return 报告
     */
    std::string Report(const std::string & strPath, const std::vector<std::string> & vecSection = std::vector<std::string>() );

    /**
     * @brief 标记块，自动维护标记的开始和结束
     */
    class HitBlock{
    public:
        HitBlock(const std::string & strMark, const std::string & strPos,  const std::vector<std::string> & vecSection = std::vector<std::string>())
            : m_strMark(strMark)  { StartHit(strMark, strPos, vecSection); }

        ~HitBlock(){ EndHit(m_strMark); }
    private:
        std::string m_strMark;
    };
} // namespace OwO



#endif /* PERFTIME_H */

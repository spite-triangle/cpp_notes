#ifndef _PERF_TIME_STATISTICS_H_
#define _PERF_TIME_STATISTICS_H_


#include <string>

namespace PerfTime {
	// 性能统计类型
	enum class PerfType
	{
		// 前端打点取最靠前，后端打点取最靠后
		PERF_E2E_MAX = 0,   // 端到端MAX性能统计，即只计算最先开始的时间点和最后结束的时间点之间的耗时，支持多线程
		// 前端打点取最靠后，后端打点取最靠前
		PERF_E2E_MIN,   // 端到端MIN性能统计，即只计算最后开始的时间点和最先结束的时间点之间的耗时，支持多线程
		// 前端打点取最靠前，后端打点取最靠前
		PERF_E2E_TAIL_MIN,
		// 
		PERF_E2E_HEAD_MAX,
		PERF_ACC,       // 累加性能统计，即将每一次 [start, end] 的耗时累加，适合做接口级的调用次数和总耗时统计，支持多线程
	};

	/* @fn      StartHit
	*  @brief   打点，起始点，可在同一进程代码任意地方打点
	*  @param   nPerfType  统计类型
	*  @param   strMark  标志字符串，唯一标识统计数据
	*  @return  void
	*/
	void StartHit(PerfType nPerfType, const char* strMark);

	/* @fn      EndHit
	*  @brief   打点，起始点，可在同一进程代码任意地方打点
	*  @param   nPerfType  统计类型
	*  @param   strMark  标志字符串，唯一标识统计数据
	*  @return  void
	*/
	void EndHit(PerfType nPerfType, const char* strMark);

	/* @fn      OutPutReport
	*  @brief   输出性能统计信息到文件，输出后数据将被清理
	*  @param   pFile  文件全路径，支持文本文件
	*  @param   isOutputCmd  统计数据是否输出到终端
	*  @return  void
	*/
	void OutPutReport(const char* pFile , bool bIsOutputCmd = true);

    std::string OutPutReport();
}
#endif // !_PERF_TIME_STATISTICS_H_

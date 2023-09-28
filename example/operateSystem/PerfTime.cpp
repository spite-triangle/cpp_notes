#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <mutex>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <exception>

#include "PerfTime.h"

#if _MSVC_VER <= 1800
#define constexpr const
#endif


namespace PerfTime {
	using UINT64 = unsigned long long;

	constexpr bool PERF_SWITCH = true;

	// 耗时统计信息
	struct PerfTimeInfo
	{
		// 耗时，毫秒
		UINT64 m_ulTimeMicroSec;
		// 调用次数
		UINT64 m_ulCallCount;

		PerfTimeInfo(UINT64 ulCostTime, UINT64 ulCallCount) :m_ulTimeMicroSec(ulCostTime), m_ulCallCount(ulCallCount)
		{}
	};

	// 打点信息
	struct HitTimeInfo
	{
		UINT64 m_ulHitTime;				// 打点时间
		bool m_bIsStart;                 // 状态，true: 打点开始, false: 打点结束
		PerfType m_nPerfType;			// 打点类型
		HitTimeInfo(PerfType nPerfType, UINT64 ulTime, bool bIsStart) :m_nPerfType(nPerfType), m_ulHitTime(ulTime), m_bIsStart(bIsStart)
		{}
	};


	class PerfInterface {
	public:
		PerfInterface() = delete;

		PerfInterface(const PerfInterface&) = delete;

		PerfInterface(PerfInterface&&) = delete;

		inline static void Clear();

		// 性能统计接口
		static void Start(PerfType nPerfType, const std::string& strMark);
		static void End(PerfType nPerfType, const std::string& strMark);

        // 结果
        static std::string GenerateReport();

		// 写出统计结果
		static void OutPutStattisticsRes(const std::string& strFile, bool bIsOutputCmd);

		// 获取时间点，精确到微秒
		inline static UINT64 GetSysTimeMicros()
		{
			// 返回 Microseconds
			using namespace std::chrono;
			return duration_cast<microseconds>( time_point_cast<microseconds>(system_clock::now()).time_since_epoch()).count();
		}
	private:
		// 计算结果
		static void CalData(std::map<std::string, PerfTimeInfo>& mark2TimeDataMap);
		// 插入打点数据
		static void InsertHitTimeData(const std::string& strMark, PerfType nPerfType, bool bIsStart);

		// 打点信息
		static std::map<std::string, std::vector<HitTimeInfo>> s_mapTimeRecord;
		static std::mutex s_xTimeRecord;
	};

	// 静态成员数据定义
	std::map<std::string, std::vector<HitTimeInfo>> PerfInterface::s_mapTimeRecord;
	std::mutex PerfInterface::s_xTimeRecord;

	// 静态成员函数定义
	void PerfInterface::Start(PerfType nPerfType, const std::string& strMark)
	{
		// 开始打点
		InsertHitTimeData(strMark, nPerfType, true);
	}

	void PerfInterface::End(PerfType nPerfType, const std::string& strMark)
	{
		// 结束打点
		InsertHitTimeData(strMark, nPerfType, false);
	}



    void PerfInterface::InsertHitTimeData(const std::string& strMark, PerfType nPerfType, bool bIsStart)
	{
		auto ulTime = GetSysTimeMicros();
		{
			std::lock_guard<std::mutex> lck(s_xTimeRecord);
			s_mapTimeRecord[strMark].emplace_back(nPerfType, ulTime, bIsStart);
		}
	}

	void PerfInterface::CalData(std::map<std::string, PerfTimeInfo>& mark2TimeDataMap)
	{
		// 加锁，一次性拷贝数据
		std::map<std::string, std::vector<HitTimeInfo>> mapTimeRecordCopy;
		{
			std::lock_guard<std::mutex> lck(s_xTimeRecord);
			mapTimeRecordCopy = s_mapTimeRecord;
		}

		// 逐个 strMark 计算
		for (auto& oneRecord : mapTimeRecordCopy)
		{
			const std::string& strMark = oneRecord.first;
			std::vector<HitTimeInfo>& hitTimes = oneRecord.second;

			// 将数据分别拷贝到开始/结束两个打点队列中，用于后续计算
			std::vector<UINT64> vecStartHitTimes;
			std::vector<UINT64> vecEndHitTimes;
			for (HitTimeInfo& hitTime : hitTimes)
			{
				if (hitTime.m_bIsStart == true)
				{
					vecStartHitTimes.push_back(hitTime.m_ulHitTime);
					continue;
				}
				vecEndHitTimes.push_back(hitTime.m_ulHitTime);
			}

			// 简单校验打点数据
			if ((vecStartHitTimes.empty()) || (vecEndHitTimes.empty()))
			{
				// 打点错误，没有开始或者结束打点
				std::cout << strMark << " hit error, does not start or end dots." << std::endl;
				mark2TimeDataMap.emplace(strMark, PerfTimeInfo(0, 0));
				continue;
			}

			// 计算耗时
			const PerfType ePerfType = hitTimes.back().m_nPerfType;
			if (ePerfType == PerfType::PERF_E2E_MAX)
			{
				// 端到端类型
				UINT64 ulDurTime = *(std::max_element(vecEndHitTimes.begin(), vecEndHitTimes.end())) -
					*(std::min_element(vecStartHitTimes.begin(), vecStartHitTimes.end()));
				mark2TimeDataMap.emplace(strMark, PerfTimeInfo(ulDurTime, 1));
				continue;
			}
			else if (ePerfType == PerfType::PERF_E2E_MIN)
			{
				UINT64 ulDurTime = *(std::min_element(vecEndHitTimes.begin(), vecEndHitTimes.end())) -
					*(std::max_element(vecStartHitTimes.begin(), vecStartHitTimes.end()));
				mark2TimeDataMap.emplace(strMark, PerfTimeInfo(ulDurTime, 1));
				continue;
			}
			else if (ePerfType == PerfType::PERF_E2E_TAIL_MIN)
			{
				UINT64 ulDurTime = *(std::min_element(vecEndHitTimes.begin(), vecEndHitTimes.end())) -
					*(std::min_element(vecStartHitTimes.begin(), vecStartHitTimes.end()));
				mark2TimeDataMap.emplace(strMark, PerfTimeInfo(ulDurTime, 1));
				continue;
			}
			else if (ePerfType == PerfType::PERF_E2E_HEAD_MAX)
			{
				UINT64 ulDurTime = *(std::max_element(vecEndHitTimes.begin(), vecEndHitTimes.end())) -
					*(std::max_element(vecStartHitTimes.begin(), vecStartHitTimes.end()));
				mark2TimeDataMap.emplace(strMark, PerfTimeInfo(ulDurTime, 1));
				continue;
			}
			else
			{
				// 累加类型
				if (vecStartHitTimes.size() != vecEndHitTimes.size())
				{
					// 打点错误，开始和结束打点没有成对出现，无法计算
					std::cout << strMark << " hit error, start and end dots are not paired." << std::endl;
					mark2TimeDataMap.emplace(strMark, PerfTimeInfo(0, 0));
					continue;
				}
				UINT64 ulDurTime = std::accumulate(vecEndHitTimes.begin(), vecEndHitTimes.end(), 0) -
					std::accumulate(vecStartHitTimes.begin(), vecStartHitTimes.end(), 0);
				mark2TimeDataMap.emplace(strMark, PerfTimeInfo(ulDurTime, vecStartHitTimes.size()));
			}
		}
	}

	void PerfInterface::Clear()
	{
		std::lock_guard<std::mutex> lck(s_xTimeRecord);
		s_mapTimeRecord.clear();
	}

    std::string PerfInterface::GenerateReport()
    {
        // 耗时数据格式化占位符宽度16
		constexpr int TIME_FORMAT_PLACEHOLDER_NUM = 16;
		// 调用数量格式化占位符宽度10
		constexpr int CALL_FORMAT_PLACEHOLDER_NUM = 10;

        std::map<std::string, PerfTimeInfo> tmpMark2TimeData; // <strMark, 耗时数据>
		CalData(tmpMark2TimeData); // 计算数据

        std::ostringstream ostream;
        {
            ostream << "\n" << "PerfTime:" << "\n";
            int nIndex = 0;
            for (const auto& kv : tmpMark2TimeData)
            {
                // 输出格式如下，各部分之间进行了格式化对齐操作
                // 1. RGB_2_YUV
                // CallCount : 1, Time : 529877 (microseconds) | 529.877 (milliseconds) | 0.529877 (s)
                // 
                // 2. YUV_2_H264
                // CallCount : 1, Time : 5593254 (microseconds) | 5593.254 (milliseconds) | 5.593254 (s)
                // 
                // 3. copy_yuv_data
                // CallCount : 0, Time : 0 (microseconds) | 0.000 (milliseconds) | 0.000000 (s)
                // 
                // 4. cv::cvtColor
                // CallCount : 17, Time : 5593254 (microseconds) | 5593.254 (milliseconds) | 5.593254 (s)
                // 
                // 5. cv::imread
                // CallCount : 17, Time : 340335 (microseconds) | 340.335 (milliseconds) | 0.340335 (s)
                // 调用次数格式化占位宽度 CALL_FORMAT_PLACEHOLDER_NUM = 10
                ostream << ++nIndex << ". " << kv.first << "\n" << "CallCount: " << std::setw(CALL_FORMAT_PLACEHOLDER_NUM) << kv.second.m_ulCallCount << ", Time: ";
                // 耗时数据统一格式化占位宽度 TIME_FORMAT_PLACEHOLDER_NUM = 16
                ostream << std::setw(TIME_FORMAT_PLACEHOLDER_NUM) << kv.second.m_ulTimeMicroSec << " (microseconds) | ";
                ostream.setf(std::ios::fixed);
                ostream.setf(std::ios::showpoint);
                ostream.precision(3); // 小数点后保留3位
                ostream << std::setw(TIME_FORMAT_PLACEHOLDER_NUM) << (kv.second.m_ulTimeMicroSec / 1000.0) << " (milliseconds) | ";
                ostream.precision(6); // 小数点后保留6位
                ostream << std::setw(TIME_FORMAT_PLACEHOLDER_NUM) << (kv.second.m_ulTimeMicroSec / (1000.0 * 1000.0)) << " (s) |";
                ostream.precision(6); // 小数点后保留6位
                ostream << std::setw(TIME_FORMAT_PLACEHOLDER_NUM) << (kv.second.m_ulTimeMicroSec / (1000.0 * 1000.0 * kv.second.m_ulCallCount)) << " (s/n)\n\n";
            }
        }
        return ostream.str();
    }


	/* @fn      OutPutStattisticsRes
	*  @brief   输出接口耗时统计信息到文件
	*  @param   strFile  耗时信息文件全路径
	*  @param   bIsOutputCmd  统计数据是否输出到终端
	*  @return  void
	*/
	void PerfInterface::OutPutStattisticsRes(const std::string& strFile, bool bIsOutputCmd)
	{
		std::ofstream ofs;
        if(strFile.empty() == false){
            try
            {
                ofs.open(strFile, std::ofstream::out | std::ofstream::ate);
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
                return;
            }
        }

		std::string strReport = GenerateReport();
       
        if (bIsOutputCmd)
        {
            // 统计结果输出到终端
            std::cout << strReport << std::flush;
        }


        if(ofs.is_open() == true){
			ofs << strReport << std::flush;
        }

		ofs.close();
	}

	// 性能统计接口
	void StartHit(PerfType nPerfType, const char* strMark)
	{
		if (PERF_SWITCH)
		{
			PerfInterface::Start(nPerfType, strMark);
		}
	}
	void EndHit(PerfType nPerfType, const char* strMark)
	{
		if (PERF_SWITCH)
		{
			PerfInterface::End(nPerfType, strMark);
		}
	}

	// 写出统计结果
	void OutPutReport(const char* pFile, bool bIsOutputCmd)
	{
		if (PERF_SWITCH)
		{
			PerfInterface::OutPutStattisticsRes(pFile == nullptr ? "" : pFile, bIsOutputCmd);
			PerfInterface::Clear();
		}
	}
    std::string OutPutReport()
    {
        std::string strReport;
        if (PERF_SWITCH)
		{
            strReport = PerfInterface::GenerateReport();
			PerfInterface::Clear();
		}
        return strReport;
    }
} // ! namespace PerfTime

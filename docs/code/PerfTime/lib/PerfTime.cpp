#include "PerfTime.h"

#include <map>
#include <mutex>
#include <ctime>
#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <functional>

#include <time.h>

#include "Dataframe.hpp"

// 1. ֧�ֻ��� section ���洢�ṹ map<section, map<itemName, index>>
// 2. ���� Dataframe �洢ÿһ�в�����Ŀ�����ʼʱ�䣬��С��ʼʱ�䣬������ʱ�䣬��С����ʱ�䣬��ʱ�䣬���ô������ϸ��־
// 3. ��ϸ����������Զ��屨�浼��
// 4. ����֧�� csv ����
namespace OwO
{

const std::string SECTION_DEFAULT = "default"; // Ĭ�ϵ� section
const std::string SEP_CSV = ", ";

class Section{
public:
    
    /**
     * @brief ���� mark��ֻ�е�һ����ӲŻ���� section
     * @param strMark 
     * @param vecSection 
     * @return 
     */
    void addMark(const std::string &strMark, const std::vector<std::string> &vecSection,  const std::string & strPos){
        assert(m_pDataTable != nullptr);
        if(m_pDataTable->hasRowAlias(strMark) == true) return;

        size_t uIndex = 0;
        uIndex = m_pDataTable->getRowLen();
        // ���һ��
        m_pDataTable->appendRow(Dataframe::Item(0LL));

        // ��ӱ���
        m_pDataTable->setRowAlias(strMark, uIndex);

        // ���λ��
        (*m_pDataTable)[KW_POSITION][strMark] = strPos;

        // ���section
        if(vecSection.size() == 0){
            // ���Ĭ�� section
            m_mapSection[SECTION_DEFAULT].push_back(strMark);
        }else{
            for(auto & strSection : vecSection){
                m_mapSection[strSection].push_back(strMark);
            }
        }
    }

    void updateMarkStart(const std::string &strMark, int64_t now){
        assert(m_pDataTable != nullptr);
        Dataframe& dataTable = *m_pDataTable;

        // û�б�ǩ���˳�
        if(dataTable.hasRowAlias(strMark) == false) return;

        // ��ȡ��ֵ
        int64_t nMax = dataTable[KW_MAX_START][strMark].Get<int64_t>();
        int64_t nMin = dataTable[KW_MIN_START][strMark].Get<int64_t>();
        int64_t nValid = dataTable[KW_VALID][strMark].Get<int64_t>();
        int64_t nTotal = dataTable[KW_TOTAL_TIME][strMark].Get<int64_t>();
        
        // ����ֵ
        if(now > nMax){
            dataTable[KW_MAX_START][strMark] = now;
        }
        
        if( nMin == 0 || now < nMin){
            dataTable[KW_MIN_START][strMark] = now;
        }

        dataTable[KW_VALID][strMark] = nValid + 1;
        dataTable[KW_TOTAL_TIME][strMark] = nTotal - now;
    }

    void updateMarkEnd(const std::string &strMark, int64_t now){
        if(m_pDataTable == nullptr) return;

        Dataframe& dataTable = *m_pDataTable;

        // û�б�ǩ���˳�
        if(dataTable.hasRowAlias(strMark) == false) return;

        // ��ȡ��ֵ
        int64_t nMax = dataTable[KW_MAX_END][strMark].Get<int64_t>();
        int64_t nMin = dataTable[KW_MIN_END][strMark].Get<int64_t>();
        int64_t nValid = dataTable[KW_VALID][strMark].Get<int64_t>();
        int64_t nCount = dataTable[KW_COUNT][strMark].Get<int64_t>();
        int64_t nTotal = dataTable[KW_TOTAL_TIME][strMark].Get<int64_t>();
        
        // ����ֵ
        if(now > nMax){
            dataTable[KW_MAX_END][strMark] = now;
        }
        
        if(nMin == 0 || now < nMin){
            dataTable[KW_MIN_END][strMark] = now;
        }

        dataTable[KW_COUNT][strMark] = nCount + 1;
        dataTable[KW_VALID][strMark] = nValid - 1;
        dataTable[KW_TOTAL_TIME][strMark] = nTotal + now;
    }

    /* ����̨���� */
    std::string format(const std::string & strMark, int nPlaceHolder, const std::string & strSep){
        assert(m_pDataTable != nullptr);
        if(m_pDataTable->hasRowAlias(strMark) == false) return "null";

        Dataframe& dataTable = *m_pDataTable;

        std::ostringstream os;

        // �ϸ��־λ
        int64_t nValid = dataTable[KW_VALID][strMark].Get<int64_t>();
        os << std::setw(nPlaceHolder) << (nValid == 0? "OK": "FUCK") <<  strSep;

        os << std::setw(nPlaceHolder) << dataTable[KW_COUNT][strMark].Get<int64_t>() <<  strSep;

        // С������
        os.precision(3);
        os.setf(std::ios::fixed);
        os.setf(std::ios::showpoint);
        os << std::setw(nPlaceHolder) << dataTable[KW_MAX_START][strMark].Get<int64_t>() / 1000.0 <<  strSep;
        os << std::setw(nPlaceHolder) << dataTable[KW_MIN_START][strMark].Get<int64_t>() / 1000.0 <<  strSep;
        os << std::setw(nPlaceHolder) << dataTable[KW_MAX_END][strMark].Get<int64_t>() / 1000.0 <<  strSep;
        os << std::setw(nPlaceHolder) << dataTable[KW_MIN_END][strMark].Get<int64_t>() / 1000.0 <<  strSep;
        os << std::setw(nPlaceHolder) << dataTable[KW_TOTAL_TIME][strMark].Get<int64_t>() / 1000.0 <<  strSep;
        os << std::setw(nPlaceHolder) << dataTable[KW_TOTAL_TIME][strMark].Get<int64_t>() / 1000.0 / dataTable[KW_COUNT][strMark].Get<int64_t>() <<  strSep;
        
        // �������
        if(m_fcnOutput){
            auto vecRes = m_fcnOutput(strMark, dataTable);
            for(auto & res : vecRes){
                os << std::setw(nPlaceHolder) << res <<  strSep;
            }
        }

        // ��ǩ��
        os << std::left << dataTable[KW_POSITION][strMark].Get<std::string>() << " " << std::setw(nPlaceHolder) << strMark;
        return os.str();
    }
    
    std::string formatHeader(int nPlaceHolder, const std::string & strSep){
        std::ostringstream os;
        
        os << std::setw(nPlaceHolder) << KW_VALID<<  strSep;
        os << std::setw(nPlaceHolder) << KW_COUNT<<  strSep;
        os << std::setw(nPlaceHolder) << KW_MAX_START <<  strSep;
        os << std::setw(nPlaceHolder) << KW_MIN_START <<  strSep;
        os << std::setw(nPlaceHolder) << KW_MAX_END <<  strSep;
        os << std::setw(nPlaceHolder) << KW_MIN_END <<  strSep;
        os << std::setw(nPlaceHolder) << KW_TOTAL_TIME <<  strSep;
        os << std::setw(nPlaceHolder) << "average" <<  strSep;

        // ��չ��ͷ
        for (auto & header : m_vecExternHeader)
        {
            os << std::setw(nPlaceHolder) << header <<  strSep;
        }

        os << std::left << std::setw(nPlaceHolder) << "mark";
        return os.str();
    }

    void appendHeader(const std::vector<std::string> vec){
        m_vecExternHeader = vec;
    }

    std::string report(int nPlaceHolder,const std::vector<std::string> vecSection = std::vector<std::string>()){
        
        std::string str = "\nPerfTime Report : " + getTime() + "\n";;
        std::string strHeader = formatHeader(nPlaceHolder, SEP_SIGN);
        if(vecSection.size() == 0){
            // ���ȫ��
            for(auto & section : m_mapSection){
                str += section.first + "\n";
                str +=  strHeader + "\n";
                for(auto & mark : section.second){
                    str += format(mark, nPlaceHolder, SEP_SIGN) + "\n"; 
                }
            }
        }else{
            // ���ָ����
            for (auto & section : vecSection)
            {
                str += section + "\n";
                str += strHeader + "\n";
                for(auto & mark : m_mapSection.at(section)){
                    str += format(mark, nPlaceHolder, SEP_SIGN) + "\n";
                }
            }
        }
        return str;
    }

    /* ���� csv ���� */
    void toCSV(const std::string & strPath){
        // �ļ�
        ofstream file(strPath, std::ios::app);
        if(file.is_open() == false) return;

        // ʱ��
        file << "\nPerfTime Report : " << getTime()<< std::endl;

        /* ͷ */
        file << "section"<<  SEP_CSV << formatHeader(0, SEP_CSV) << std::endl;

        /* ���� */
        for(auto & section : m_mapSection){
            for(auto & mark : section.second){
                file << section.first <<  SEP_CSV << format(mark, 0, SEP_CSV) << std::endl; 
            }
        }

        file.close();
    }

    /* ������չ���� */
    void setAppendReport(const std::vector<std::string> & vecHeader, std::function<FcnOutputCallback_t>  fcn){
        m_vecExternHeader = vecHeader;
        m_fcnOutput = fcn;
    }

    void reset(){
        // ���� datatable
        m_pDataTable = std::make_shared<Dataframe>();
        assert(m_pDataTable != nullptr);
        
        // ���ʼʱ�䣬��С��ʼʱ�䣬������ʱ�䣬��С����ʱ�䣬��ʱ�䣬���ô������ϸ��־
        m_pDataTable->appendColumn({KW_MAX_START,KW_MIN_START,KW_MAX_END,KW_MIN_END, KW_TOTAL_TIME, KW_COUNT, KW_VALID, KW_POSITION});

        // ��� section
        m_mapSection.clear();
    }

private:
    std::string getTime(){
        auto t = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now());
        auto timeInfo = std::localtime(&t);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
        return std::string(buffer);
    }

private:
    std::function<FcnOutputCallback_t> m_fcnOutput;
    std::shared_ptr<Dataframe> m_pDataTable; // ������� mark ��Ϣ�ı��
    std::vector<std::string> m_vecExternHeader;
    std::map<std::string, std::vector<std::string>> m_mapSection; // map<section, vector<mark>>
};

class PerfTimeImpl{

public:
    static PerfTimeImpl * instance(){
        if(m_pInstance == nullptr){
            std::lock_guard<std::mutex> clsGuard(m_mutSingle);
            if(m_pInstance == nullptr){
                m_pInstance = new PerfTimeImpl();
                m_pInstance->init();
            }
        }
        return m_pInstance;
    }

    static void release(){
        if(m_pInstance != nullptr){
            std::lock_guard<std::mutex> clsGuard(m_mutSingle);
            if(m_pInstance != nullptr){
                delete m_pInstance;
                m_pInstance = nullptr;
            }
        }
    }


    void startHit(const std::string &strMark, const std::vector<std::string> &vecSection, const std::string & strPos){
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        m_section.addMark(strMark, vecSection, strPos);
        m_section.updateMarkStart(strMark, now() - m_nBaseTime);
    }

    void endHit(const std::string &strMark){
        int64_t time = now() - m_nBaseTime;
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        m_section.updateMarkEnd(strMark, time);
    }

    std::string format(const std::string &strMark){
        std::string str = "";
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        str += m_section.formatHeader(FORMAT_PLACEHOLDER_NUM, SEP_SIGN) + "\n";
        str += m_section.format(strMark, FORMAT_PLACEHOLDER_NUM, SEP_SIGN);
        return str;
    }

    std::string report(const std::vector<std::string> vecSection){
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        return m_section.report(FORMAT_PLACEHOLDER_NUM, vecSection);
    }

    void save(const std::string & strPath){
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        m_section.toCSV(strPath);
    }

    void setAppendReport(const std::vector<std::string> & vecHeader, std::function<FcnOutputCallback_t>  fcn){
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        m_section.setAppendReport(vecHeader, fcn);
    }

    void reset(){
        std::lock_guard<std::mutex> clsGuard(m_mutex);
        init();
    }

    void init(){
        // ���� mark
        m_section.reset();

        // ָ����׼ʱ��
        m_nBaseTime = now();
    }

private:

    // ��ǰʱ�䣬ms
    int64_t now(){
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    }

private:
    static PerfTimeImpl* m_pInstance;
    static std::mutex m_mutSingle;

    std::mutex m_mutex;
    Section m_section; // ���� mark
    int64_t m_nBaseTime = 0; // ����һ����׼ʱ�䣬�ô洢��ʱ��ֵСһЩ
};
PerfTimeImpl* PerfTimeImpl::m_pInstance = nullptr;
std::mutex PerfTimeImpl::m_mutSingle;

} // namespace OwO

std::string OwO::GetPosition(const std::string &strFile, const std::string &strLine)
{
    return strFile + ":" + strLine;
}

void OwO::StartHit(const std::string &strMark,  const std::string & strPos, const std::vector<std::string> &vecSection)
{
    PerfTimeImpl::instance()->startHit(strMark, vecSection, strPos);
}

void OwO::EndHit(const std::string &strMark)
{
    PerfTimeImpl::instance()->endHit(strMark);
}

std::string OwO::Show(const std::string &strMark)
{
    return PerfTimeImpl::instance()->format(strMark);
}

std::string OwO::Report(const std::vector<std::string> &vecSection)
{
    std::string str = PerfTimeImpl::instance()->report(vecSection);
    PerfTimeImpl::instance()->reset();
    return str;
};

std::string OwO::Report(const std::string &strPath, const std::vector<std::string> &vecSection)
{
    std::string str = PerfTimeImpl::instance()->report(vecSection);
    PerfTimeImpl::instance()->save(strPath);
    PerfTimeImpl::instance()->reset();
    return str;
}

void OwO::AppendReport(const std::vector<std::string> & vecHeader, std::function<FcnOutputCallback_t> fcn){
    PerfTimeImpl::instance()->setAppendReport(vecHeader, fcn);
}
#ifndef PERFTIME_H
#define PERFTIME_H

#include <string>
#include <vector>
#include <functional>


namespace OwO
{
    const std::string KW_MAX_START = "max_start_time"; // ���ʼʱ�䣬StartHit ��¼�����п�ʼʱ������ֵ
    const std::string KW_MIN_START = "min_start_time"; // ��С��ʼʱ�䣬StartHit ��¼�����п�ʼʱ�����Сֵ
    const std::string KW_MAX_END = "max_end_time"; // ������ʱ�䣬EndHit ��¼�����п�ʼʱ������ֵ
    const std::string KW_MIN_END = "min_end_time"; // ��С����ʱ�䣬EndHit ��¼�����п�ʼʱ�����Сֵ
    const std::string KW_TOTAL_TIME = "total_time"; // ��ʱ�䣬 (EndHit - StartHit) ��¼��ʱ����ܺ�
    const std::string KW_COUNT = "count"; // ���ô����� (EndHit - StartHit) ���ô���
    const std::string KW_VALID = "valid"; // ��Ч��־��0��EndHit �� StartHit ��ԣ������Ч��������Ч
    const std::string KW_POSITION = "position"; // �������λ��
    const std::string SEP_SIGN = "  "; // �ָ��� 

    const int FORMAT_PLACEHOLDER_NUM = 16;

    // �Զ��屨�����ɻص�����
    class Dataframe;
    using FcnOutputCallback_t = std::vector<double>(const std::string & strMark, const OwO::Dataframe & dataFrame);
} // namespace OwO


#define _TO_STR(x)  (#x)
#define _MACRO_STR(x) _TO_STR(x)

/* ��ȡԴ�ļ��� */
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)

/* ��ȡλ�ã����� test.cpp:100 */
#define OwO_POS OwO::GetPosition(__FILENAME__, _MACRO_STR(__LINE__))

/* �Զ���� OwO_POS */
#define OwO_START(strMark,...) OwO::StartHit(strMark, OwO_POS , ##__VA_ARGS__)
#define OwO_END(strMark) OwO::EndHit(strMark)
#define OwO_BLOCK_MARK(strMark,...) OwO::HitBlock __clsBlock(strMark, OwO_POS , ##__VA_ARGS__)
#define OwO_BLOCK(strMark,...)  for(int __i = (OwO::HitBlock(strMark, OwO_POS , ##__VA_ARGS__),1); __i == 1 ; __i--)


namespace OwO
{
    std::string GetPosition(const std::string & strFile, const std::string & strLine);

    /**
     * @brief ��ǵ㿪ʼ��ʱ
     * @param strMark �������
     * @param strPos �������
     * @param vecSection ��Ƕ�Ӧ�ķ��飬Ĭ�Ϸ��� default
     */
    void StartHit(const std::string & strMark, const std::string & strPos, const std::vector<std::string> & vecSection = std::vector<std::string>());
    
    /**
     * @brief ��ǵ��ʱ����
     * @param strMark �������
     */
    void EndHit(const std::string & strMark);

    /**
     * @brief �Զ��屨�������
     * @param vecHeader �Զ�������
     * @param fcn �Զ�������ص�
     */
    void AppendReport(const std::vector<std::string> & vecHeader, std::function<FcnOutputCallback_t> fcn);

    /**
     * @brief ���ָ�����
     * @param strMark �����
     * @return ��Ƕ�Ӧ����
     */
    std::string Show(const std::string & strMark);

    /**
     * @brief �������ɡ��������ʷ����
     * @param vecSection ��Ҫ������顣����ָ����Ĭ�����ȫ����
     * @return ����
     */
    std::string Report(const std::vector<std::string> & vecSection = std::vector<std::string>());

    /**
     * @brief �������ɡ������ȫ����ʷ����
     * @param strPath ���� csv ��·��
     * @param vecSection ��Ҫ������顣����ָ����Ĭ�����ȫ����
     * @return ����
     */
    std::string Report(const std::string & strPath, const std::vector<std::string> & vecSection = std::vector<std::string>() );

    /**
     * @brief ��ǿ飬�Զ�ά����ǵĿ�ʼ�ͽ���
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

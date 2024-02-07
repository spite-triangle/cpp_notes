#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <map>
#include <vector>
#include <string>

#undef NDEBUG // ǿ�������쳣���
#include <assert.h>

#include "Variant.hpp"

namespace OwO{

class Dataframe{
public:
    // NOTE - ��֧��ָ�룬�����ͱ���֧���ƶ��뿽��
    using Item = Variant<int,double,float,bool,int64_t,size_t, std::string>;

    // ��
    class Column{
    public:
        Column(Dataframe * pDataFrame, const std::string & strColName,size_t uSize)
        : m_pDataFrame(pDataFrame),m_vecItem(uSize),m_strName(strColName)
        {
        }

        Item & operator[](size_t index){
            assert(checkBoundary(index) == true);
            return m_vecItem.at(index);
        }

        Item & operator[](const std::string & strIndex){
            assert(m_pDataFrame != nullptr);
            assert(m_pDataFrame->m_mapRowAlias.count(strIndex) > 0);
            return operator[](m_pDataFrame->m_mapRowAlias.at(strIndex));
        }

        // ������ֵ
        void operator=(const std::vector<Item> & items){
            m_vecItem.clear();

            m_vecItem.insert(m_vecItem.end(), items.begin(), items.end());

            assert(m_pDataFrame != nullptr);
            m_pDataFrame->updateColumnSize(m_strName);
        }

        void operator=(std::vector<Item> && items){
            m_vecItem.clear();

            for(auto & item : items){
                m_vecItem.emplace_back(std::move(item));
            }

            assert(m_pDataFrame != nullptr);
            m_pDataFrame->updateColumnSize(m_strName);
        }

        void operator=( std::vector<std::string> && items){
            m_vecItem.clear();

            for(auto & item : items){
                m_vecItem.emplace_back(std::move(item));
            }

            assert(m_pDataFrame != nullptr);
            m_pDataFrame->updateColumnSize(m_strName);
        }

        void operator=(const std::vector<std::string> & items){
            m_vecItem.clear();

            for(auto & item : items){
                m_vecItem.emplace_back(std::move(item));
            }

            assert(m_pDataFrame != nullptr);
            m_pDataFrame->updateColumnSize(m_strName);
        }

        // Ԫ�����
        void append(const Item & item){
            m_vecItem.push_back(item);

            assert(m_pDataFrame != nullptr);
            m_pDataFrame->updateColumnSize(m_strName, item);
        }

        void append(Item && item){
            m_vecItem.emplace_back(std::move(item));

            assert(m_pDataFrame != nullptr);
            m_pDataFrame->updateColumnSize(m_strName, m_vecItem.back());
        }

        // �޸Ĵ�С
        void resize(size_t uSize, const Item & item = Item()){
            m_vecItem.resize(uSize, item);
        }

        size_t size(){
            return m_vecItem.size();
        }

        // ������
        std::vector<Item>::iterator & begin(){
            return m_vecItem.begin();
        }

        std::vector<Item>::iterator & end(){
            return m_vecItem.end();
        }

        // չʾ
        std::string format(){
            std::string str = m_strName + ":";
            for(auto & item : m_vecItem){
                str += item.ToString() + ",";
            }
            // �������һ�� ����
            str.pop_back();
            return "{" + str + "}";
        }


    private:

        /* ����Ƿ�Խ�� */
        bool checkBoundary(size_t index){
            return m_vecItem.size() > index ? true : false;
        }

    private:
        std::vector<Item> m_vecItem; // ������
        std::string m_strName; // ����
        Dataframe * m_pDataFrame;
    };

public:
    Dataframe() = default;

    /* ��ֹ�������ƶ� */
    Dataframe(Dataframe && ) = delete;
    Dataframe(const Dataframe & ) = delete;
    void operator=(Dataframe &&) = delete;
    void operator=(const Dataframe &) = delete;

    /* ������ */        
    Column & operator[](const std::string & strCol){
        appendColumn(strCol);
        return m_mapTable.at(strCol);
    }

    void appendColumn(const std::vector<std::string> & vecColumns){
        for(auto & strCol : vecColumns){
            appendColumn(strCol);
        }
    }

    void appendColumn(const std::string & strCol){
        if(m_mapTable.count(strCol) <= 0){
            Column clsCol(this, strCol,m_uRowCount);
            m_mapTable.insert(std::pair<std::string, Column>(std::move(strCol), std::move(clsCol)));
            // ������ӵ�˳��洢
            m_vecHeader.push_back(strCol);
        }
    }


    /* ������ */
    /**
     * @brief ���� column �����˳���������
     * @param vecItem 
     */
    void appendRow(const std::vector<Item> & vecItem){
        assert( vecItem.size() == m_vecHeader.size());

        for (size_t i = 0; i < m_vecHeader.size(); i++)
        {
            m_mapTable.at(m_vecHeader.at(i)).append(vecItem[i]);
        }
    }

    /**
     * @brief ��ӿ���
     */
    void appendRow(const Item item ){
        // ���һ���������л�ͬ������
        if(m_mapTable.size() >  0){
            m_mapTable.begin()->second.append(item);
        }
    }

    /* ��ȡ���� */
    size_t getRowLen(){
        return m_uRowCount;
    }

    /* �����б��� */
    void setRowAlias(const std::string & strName, size_t uIndex){
        assert(uIndex < m_uRowCount);
        m_mapRowAlias[strName] = uIndex;
    }

    bool hasRowAlias(const std::string & strName){
        return m_mapRowAlias.count(strName) <= 0 ? false : true;
    }

    size_t getIndexFromAlias(const std::string & strName){
        return m_mapRowAlias.at(strName);
    }

    /* ��ͷ */
    std::vector<std::string> getHeader(){
        return m_vecHeader;
    }

private:

    // ���������еĳ���
    void updateColumnSize(const std::string & strCurCol, const Item & item = Item()){
        size_t uTargetSize = m_mapTable.at(strCurCol).size();

        if(uTargetSize < m_uRowCount){
            m_mapTable.at(strCurCol).resize(m_uRowCount);
        }
        else if(uTargetSize > m_uRowCount){
            // ��������
            m_uRowCount = uTargetSize;

            for (auto & col : m_mapTable)
            {
                // ��ǰ�Ĳ�����
                if(col.first == strCurCol) continue;

                // ׷����
                col.second.resize(m_uRowCount, item);
            }
        }
    }

private:
    size_t m_uRowCount = 0; // ��������
    std::vector<std::string> m_vecHeader; // ��ͷ
    std::map<std::string, Column> m_mapTable; // ���
    std::map<std::string, size_t> m_mapRowAlias;
};

}; // namespace OwO;


#endif /* DATAFRAME_HPP */

#ifndef DATAFRAME_HPP
#define DATAFRAME_HPP

#include <map>
#include <vector>
#include <string>

#undef NDEBUG // 强制启用异常检测
#include <assert.h>

#include "Variant.hpp"

namespace OwO{

class Dataframe{
public:
    // NOTE - 不支持指针，且类型必须支持移动与拷贝
    using Item = Variant<int,double,float,bool,int64_t,size_t, std::string>;

    // 列
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

        // 批量赋值
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

        // 元素添加
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

        // 修改大小
        void resize(size_t uSize, const Item & item = Item()){
            m_vecItem.resize(uSize, item);
        }

        size_t size(){
            return m_vecItem.size();
        }

        // 迭代器
        std::vector<Item>::iterator & begin(){
            return m_vecItem.begin();
        }

        std::vector<Item>::iterator & end(){
            return m_vecItem.end();
        }

        // 展示
        std::string format(){
            std::string str = m_strName + ":";
            for(auto & item : m_vecItem){
                str += item.ToString() + ",";
            }
            // 弹出最后一个 符号
            str.pop_back();
            return "{" + str + "}";
        }


    private:

        /* 检测是否越界 */
        bool checkBoundary(size_t index){
            return m_vecItem.size() > index ? true : false;
        }

    private:
        std::vector<Item> m_vecItem; // 列数据
        std::string m_strName; // 列名
        Dataframe * m_pDataFrame;
    };

public:
    Dataframe() = default;

    /* 禁止拷贝和移动 */
    Dataframe(Dataframe && ) = delete;
    Dataframe(const Dataframe & ) = delete;
    void operator=(Dataframe &&) = delete;
    void operator=(const Dataframe &) = delete;

    /* 增加列 */        
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
            // 按照添加的顺序存储
            m_vecHeader.push_back(strCol);
        }
    }


    /* 增加行 */
    /**
     * @brief 按照 column 的添加顺序，添加数据
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
     * @brief 添加空行
     */
    void appendRow(const Item item ){
        // 添加一个，其他列会同步更新
        if(m_mapTable.size() >  0){
            m_mapTable.begin()->second.append(item);
        }
    }

    /* 获取行数 */
    size_t getRowLen(){
        return m_uRowCount;
    }

    /* 设置行别名 */
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

    /* 表头 */
    std::vector<std::string> getHeader(){
        return m_vecHeader;
    }

private:

    // 更新其他列的长度
    void updateColumnSize(const std::string & strCurCol, const Item & item = Item()){
        size_t uTargetSize = m_mapTable.at(strCurCol).size();

        if(uTargetSize < m_uRowCount){
            m_mapTable.at(strCurCol).resize(m_uRowCount);
        }
        else if(uTargetSize > m_uRowCount){
            // 更新行数
            m_uRowCount = uTargetSize;

            for (auto & col : m_mapTable)
            {
                // 当前的不增加
                if(col.first == strCurCol) continue;

                // 追加行
                col.second.resize(m_uRowCount, item);
            }
        }
    }

private:
    size_t m_uRowCount = 0; // 行数计数
    std::vector<std::string> m_vecHeader; // 表头
    std::map<std::string, Column> m_mapTable; // 表格
    std::map<std::string, size_t> m_mapRowAlias;
};

}; // namespace OwO;


#endif /* DATAFRAME_HPP */

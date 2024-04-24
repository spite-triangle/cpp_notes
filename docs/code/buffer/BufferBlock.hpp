#ifndef DEFINEBUFFER_HPP
#define DEFINEBUFFER_HPP

#include <memory>

namespace OwO
{
    /* �ڴ����ģ�� */
    class BufferBlock{
    public:
        BufferBlock()
            : m_uLen(0), m_uCapacity(0)
        {}

        /* ��ȡ buffer ��Ϣ*/
        size_t len() const {return m_uLen;} 
        size_t useCount() const {return m_data.use_count();}
        std::shared_ptr<char> getData() const {return m_data;}
        char* data(long long offset = 0) const{
            if(offset >= m_uLen) return nullptr; 
            return m_data.get() + offset;
        }

        /* ���� buffer */
        static BufferBlock mallocBuffer(size_t uLen){
            BufferBlock buffer;
            auto pBuffer = new char[uLen]();
            if(pBuffer == nullptr) return buffer;

            buffer.m_uLen = uLen;
            buffer.m_uCapacity = uLen;
            buffer.m_data = std::shared_ptr<char>(pBuffer, BufferBlock::bufferRelease);
            return buffer;
        }

        /* �����ڴ� */
        bool reset(size_t uLen = 0){
            if(uLen > m_uCapacity){
                freeBuffer();
                auto buffNew = mallocBuffer(uLen);
                if(buffNew == nullptr) return false;

                m_uLen = buffNew.m_uLen;
                m_uCapacity = buffNew.m_uCapacity;
                m_data = buffNew.m_data;
            }else{
                m_uLen = uLen;
                memset(m_data.get(), 0, m_uCapacity);
            }
            return true;
        }

        /* �ͷ� buffer */
        void freeBuffer(){
            m_uLen = 0;
            m_uCapacity = 0;
            m_data.reset();
        }

        /* �� nullptr �Ƚ� */
        bool operator==(std::nullptr_t) const noexcept{
            return (m_uLen <= 0 || m_data == nullptr);
        }
        bool operator!=(std::nullptr_t) const noexcept{
            return (m_uLen > 0 && m_data != nullptr);
        }

        /* ����ת�� */
        template<class Type,class PureType = typename std::decay<Type>::type>
        PureType * convert(const size_t & offset = 0) const {
            if(operator==(nullptr) || m_uLen < offset + sizeof(PureType)) return nullptr;
            return reinterpret_cast<PureType*>(m_data.get() + offset);
        }

        /* ����ֵ */
        template<class Type>
        bool updateValue(Type value, const size_t & offset = 0){
            using PureType = typename std::decay<Type>::type;
            PureType* pDest = convert<PureType>(offset);
            if(pDest == nullptr) return false;
            *pDest = value;
            return true;
        }

        /* ����ֵ */
        template<class Type>
        bool setValue(Type value, size_t & cursor){
            using PureType = typename std::decay<Type>::type;
            PureType* pDest = convert<PureType>(cursor);
            if(pDest == nullptr) return false;
            *pDest = value;
            cursor += sizeof(PureType);
            return true;
        }

        /* ��ȡֵ */
        template<class Type, class PureType = typename std::decay<Type>::type>
        PureType* getValue(size_t & cursor) const {
            PureType* pDest = convert<PureType>(cursor);
            if(pDest != nullptr) cursor += sizeof(PureType);
            return pDest;
        }

    private:
        /* ָ���ͷ� */
        static void bufferRelease(char data[]){
            if(data != nullptr){
                delete [] data;
                data = nullptr;
            }
        }

    private:
        size_t m_uLen; // ��ǰҪʹ�õĳ���
        size_t m_uCapacity;  // ʵ��������ڴ泤��
        std::shared_ptr<char> m_data; // ת���������
    }; 
} // namespace OwO


#endif /* DEFINEBUFFER_HPP */

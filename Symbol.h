#ifndef _Symbol_h_
#define _Symbol_h_

typedef unsigned char Byte;

#include <string>

/*
 typedef unsigned char Symbol;
inline std::string toString(const Symbol& c)
{
    char sBuffer[2];
    sBuffer[0] = c;
    sBuffer[1] = 0;
    return std::string(sBuffer);
}
*/

class Symbol
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

        Symbol( )
            :  m_data(0)
        { }

        Symbol(const Byte c)
            :   m_data(c)
        { }

        Symbol(const Symbol& other)
            :   m_data(other.m_data)
        { }

        Symbol& operator=(const Symbol& other)
        { m_data = other.m_data; return *this; }

        bool operator==(const Symbol& other)const
        { return m_data == other.m_data; }

        int32_t toInt()const
        { return m_data; }

        std::string toString()const
        {
            char sBuffer[2];
            sBuffer[0] = m_data;
            sBuffer[1] = 0;
            return std::string(sBuffer);
        }

///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

        Byte                    m_data                                        ;
};

inline std::string toString(const Symbol& s)
{ return s.toString(); }

inline uint32_t toInt(const Symbol& s)
{ return s.toInt(); }



#endif


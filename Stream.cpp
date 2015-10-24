#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#ifdef WIN32
#include <winsock2.h>
#include <malloc.h>
#define alloca _alloca
#else
#include <netinet/in.h>
#endif

#include "MemBuffer.h"
#include "Stream.h"

///////////////////////////////////////////////////////////////////////////////

std::string _print(const char* sMsg, ...)
{
    const size_t iSize = 2048;
    char* pBuffer = new char[iSize];

    va_list args;
    va_start(args, sMsg);
    vsnprintf(pBuffer, iSize, sMsg, args);
    va_end(args);

    std::string s(pBuffer);
    delete[] pBuffer;
    return s;
}

///////////////////////////////////////////////////////////////////////////////

Stream::Stream( )
    :   m_file(0)
    ,   m_ownFileHander(false)
    ,   m_error(false)
    ,   m_inEOF(false)
    ,   m_streamIO(StreamIONone)
    ,   m_mode(StreamNone)
{
}

///////////////////////////////////////////////////////////////////////////////

Stream::~Stream( )
{
}

///////////////////////////////////////////////////////////////////////////////

bool  Stream::open(FILE* p, StreamIO io)
{
    m_file      = p;
    m_streamIO  = io;
    m_mode      = StreamFile;
    return true;

}

///////////////////////////////////////////////////////////////////////////////

bool  Stream::open(MemBuffer* p, StreamIO io)
{
    m_memBuffer = p;
    m_streamIO  = io;
    m_mode      = StreamMemory;
    return (m_memBuffer) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::open(std::string fname, StreamIO io)
{
    bool bOk = true;
    m_streamIO  = io;
    m_mode      = StreamFile;

    if(!fname.empty())
    {
        m_file = (io == StreamRead)
                            ? fopen(fname.c_str(), "rb")
                            : fopen(fname.c_str(), "wb");
        if(!m_file)
        {
            fprintf(stderr, "Error: Can't open input file '%s': %s\n",  fname.c_str(), strerror(errno));
            bOk =  false;
        }
        else
        {
            m_ownFileHander = true;
        }
    }
    else
    {
        m_file = (io == StreamRead)
                         ? stdin
                         : stdout;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

void Stream::close( )
{
    if(m_ownFileHander)
    {
        fclose(m_file);
        m_file = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::write(uint32_t uint32)
{
    bool bOk = true;
    if(checkIO(StreamWrite))
    {
        if(m_mode == StreamFile)
        {
            // Write the number of in network byte order.
            uint32 = htonl(uint32);
            if(fwrite(&uint32, sizeof(uint32_t), 1, m_file) != 1)
            {
                m_error = true;
                fprintf(stdout, "ERROR: cannot write to file\n");
                bOk = false;
            }
        }
        else // StreamMemory
        {
            bOk = m_memBuffer->write(uint32);
        }
    }
    else
    {
        m_error = true;
        bOk = false;
        fprintf(stdout, "ERROR: file is not set\n");
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::writeSymbol(const Symbol& s)
{
    bool bOk = true;
    if(checkIO(StreamWrite))
    {
        Byte b = (Byte)s.toInt();
        if(m_mode == StreamFile)
        {
            fputc(b, m_file);
        }
        else // StreamMemory
        {
            bOk = m_memBuffer->writeSymbol(s);
        }
    }
    else
    {
        m_error = true;
        bOk = false;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::writeByte(const Byte b)
{
    bool bOk = true;
    if(checkIO(StreamWrite))
    {
        if(m_mode == StreamFile)
        {
            fputc(b, m_file);
        }
        else // StreamMemory
        {
            bOk = m_memBuffer->writeByte(b);
        }
    }
    else
    {
        m_error = true;
        bOk = false;
    }
    return bOk;
}


///////////////////////////////////////////////////////////////////////////////

bool Stream::write(const Byte* array, size_t size)
{
    bool bOk = true;
    if(checkIO(StreamWrite))
    {
        if(m_mode == StreamFile)
        {
            if(fwrite(array, 1, size, m_file) != size)
            {
                m_error = true;
                fprintf(stdout, "ERROR: cannot write to file\n");
                bOk = false;
            }
        }
        else // StreamMemory
        {
            bOk = m_memBuffer->write(array, size);
        }
    }
    else
    {
        m_error = true;
        fprintf(stdout, "ERROR\n");
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

Byte Stream::readByte( )
{
    if(!checkIO(StreamRead)) m_error = true;
    Byte b = 0;
    if(m_file)
    {
        if(m_mode == StreamFile)
        {
            int c = fgetc(m_file);
            if(c == EOF)
            {
                m_inEOF = true;
            }
            b = (Byte)c;
        }
        else // StreamMemory
        {
            b= m_memBuffer->readByte();
        }
    }
    return b;
}

///////////////////////////////////////////////////////////////////////////////

Symbol Stream::readSymbol( )
{
    Byte b = 0;
    if(!checkIO(StreamRead))
    {
       fprintf(stdout, "error: Wrong Stream. It should be read only.\n");
        m_error = true;
    }
    else
    {
       if(m_mode == StreamFile && m_file)
        {
            int c = fgetc(m_file);
            if(c == EOF)
            {
                m_inEOF = true;
            }
            else
            {
                b = (Byte)(c);
            }
        }
        else // StreamMemory
        {
           b = m_memBuffer->readSymbol();
        }
    }
    return Symbol(b);
}

///////////////////////////////////////////////////////////////////////////////

uint32_t Stream::read( )
{
    uint32_t uint32 = 0;
    if(checkIO(StreamRead))
    {
        if(m_mode == StreamFile)
        {
            // Read the number of entries
            if(fread(&uint32, sizeof(uint32), 1, m_file) != 1)
            {
                m_error = true;
                assert(0);
                //return nullptr;
            }
            else
            {
                uint32 = ntohl(uint32);
            }
        }
        else // StreamMemory
        {
            uint32 = m_memBuffer->read();
        }
    }
    return uint32;
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::read(unsigned char* buffer, const size_t size)
{
    bool bOk = true;
    if(checkIO(StreamRead))
    {
        if(m_mode == StreamFile)
        {
            if(fread(buffer, 1, size, m_file) != size)
            {
                m_error = true;
                bOk = false;
            }
        }
        else // StreamMemory
        {
            bOk = m_memBuffer->read(buffer, size);
        }
    }
    else
    {
        bOk = false;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

void Stream::rewind( )
{
    if(checkIO(StreamRead))
    {
        if(m_mode == StreamFile)
        {
            ::rewind(m_file);
        }
        else // StreamMemory
        {
            m_memBuffer->rewind();
        }
        m_inEOF = false;
    }
    else
    {
        m_error = true;
    }
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::inError( )
{
    return m_error;
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::isOpen( )
{
    if(m_mode == StreamFile)
    {
        return (m_file && feof(m_file) == 0 && !m_inEOF) ? true : false;
    }
    else
    {
        return m_memBuffer->isOpen();
    }
}

///////////////////////////////////////////////////////////////////////////////

void Stream::printf(const std::string& buffer, bool eof)
{
    if(checkIO(StreamWrite))
    {
        if(m_mode == StreamFile)
        {
            fprintf(m_file, "%s", buffer.c_str());
            if(eof)
            {
                fprintf(m_file, "\n");
            }
        }
        else // StreamMemory
        {
            m_memBuffer->write((Byte*)buffer.c_str(), buffer.size());
            if(eof)
            {
                const char* s = "\n";
                m_memBuffer->write((Byte*)s, 2);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

Stream& Stream::operator<<(const std::string& msg)
{
    if(checkIO(StreamWrite))
    {
        if(m_mode == StreamFile)
        {
            fprintf(m_file, "%s", msg.c_str());
        }
        else // StreamMemory
        {
            m_memBuffer->write((Byte*)msg.c_str(), msg.size());
        }
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

Stream& Stream::operator<<(const char* msg)
{
    if(checkIO(StreamWrite))
    {
        if(m_mode == StreamFile)
        {
            fprintf(m_file, "%s", msg);
        }
        else // StreamMemory
        {
            size_t iNb = strlen(msg);
            m_memBuffer->write((Byte*)msg, iNb);
        }
    }
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

bool Stream::checkIO(StreamIO mode)
{
    if(m_mode == StreamFile)
    {
        return (m_file && mode==m_streamIO);
    }
    else
    {
        return (m_memBuffer && mode==m_streamIO);
    }
}




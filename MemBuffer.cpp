#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Symbol.h"
#include "MemBuffer.h"

static size_t g_sizeUINT32 = sizeof(uint32_t);
static size_t g_sizeBYTE   = sizeof(Byte);

MemBuffer::MemBuffer( )
    :   m_buffer(0)
    ,   m_currentPos(0)
    ,   m_bufferSize(0)
    ,   m_error(false)
{
}

///////////////////////////////////////////////////////////////////////////////

MemBuffer::~MemBuffer( )
{
    delete[] m_buffer;
}

///////////////////////////////////////////////////////////////////////////////

size_t MemBuffer::size( )const
{
    return m_dataSize;
}

///////////////////////////////////////////////////////////////////////////////

Byte* MemBuffer::buffer( )const
{
    return m_buffer;
}

///////////////////////////////////////////////////////////////////////////////

void MemBuffer::create(size_t size)
{
   if(m_buffer) free(m_buffer);
    m_buffer = size>0 ? (Byte*)malloc(sizeof(Byte)*size) : 0;
    m_bufferSize = size;
    m_dataSize = 0;
    m_currentPos = 0;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::readFile(std::string fname)
{
    bool bOk = true;
    FILE* p = fopen(fname.c_str(), "r");
    if(!p)
    {
        fprintf(stdout, "Error: cannot read file '%s'\n", fname.c_str());
        bOk = false;
    }
    else
    {
        fseek(p, 0, SEEK_END);
        size_t size = ftell(p);
        ::rewind(p);
        m_buffer = (Byte*)malloc(g_sizeBYTE*size);
        fread(m_buffer, size, 1, p);
        m_dataSize   = size-1;
        m_bufferSize = size-1;
        m_currentPos = 0;
        m_error = false;
        fclose(p);
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::writeFile(std::string fname)
{
    bool bOk = true;
    FILE* p = fname.empty() ? stdout: fopen(fname.c_str(), "w");
    if(!p)
    {
        fprintf(stdout, "Error: cannot write file '%s'\n", fname.c_str());
        bOk = false;
    }
    else
    {
        fwrite(m_buffer, m_currentPos, 1, p);
        if(!fname.empty()) fclose(p);
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::write(uint32_t uint32)
{
    bool bOk = false;
    if(canWrite(g_sizeUINT32))
    {
        uint32 = htonl(uint32);
        memcpy(getBuffer(), &uint32, g_sizeUINT32);
        bOk =  shiftBuffer(g_sizeUINT32);
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::writeSymbol(const Symbol& s)
{
    bool bOk = false;
    size_t size = sizeof(Symbol);
    if(canWrite(size))
    {
        Byte b = (Byte)s.toInt();
        memcpy(getBuffer(), &b, size);
        bOk = shiftBuffer(size);
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::writeByte(const Byte b)
{
    bool bOk = false;
    if(canWrite(g_sizeBYTE))
    {
        memcpy(getBuffer(), &b, g_sizeBYTE);
        bOk = shiftBuffer(g_sizeBYTE);
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::write(const Byte* array, size_t size)
{
    bool bOk = false;
    if(canWrite(g_sizeBYTE*size))
    {
        memcpy(getBuffer(), array, size);
        bOk = shiftBuffer(g_sizeBYTE*size);
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

Byte* MemBuffer::getBuffer( )const
{
    Byte* ret = (Byte*)(m_buffer + m_currentPos);
    assert(ret);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::shiftBuffer(size_t size)
{
    m_currentPos += size;
    return (m_currentPos>m_bufferSize) ? false : true;
}

///////////////////////////////////////////////////////////////////////////////

Byte MemBuffer::readSymbol( )
{
    Byte b = 0;
    if(canRead(g_sizeBYTE))
    {
        b = *getBuffer();
        shiftBuffer(g_sizeBYTE);
    }
    return b;
}

///////////////////////////////////////////////////////////////////////////////

Byte MemBuffer::readByte( )
{
    Byte b = 0;
    if(canRead(g_sizeBYTE))
    {
        b = *getBuffer();
        shiftBuffer(g_sizeBYTE);
    }
    else
    {
        fprintf(stdout, "Error: cannot read buffer.\n");

    }
    return b;
}

///////////////////////////////////////////////////////////////////////////////

uint32_t MemBuffer::read( )
{
    uint32_t uint32 = 0;
    if(canRead(g_sizeUINT32))
    {
        memcpy(getBuffer(), &uint32, sizeof(uint32_t));
        shiftBuffer(g_sizeUINT32);
    }
    else
    {
        fprintf(stdout, "Error: cannot read buffer.\n");
    }
    return uint32;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::read(Byte* array, const size_t size)
{
    bool bOk = false;
    if(canRead(g_sizeBYTE*size))
    {
        memcpy(getBuffer(), array, g_sizeBYTE*size);
        shiftBuffer(g_sizeBYTE*size);
        bOk = true;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

void MemBuffer::rewind( )
{
    m_currentPos = 0;
    m_error = false;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::isOpen( )const
{
    return !m_error ? true : false;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::canWrite(size_t size)
{
    bool bOk = (m_currentPos+size<=(m_bufferSize)) ? true : false;
    if(!bOk)
    {
        m_error = true;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool MemBuffer::canRead(size_t size)
{
    bool bOk =  (m_currentPos+size-1<=m_dataSize) ? true : false;
    if(!bOk)
    {
        m_error = true;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////



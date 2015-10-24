#ifndef _Stream_h_
#define _Stream_h_

#include <string>
#include <stdio.h>
#include <stdint.h>

#include "Symbol.h"

class MemBuffer;

std::string _print(const char* msg, ...);

enum StreamIO
{
        StreamIONone
    ,   StreamWrite
    ,   StreamRead
};

enum StreamMode
{
        StreamNone
    ,   StreamMemory
    ,   StreamFile
};

////
//// \brief The Stream class represents an I/O stream. Internally, it can read
///   write data from a file or memory, depending of the constructor.
/// Example:
/// Stream(buffer, size, io): creates from memory
/// Stream(FILE*  io)        : creates from a pre-existent file
/// Stream(fname, io)        : open/close the file internally. if fname is empty,
///                            it will use stdout/stdin.

class Stream
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

                     Stream      ( )                                         ;
                    ~Stream      ( )                                         ;

    //////////////////////////////////////////////////////////////////////////

     bool            open        (const std::string s, StreamIO io)          ;
     bool            open        (FILE* p,      StreamIO io)                 ;
     bool            open        (MemBuffer* p, StreamIO io)                 ;

    //////////////////////////////////////////////////////////////////////////

    void            close        ( )                                         ;

    //////////////////////////////////////////////////////////////////////////

    bool            writeSymbol  (const Symbol& b)                           ;
    Symbol          readSymbol   ( )                                         ;

    bool            writeByte    (const Byte i)                              ;
    Byte            readByte     ( )                                         ;

    bool            write        (uint32_t b)                                ;
    uint32_t        read         ( )                                         ;

    bool            write        (const unsigned char* array, size_t size)    ;
    bool            read         (unsigned char* array, const size_t size)    ;

    void            rewind       ( )                                          ;
    bool            inError      ( )                                          ;
    bool            isOpen       ( )                                          ;

    StreamIO        mode         ( )                                     const
    { return m_streamIO; }

    void                printf              (const std::string& sMsg,
                                             bool eof=true)                   ;

    Stream&             operator<<          (const std::string& msg)          ;
    Stream&             operator<<          (const char*        msg)          ;

///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

    bool                        checkIO                 (StreamIO mode)      ;
    FILE*                       m_file                                       ;
    bool                        m_ownFileHander                              ;
    bool                        m_error                                      ;
    bool                        m_inEOF                                      ;
    StreamIO                    m_streamIO                                   ;
    StreamMode                  m_mode                                       ;
    MemBuffer*                  m_memBuffer                                  ;

};


#endif


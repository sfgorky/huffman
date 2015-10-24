#ifndef _MemBuffer_h_
#define _MemBuffer_h_

#include "Symbol.h"

class MemBuffer
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

                    MemBuffer               ( )                               ;
                   ~MemBuffer               ( )                               ;

    size_t          size                    ( )                         const ;
    Byte*           buffer                  ( )                         const ;
    
    /// \brief create Allocates the internal buffer with a given size
    /// \param size size_t size of the buffer to be allocated
    void            create                  (size_t size)                     ;


    /// \brief rewind rewinds the reading buffer, and sets is reading pointer
    /// at the beginning
    void            rewind                  ( )                               ;
    
    // returns true if the memory buffer is open, This means that it is allowed
    // to read/write into it.
    ///
    /// \brief isOpen Indicates if the 'buffer' is opens frr read/write.
    /// \return
    ///
    bool            isOpen                  ( )                         const ;

    // reads the given file into the buffer. It will be resized to read the 
    // entire file contents
    bool            readFile                (std::string fname)               ;
    
    // writes the current contents of the buffer to file. 
    bool            writeFile               (std::string fname)               ;

    // write the 'uint32' data to the current buffer position, and reposition
    // the reading location.
    // returns true/false if success.
    bool            write                   (uint32_t uint32)                 ;
    bool            writeByte               (const Byte b)                    ;
    bool            write                   (const Byte* array, size_t size)  ;
    bool            writeSymbol             (const Symbol& s)                 ;

    ///////////////////////////////////////////////////////////////////////////

    Byte            readSymbol              ( )                               ;
    Byte            readByte                ( )                               ;
    uint32_t        read                    ( )                               ;
    bool            read                    (Byte* array, const size_t size)  ;

///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

    /// \brief canWrite true if 'size' bytes can be written to buffer
    /// \param size
    /// \return true/false on success.
    bool                 canWrite          (size_t size)                     ;

    ///
    /// \brief canRead true if 'size' bytes can be readen from buffer.
    /// \param size
    /// \return true/false on success.
    bool                 canRead           (size_t size)                    ;

    ///
    /// \brief getBuffer Valid poiter to the actual buffer.
    /// \return
    Byte*                 getBuffer         ( )                      const ;

    ///
    /// \brief shiftBuffer shifts the current pointer by 'size' bytes. In case
    ///                    of error, the internal flag is set, and isOpen
    ///                    return returns false.
    /// \param size
    /// \return true/false on success.
    ///
    bool                  shiftBuffer       (size_t size)                   ;

    Byte*                 m_buffer                                          ;
    size_t                m_currentPos                                      ;
    size_t                m_bufferSize                                      ;
    size_t                m_dataSize                                        ;
    bool                  m_error                                           ;
};


#endif


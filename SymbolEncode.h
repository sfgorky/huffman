#include <vector>
#include "HuffmanCode.h"
#include "Stream.h"
#include "Symbol.h"

class SymbolEncoder
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

                            SymbolEncoder       ( )                           ;
                           ~SymbolEncoder       ( )                           ;

    HuffmanCode*            operator[]          (const size_t i)              ;
    const HuffmanCode*      operator[]          (const size_t i)        const ;

    size_t                  size                ( )                     const ;

    // total number of Symbols in this encoder.
    size_t                  nbSymbol            ( )                     const ;


    // builds a SymbolEncoder by walking down to the leaves of the Huffman tree
    // and for each leaf, determines its code.
    void                    buildSymbolEncoder  (const HuffmanNode* subtree)   ;


    ////////////////////////////////////////////////////////////////////////////
    // Write the huffman code table. The format is:
    //
    // 4 byte code count in network byte order.
    // 4 byte number of bytes encoded
    //   (if you decode the data, you should get this number of bytes)
    // code1
    // ...
    // codeN, where N is the count read at the begginning of the file.
    // Each codeI has the following format:
    //
    // 1 byte symbol, 1 byte code bit length, code bytes.
    // Each entry has numbytes_from_numbits code bytes.
    // The last byte of each code may have extra bits, if the number of
    // bits in the code is not a multiple of 8.
    ///
    /// \brief writeCodeTable
    /// \param out
    /// \param totalNbWords Total number of words(bytes) in the input stream
    /// \return
    ///
    bool                    writeCodeTable      (Stream& out,
                                                 uint32_t totalNbWords)       ;

    bool                    printCodeTable      (Stream& out)           const ;
    const HuffmanCode*      findCode            (const Symbol& symbol)  const ;
    bool                    encode              (Stream&    in,
                                                 Stream&    out,
                                                 bool       verbose)     const ;


///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

    std::vector<HuffmanCode*>   m_table                                       ;
};


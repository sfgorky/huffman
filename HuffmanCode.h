#ifndef _HuffmanCode_h_
#define _HuffmanCode_h_

#include "HuffmanNode.h"

class HuffmanCode
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

                        HuffmanCode         ( )                               ;

                    // new_code builds a HuffmanCode from a
                    // leaf in a Huffman tree.
                        HuffmanCode         (const HuffmanNode* leaf)         ;
                       ~HuffmanCode         ( )                               ;

    ///
    /// \brief nbBits returns number of bits associated to this code. The
    ///               bits (1/0) can be retrieved from the 'bit' method.
    /// \return
    ///
    size_t              nbBits              ( )                         const ;

    ///
    /// \brief bit returns the i-th bit, for this code. [0,...,nbBits]
    /// \param i
    /// \return
    Byte                bit                 (size_t i)                  const ;

    /// \brief bitArrayString creates string, with an array of 1 and 0
    /// \return
    std::string         bitArrayString      ( )                         const ;


    const Byte*         byteArray           ( )                         const ;
    // the size of this code
    ///
    /// \brief size Number of bytes internally used to store the code. The
    ///             size of the 'bitArray' array.
    /// \return
    ///
    size_t              nbByte              ( )                         const ;

    const HuffmanNode*  leaf                ( )                         const ;

    /// \brief symbol returns the symbol associated to this code
    /// \return
    const Symbol&      symbol              ( )                         const ;

    // Writes the code for this current node, that represents a symbol
    //
    // writes: [symbol] [nb bits] [count: # of  symbol on stream] [ byte array ]
    // not that the byte array is padded. For example,
    // if the symbol contains 11 bits, 2 bytes will be written.
    bool                writeCode           (Stream& out)               const ;

///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

    // The length of this code in bits.
    unsigned char       m_nbBits                                              ;
    // The bits associated to this code.
    //  1st bit at position 0 in bits[0]
    //  2nd bit at position 1 in bits[0]
    //  8th bit at position 7 in bits[0]
    Byte*                m_bits                                               ;
    const HuffmanNode*   m_leaf                                               ;
};


#endif




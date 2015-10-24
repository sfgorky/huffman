#include <assert.h>
#include "Util.h"
#include "HuffmanCode.h"

HuffmanCode::HuffmanCode( )
    :   m_nbBits(0)
    ,   m_bits(0)
    ,   m_leaf(0)
{
}

///////////////////////////////////////////////////////////////////////////////
// new_code builds a HuffmanCode from a leaf in a Huffman tree.
HuffmanCode::HuffmanCode(const HuffmanNode* leaf)
    :   m_nbBits(0)
    ,   m_bits(0)
    ,   m_leaf(leaf)
{
    // Build the huffman code by walking up to the root node and then
    // reversing the bits, since the Huffman code is calculated by
    // walking down the tree.
    unsigned long numbits = 0;
    unsigned char* bits   = nullptr;

    while(leaf && leaf->parent())
    {
        HuffmanNode* parent = leaf->parent();
        unsigned char cur_bit  = (unsigned char)(numbits % 8);
        unsigned long cur_byte = numbits / 8;

        // If we need another byte to hold the code, then allocate it
        if(cur_bit == 0)
        {
            size_t newSize = cur_byte + 1;
            bits = (Byte*)realloc(bits, newSize);
            bits[newSize-1] = 0; // Initialize the new byte.
        }

        //If a one must be added then or it in. If a zeromust be added then do nothing,
        // since the byte was initialized to zero.
        if(leaf == parent->one())
        {
            bits[cur_byte] |= 1 << cur_bit;
        }
        ++numbits;
        leaf = parent;
    }
    if(bits)
    {
        reverse_bits(bits, numbits);
    }
    delete m_bits;

    m_nbBits = numbits;
    m_bits   = bits;
}

///////////////////////////////////////////////////////////////////////////////

HuffmanCode::~HuffmanCode( )
{
    delete m_bits;
}

///////////////////////////////////////////////////////////////////////////////

size_t HuffmanCode::nbBits()const
{
    return m_nbBits;
}

///////////////////////////////////////////////////////////////////////////////

const Byte* HuffmanCode::byteArray( )const
{
    return m_bits;
}

///////////////////////////////////////////////////////////////////////////////

std::string HuffmanCode::bitArrayString( )const
{
    std::string s;
    for(size_t i=0; i<nbBits(); i++)
    {
        s += (bit(i) == 1 ? "1" : "0");
    }
    return s;
}

///////////////////////////////////////////////////////////////////////////////
// returns a particular bit
Byte HuffmanCode::bit(size_t i)const
{
    return (Byte)get_bit(m_bits, i);
}

///////////////////////////////////////////////////////////////////////////////

size_t HuffmanCode::nbByte( )const
{
    return numbytes_from_numbits(m_nbBits);
}

///////////////////////////////////////////////////////////////////////////////

const HuffmanNode* HuffmanCode::leaf( )const
{
    return m_leaf;
}

///////////////////////////////////////////////////////////////////////////////

const Symbol& HuffmanCode::symbol( )const
{
    assert(m_leaf);
    return m_leaf->symbol();
}

///////////////////////////////////////////////////////////////////////////////
// Writes the code for this current node, that represents a symbol
//
// writes: [symbol] [nb bits] [count: # of  symbol on stream] [ byte array ]
// not that the byte array is padded. For example,
// if the symbol contains 11 bits, 2 bytes will be written.
bool HuffmanCode::writeCode(Stream& out)const
{
    bool bOk = true;
    out.writeSymbol(symbol());              // Write the 1 byte symbol.
    out.writeByte(nbBits());                // Write the 1 byte code bit length.
    out.write((uint32_t)leaf()->count());   // Write the count of this bit

    if(bOk) bOk &= out.write(byteArray(), (size_t)nbByte());
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////





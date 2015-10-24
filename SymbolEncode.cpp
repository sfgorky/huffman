#include <vector>
#include <assert.h>

#include "HuffmanCode.h"
#include "Stream.h"
#include "Symbol.h"
#include "Util.h"

#include "SymbolEncode.h"

SymbolEncoder::SymbolEncoder( )
{
}

///////////////////////////////////////////////////////////////////////////////

SymbolEncoder::~SymbolEncoder( )
{
    for(size_t i=0; i<m_table.size(); i++) delete m_table[i];
}

///////////////////////////////////////////////////////////////////////////////

HuffmanCode* SymbolEncoder::operator[](const size_t i)
{
    return m_table[i];
}

///////////////////////////////////////////////////////////////////////////////

const HuffmanCode* SymbolEncoder::operator[](const size_t i)const
{
    return m_table[i];
}

///////////////////////////////////////////////////////////////////////////////

size_t SymbolEncoder::size( )const
{
    return m_table.size();
}

///////////////////////////////////////////////////////////////////////////////

// total number of Symbols in this encoder.
size_t SymbolEncoder::nbSymbol( )const
{
    size_t count = 0;
    for(size_t i=0; i<m_table.size(); ++i)
    {
        if(m_table[i]) count++;
    }
    return count;
}

///////////////////////////////////////////////////////////////////////////////

// builds a SymbolEncoder by walking down to the leaves of the Huffman tree
// and for each leaf, determines its code.
void SymbolEncoder::buildSymbolEncoder(const HuffmanNode* subtree)
{
    if(subtree)
    {
        if(subtree->isLeaf( ))
        {
            m_table.push_back(new HuffmanCode(subtree));
        }
        else
        {
            buildSymbolEncoder(subtree->zero());
            buildSymbolEncoder(subtree->one());
        }
    }
}

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
bool SymbolEncoder::writeCodeTable(Stream& out, uint32_t totalNbWords)
{
    if(0)
    {
        fprintf(stdout, "writeCodeTable: nbSymbol: %lu, nbWords: %u\n",
                nbSymbol(),
                totalNbWords);
    }
    bool bOk = true;
    uint32_t iNbSymbol = nbSymbol( );

    writeHeader(out);

    if(bOk) bOk = out.write(iNbSymbol);
    if(bOk) bOk = out.write(totalNbWords);
    if(bOk)
    {
        // Write the entries.
        for(size_t i=0; i<m_table.size(); ++i)
        {
            const HuffmanCode* p = m_table[i];
            assert(p);
            bOk = p->writeCode(out);
            if(!bOk) break;
         }
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

bool SymbolEncoder::printCodeTable(Stream& out)const
{
    bool bOk = true;
    // Write the entries.
    for(size_t i=0; i<m_table.size(); ++i)
    {
        const HuffmanCode *p = m_table[i];
        assert(p);

        const Symbol& symbol = p->symbol();
        const int    nbBits = p->nbBits();

        out.printf(_print("symbol: %03u, nbBits: %02d, code: %s",
                          (int)toInt(symbol),
                          (int)nbBits,
                          p->bitArrayString().c_str()));
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////
// TBD
const HuffmanCode* SymbolEncoder::findCode(const Symbol& symbol)const
{
    const HuffmanCode* code = nullptr;
    for(size_t i=0; i<m_table.size(); i++)
    {
        if(m_table[i]->symbol() == symbol)
        {
            code = m_table[i];
        }
    }
    return code;
}

///////////////////////////////////////////////////////////////////////////////

bool SymbolEncoder::encode(Stream& in, Stream& out, bool verbose)const
{
    //Symbol curbyte = 0;
    Byte   curbyte = 0;
    Byte   curbit  = 0; //current bit index
    size_t symbolcount = 0;

    while(1)
    {
        const Symbol uc = in.readSymbol();
        if(!in.isOpen())
        {
            break;
        }
        symbolcount++;
        const HuffmanCode *code = findCode(uc);

        for(size_t i=0; i<code->nbBits(); ++i)
        {
            const Byte bit = code->bit(i);
            curbyte |= (bit << curbit);  // Add the current bit to curbyte.

            //If this byte is filled up then write it out and reset the curbit and curbyte.
            curbit++;
            if(curbit == 8)
            {
                out.writeSymbol(curbyte);
                curbyte = 0;
                curbit = 0;
            }
        }
    }
    // If there is data in curbyte that has not been output yet, which means
    // that the last encoded character did not fall on a byte boundary,
    //then output it.
    if(curbit > 0)
    {
        out.write(curbyte);
    }
    if(verbose) fprintf(stdout, "Encoded  %lu symbols\n", symbolcount);
    return true;
}

#include <stdlib.h>
#include <stdio.h>
#include "Util.h"


unsigned long
numbytes_from_numbits(unsigned long numbits)
{
    return numbits / 8 + (numbits % 8 ? 1 : 0);
}

///////////////////////////////////////////////////////////////////////////////
/*
 * get_bit returns the ith bit in the bits array
 * in the 0th position of the return value.
 */
unsigned char get_bit(const unsigned char* bits, const unsigned long i)
{
    return (bits[i / 8] >> i % 8) & 1;
}

///////////////////////////////////////////////////////////////////////////////

void reverse_bits(unsigned char* bits, unsigned long numbits)
{
    unsigned long numbytes = numbytes_from_numbits(numbits);
    unsigned char* tmp = (unsigned char*)alloca(numbytes);
    unsigned long curbit;
    long curbyte = 0;

    memset(tmp, 0, numbytes);

    for(curbit = 0; curbit < numbits; ++curbit)
    {
        unsigned int bitpos = curbit % 8;

        if(curbit > 0 && curbit % 8 == 0)
            ++curbyte;

        tmp[curbyte] |= (get_bit(bits, numbits - curbit - 1) << bitpos);
    }

    memcpy(bits, tmp, numbytes);
}


const unsigned char FILE_HEADER[12] = "HUFFMAN_02\n";
static const bool ADD_HEADER = true;
///////////////////////////////////////////////////////////////////////////////

///
/// \brief readHeader Reads and validade the file header on a input stream.
/// \param in Stream read only stream.
/// \param rewind bool: if true, it will rewind the stream.
/// \return true/false if success.
///
bool readHeader(Stream& in, bool rewind)
// read the file header
{
    bool bOk = true;
    if(ADD_HEADER)
    {
        if(in.mode() == StreamRead)
        {
            unsigned char  thisHeader[64];
            const size_t   nb = sizeof(FILE_HEADER)/sizeof(FILE_HEADER[0]);
            in.read(thisHeader, nb);
            if(memcmp(thisHeader, FILE_HEADER, nb) != 0)
            {
                bOk = false;
            }
            if(rewind)
            {
                in.rewind( );
            }
        }
        else
        {
            fprintf(stdout, "Error: input stream needs to have read mode\n.");
            bOk = false;
        }
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief writeHeader Writes the the file signature to the output stream
/// \param out output Stream
/// \return true/false if success
///
bool writeHeader(Stream& out)
{
    bool bOk = true;
    if(ADD_HEADER)
    {
        if(out.mode() == StreamWrite)
        {
            bOk = out.write(FILE_HEADER, sizeof(FILE_HEADER)/sizeof(FILE_HEADER[0]));
        }
        else
        {
            bOk = false;
        }
    }
    if(!bOk)
    {
        fprintf(stdout, "Error: cannot write file header\n");
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief isHuffmanFile
/// \param in
/// \return
///
bool isHuffmanFile(Stream& in)
{
    bool bRet = false;
    if(in.mode() == StreamRead)
    {
        bRet = readHeader(in);
        in.rewind();
    }
    return bRet;
}


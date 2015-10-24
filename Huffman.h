#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include "Stream.h"

enum HuffmanDirection
{
        HuffmanEncode
    ,   HuffmanDecode
};

bool huffmanProcess(Stream& in, Stream& out, HuffmanDirection direction, bool verbose);


bool huffmanGetFreqencyTable(Stream& in, Stream& out, bool verbose);
bool huffmanPrintCode(Stream& in, Stream& out, bool verbose);


#endif

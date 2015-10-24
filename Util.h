#ifndef _Util_h_
#define _Util_h_


#ifndef nullptr
#define nullptr 0
#endif

#include "Stream.h"

#define DEBUG_MSG(X) fprintf(stdout, "[DEBUG:%d] %s\n", __LINE__, X);

unsigned long numbytes_from_numbits(unsigned long numbits);

unsigned char get_bit(const unsigned char* bits, const unsigned long i);

void reverse_bits(unsigned char* bits, unsigned long numbits);

///
/// \brief readHeader Reads and validade the file header on a input stream.
/// \param in Stream read only stream.
/// \param rewind bool: if true, it will rewind the stream.
/// \return true/false if success.
///
bool readHeader(Stream& in, bool rewind=false);

///
/// \brief writeHeader Writes the the file signature to the output stream
/// \param out output Stream
/// \return true/false if success
///
bool writeHeader(Stream& out);

/// \brief isHuffmanFile Returns true if the given stream is a Huffman encoded.
/// \param in
/// \return
///
bool isHuffmanFile(Stream& in);

#endif

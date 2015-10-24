#include <vector>
#include <string>
#include <assert.h>
#include <math.h>

#include "Util.h"
#include "Symbol.h"
#include "Stream.h"
#include "HuffmanNode.h"
#include "HuffmanCode.h"
#include "FrequencyTable.h"
#include "SymbolEncode.h"

#include "Huffman.h"

///////////////////////////////////////////////////////////////////////////////
// calculateHuffmanCode turns freqTable into an array with a single entry that
// is the root of the huffman tree. The return value is a SymbolEncoder,
// which is an array of huffman codes index by symbol value.
bool calculateHuffmanCode(FrequencyTable& freqTable, SymbolEncoder& SE)
{
    bool bOk = true;
    freqTable.sort( );
    size_t n = freqTable.numberOfSymbols();
    if(n>0)
    {
        // Construct a Huffman tree. This code is based on the algorithm given in
        //  Managing Gigabytes by Ian Witten et al, 2nd edition, page 34.
        // Note that this implementation uses a simple count instead of probability.
        for(size_t i=0; i<n-1; ++i)
        {
            // Set m1 and m2 to the two subSets of least probability.
            HuffmanNode* m1 = freqTable[0];
            HuffmanNode* m2 = freqTable[1];

            // Replace m1 and m2 with a Set {m1, m2} whose probability
            // is the sum of that of m1 and m2.
            HuffmanNode* newNode = new HuffmanNode(m1,  // zero tree
                                                  m2); // one  tree

            freqTable.setNodeAtBegin(newNode);

            // Put new set into the correct count position in freqTable.
            freqTable.sort();
        }
        // Build the SymbolEncoder array from the tree.
        SE.buildSymbolEncoder(freqTable[0]);
    }
    else
    {
        fprintf(stdout, "Error: cannot compute code. Frequency table is empty.\n");
        bOk = false;
    }
    return bOk;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Builds a Huffman tree from the data in the in file.
// Returns nullptr on error.
// The returned value should be freed with free_huffman_tree.
HuffmanNode* readCodeTable(Stream& in, uint32_t& dataBytes)
{
    if(!readHeader(in))
    {
        fprintf(stdout, "Error: wrong file signature.\n");
        return nullptr;
    }

    HuffmanNode*    root       = new HuffmanNode(NON_LEAF);
    bool            bOk        = true;
    uint32_t        nbEntries  = in.read( );
    dataBytes                  = in.read( );

    while(nbEntries-- > 0)
    {
        // [ symbol ] [ number of bits ] [ count of this symbol (# instances ]

        HuffmanNode* p = root;

        const Symbol   symbol  = in.readSymbol();
        const Byte     numbits = in.readByte();
        const uint32_t count    = in.read();

        std::string s;

        if(in.inError())
        {
            bOk = false;
            break;
        }

        // note that the bytes are padded. For exameple,
        // if a symbol takes 11bits, code will read 2 Bytes
        Byte  numbytes  = (Byte)numbytes_from_numbits(numbits);
        Byte* bytes     = (Byte*)malloc(numbytes);

        // read the actual bytes
        if(in.read(bytes, numbytes))
        {
            // Adds the entry to the Huffman tree. The value of the current bit
            // is used switch between zero and one child nodes in the tree.
            // New nodes are added as needed in the tree.
            for(unsigned int curbit=0; curbit<numbits; ++curbit)
            {
                unsigned char bit = get_bit(bytes, curbit);
                p = p->getSubNode(bit, curbit, symbol, count, numbits);

                s += bit ? "1" : "0";
            }
            //fprintf(stdout, "symbol: %03d, nbBits: %02d, %03d, nbEntries:%d %s %p %p\n", (int)symbol, (int)numbits, (int)numbytes, (int)nbEntries, s.c_str(),
            //            root->one(), root->zero());
        }
        else
        {
            bOk = false;
            free(bytes);
            break;
        }
        free(bytes);
    }
    if(!bOk)
    {
        fprintf(stdout, "Cannot read table\n");
        delete root;
        root = nullptr;
    }
    return root;
}

///////////////////////////////////////////////////////////////////////////////

void  computeAverageBitSize(const SymbolEncoder& SE,
                             size_t  totalCountOfSymbols,
                             double&  bitSize,
                             double&  entropy,
                             bool     verbose)
{
    const double totalCount = totalCountOfSymbols;

    entropy = 0.0;
    for(size_t i=0; i<SE.size(); i++)
    {
        const HuffmanCode* h = SE[i];
        if(h)
        {
            //Symbol s  = h->leaf()->symbol();
            const size_t  bitLength = h->nbBits( );
            const double bitProb    = h->leaf()->count( )/totalCount;

            bitSize += (bitProb * (double)bitLength);
            entropy += (-bitProb * log2(bitProb));
        }
    }
    if(verbose) fprintf(stdout, "Average Bit Size: %-.2g, H:%02g\n", bitSize, entropy);
}

///////////////////////////////////////////////////////////////////////////////

bool huffmanProcess(Stream& in, Stream& out, HuffmanDirection direction, bool verbose)
{
    bool bOk = true;

    if(direction == HuffmanEncode)
    {
        FrequencyTable  freqTable;
        SymbolEncoder   SE;

        if(verbose) fprintf(stdout, "Encode\n");
        {
            // Get the frequency of each symbol in the input file.
            freqTable.updateFreqTable(in);

            if(verbose) fprintf(stdout, "Input Size(%ld bytes)\n", freqTable.totalCount());
            if(verbose) fprintf(stdout, "Nb symbols: %ld\n",       freqTable.numberOfSymbols()); 

            // Build an optimal table from the symbolCount.
            bOk = calculateHuffmanCode(freqTable, SE);

            double bitSize = 0.0;
            double entropy = 0.0;
            if(bOk) 
            {
                computeAverageBitSize(SE, freqTable.totalCount(), bitSize, entropy, verbose);
                // Scan the file again and, using the table
                // previously built, encode it into the output file.
                in.rewind( );
            }
            if(verbose) fprintf(stdout, "Writing code table (%lu symbols, %lu worlds)\n", freqTable.size(), freqTable.totalCount());
            bOk = bOk ? SE.writeCodeTable(out, freqTable.totalCount()) : false;
            if(bOk)
            {
                if(verbose) fprintf(stdout, "Encoding\n");
                bOk = SE.encode(in, out, verbose);
            }
            // Print info
            fprintf(stdout, "Encoded: nbSymbol: %ld, bitSize: %.2g, entropy: %.2g, nbBytes: %ld\n",
                        freqTable.numberOfSymbols(), bitSize, entropy, freqTable.totalCount());
        }
    }
    else
    {
        if(verbose) fprintf(stdout, "Decode File");
        unsigned int totalNbBytes = 0;

        // Read the Huffman code table.
        fprintf(stdout, "# Reading code table");

        std::auto_ptr<const HuffmanNode> root(readCodeTable(in, totalNbBytes));
        if(!root.get())
        {
            fprintf(stdout, "Error: cannot read code table\n");
            bOk = false;
        }
        else
        {
            if(verbose) fprintf(stdout, "# Decoding (size: %u bytes)", totalNbBytes);
            // Decode the file.
            const HuffmanNode*  p = root.get();
            unsigned int remainingNbBytes = totalNbBytes;
            unsigned int totalCountOfSymbols = 0;
            while(remainingNbBytes>0 && in.isOpen())
            {
                const Byte byte = in.readByte();
                Byte       mask = 1;
                while(remainingNbBytes>0 && mask)
                {
                    p = (byte & mask) ? p->one() : p->zero();
                    mask <<= 1;
                    assert(p);
                    if(p->isLeaf( ))
                    {
                        out.writeSymbol(p->symbol());
                        totalCountOfSymbols++;
                        p = root.get();
                        --remainingNbBytes;
                    }
                }
            }

            // this is not needed for the decode - Just for the info section
            SymbolEncoder SE;
            SE.buildSymbolEncoder(root.get());
            double bitSize = 0.0;
            double entropy = 0.0;
            computeAverageBitSize(SE, totalCountOfSymbols, bitSize, entropy, verbose);
            fprintf(stdout, "Decoded: nbSymbol: %ld, bitSize: %.2g, entropy: %.2g, nbBytes: %u\n",
                        SE.nbSymbol(), bitSize, entropy, totalNbBytes);
        }
    }
    if(verbose) fprintf(stdout, "done (%d).\n", bOk);
    return bOk;
}

//////////////////////////////////////////////////////////////////////////////

bool huffmanGetFreqencyTable(Stream& in, Stream& out, bool verbose)
{
    bool bOk = true;
    if(verbose)
    {
        fprintf(stdout, "huffmanGetFreqencyTable\n");
    }
    if(in.mode() == StreamRead && out.mode() == StreamWrite)
    {
        FrequencyTable freqTable;
        freqTable.updateFreqTable(in);
        freqTable.sort();
        freqTable.printFrequencyTable(out);
    }
    else
    {
        bOk = false;
    }
    return bOk;
}

//////////////////////////////////////////////////////////////////////////////

bool huffmanPrintCode(Stream& in, Stream& out, bool verbose)
{
    bool bOk = true;
    // check for the binnary header
    if(in.mode() == StreamRead && out.mode() == StreamWrite)
    {
        if(isHuffmanFile(in))
        {   // An Encoded file
            if(verbose)
            {
                fprintf(stdout, "huffmanPrintCode: encoded file\n");
            }
            uint32_t dataBytes;
            HuffmanNode* root = readCodeTable(in, dataBytes);
            assert(root);
            SymbolEncoder SE;
            SE.buildSymbolEncoder(root);
            SE.printCodeTable(out);
            bOk = true;
        }
        else
        {
            // A natural File
            if(verbose)
            {
                fprintf(stdout, "huffmanPrintCode: natural file\n");
            }
            FrequencyTable freqTable;
            SymbolEncoder  SE;

            freqTable.updateFreqTable(in);
            calculateHuffmanCode(freqTable, SE);
            SE.printCodeTable(out);
            bOk = true;
        }
    }
    else
    {
        bOk = false;
    }
    return bOk;
}

//////////////////////////////////////////////////////////////////////////////

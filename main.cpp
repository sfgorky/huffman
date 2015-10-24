#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
#include <malloc.h>
extern int getopt(int, char**, char*);
extern char* optarg
#else
#include <unistd.h>
#endif

#include "MemBuffer.h"
#include "Huffman.h"

#define VERSION "1.0"

static void version(FILE* out)
{
    fprintf(out, "Huffman v.%s\n", VERSION);
}

///////////////////////////////////////////////////////////////////////////////

static void usage(FILE* out)
{
    fprintf(out, "Huffman (%s) Usage\n", VERSION);
    fputs("Huffman [-i<input file>] [-o<output file>] [-d |-c | -f | -b ]\n"
          "   -i input file (default is standard input)\n"
          "   -o output file (default is standard output)\n"
          "   -d decompress\n"
          "   -c compress\n"
          "   -f prints frequency table\n"
          "   -b prints huffman bit table\n"
          "   -V prints version\n"
          "   -v verbose mode\n"
          "   -m use the memory mode (for test: move to memory, process, dump)\n"
          , out);
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    std::string command;
    std::string file_in;
    std::string file_out;
    bool verboseMode = false;
    bool memoryFunction = false;
    int opt = 0;
    while((opt = getopt(argc, argv, "i:o:cdhvVmfb")) != -1)
    {
        switch(opt)
        {
            case 'i':
                file_in = optarg;
                break;
            case 'o':
                file_out = optarg;
                break;
            case 'c':
                command = "compress";
                break;
            case 'd':
                command = "decompress";
                break;
            case 'h':
                usage(stdout);
                return 0;
            case 'v':
                version(stdout);
                return 0;
            case 'V':
                verboseMode = true;
                break;
            case 'f':
                command = "print_freq_table";
                break;
            case 'b':
                command = "print_huffman_table";
                break;
            case 'm':
                memoryFunction = true;
            break;
            default:
                usage(stdout);
                return 0;
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    // this is a memory buffer, used on memory option (-m). 
    MemBuffer inMemBuffer;
    MemBuffer outMemBuffer;

    // The stream/IO objects that are passed by around the program. Code can read
    // write to it.
    Stream in;
    Stream out;

    if(memoryFunction)
    {
        fprintf(stdout, "Using Memory test....\n");
        if(!inMemBuffer.readFile(file_in))
        {
            return 0;
        }
        outMemBuffer.create(inMemBuffer.size());
        in.open(&inMemBuffer,   StreamRead);
        out.open(&outMemBuffer, StreamWrite);
    }
    else
    {
         if(!in.open(file_in, StreamRead))
         {
             fprintf(stdout, "Error: cannot open input stream (%s)\n", file_in.c_str());
             return 1;
         }
         if(!out.open(file_out, StreamWrite))
         {
             fprintf(stdout, "Error: cannot open output stream (%s)\n", file_out.c_str());
             return 1;
         }
    }

    if(command == "print_freq_table")
    {
       huffmanGetFreqencyTable(in, out, verboseMode);
    }
    else if(command == "print_huffman_table")
    {
        huffmanPrintCode(in, out, verboseMode);
    }
    else if(command == "compress" || command == "decompress")
    {
        huffmanProcess(in, out, (command == "compress" ? HuffmanEncode : HuffmanDecode), verboseMode);
    }
    else
    {
        usage(stdout);    
        return 0;
    }

    if(memoryFunction)
    {
        outMemBuffer.writeFile(file_out);
    }

    in.close();
    out.close();
    return 0;
}


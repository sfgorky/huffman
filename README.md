# huffman
Huffmann encoder/decoder example


## The Following operations are allowed:

* Encode an ascii file:

    huff -i <inputFile> -o <outputFile> -c
    
* Decode an huffman file:

    huff -i <huffFile> -o <asciifile> -d
    
* Prints the frequency table:

    huff -i <asciiFile> -f 
    
* Prints the huffman code:

    huff -i <asciiFile> -b
    
    
* Note: 
    * The -o option is available and points to the output file. When not defined, stdout will be used.

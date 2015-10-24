#ifndef _Frequency_Table_h_
#define _Frequency_Table_h_

#include <vector>
#include "HuffmanNode.h"
#include "Symbol.h"

class FrequencyTable
{
///////////////////////////////////////////////////////////////////////////////
    public:
///////////////////////////////////////////////////////////////////////////////

                 FrequencyTable             ( )                               ;
                ~FrequencyTable             ( )                               ;

    ///
    /// \brief clearFrequencies Set the count table to zero - All the pre-existent
    ///                         symbols stay allocated.
    void        clearFrequencies            ( )                                ;

    ///
    /// \brief sort Sorts the symbol frequency array by ascending frequency. The
    ///             first entry is the the symbol with smaller count, and the
    ///             unused symbol (null pointers) are at the end.
    void        sort                        ( )                                ;

    ///
    /// \brief numberOfSymbols Total number of allocated symbols
    /// \return number of symbols
    size_t      numberOfSymbols             ( )                          const
    { return m_nbSymbols; }

    ///
    /// \brief setNodeAtBegin sets node to position[0], remove position[1]
    ///                       used during code construction
    /// \param node
    void        setNodeAtBegin              (HuffmanNode* node)               ;

    ///
    /// \brief addSymbol Adds a symbol to the frequency table.
    /// \param symbol a symbol to be allocated
    void        addSymbol                   (const Symbol& symbol)            ;

    //TBD: Slow
    HuffmanNode* findSymbol                 (const Symbol& symbol)            ;

    ///
    /// \brief updateFreqTable Scan the input stream, and add all its symbols to the
    ///                       frequency table.
    /// \param in input stream to read the freq. table from
    /// \param initTable if true, the table count be set set to zero.
    void        updateFreqTable             (Stream& in, bool initTable=true) ;

    ///
    /// \brief operator [] returns the i-th node. will return zero, i-th
    ///                    the i-th element does not exist.
    /// \param i the node index (zero based)
    /// \return the i-th mode pointer.
    ///
    HuffmanNode* operator[]                 (const size_t i)            const ;

    ///
    /// \brief size Total number of symbols (includes the ones not used)
    /// \return Total number of symbols
    size_t      size                        ( )                         const ;

    ///
    /// \brief totalCount Total count of elements in the freq. table. Used to
    ///                   compute the individual probabilities.
    /// \return total count
    size_t      totalCount                  ( )                         const ;

    ///
    /// \brief printFrequencyTable Prints a readable (ascii) version of the
    ///                            frequency table.
    /// \param out the output Stream
    void        printFrequencyTable         (Stream& out)               const ;

///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

    std::vector<HuffmanNode*>           m_table                               ;
    size_t                              m_totalCount                          ;
    size_t                              m_nbSymbols                           ;
};

///////////////////////////////////////////////////////////////////////////////

#endif


#ifndef _HuffmanNode_h_
#define _HuffmanNode_h_

static const bool NON_LEAF = false;

#include "Stream.h"
#include "Symbol.h"

///////////////////////////////////////////////////////////////////////////////

class HuffmanNode
{
///////////////////////////////////////////////////////////////////////////////
    public:
 ///////////////////////////////////////////////////////////////////////////////

                HuffmanNode (bool bLeaf)                                       ;
    explicit    HuffmanNode (const Symbol&  _symbol,  const size_t   _count)   ;
    explicit    HuffmanNode (HuffmanNode*   _zero,  HuffmanNode*   _one)       ;

               ~HuffmanNode ( )                                                ;

    void        clear       ( )                                                ;

    bool        isLeaf      ( )                                          const
    { return m_isLeaf; }

    size_t      count       ( )                                          const
    { return m_count; }

    void        addCount    ( )
    { m_count++; }

    const Symbol&       symbol                      ( )                  const
    { return m_symbol; }

    HuffmanNode*        parent                      ( )                  const
    { return m_parent; }

    void                setParent                   (HuffmanNode* parent)
    { m_parent = parent; }

    const HuffmanNode*  one                         ( )                 const
    { return m_one; }

    const HuffmanNode*  zero                        ( )                 const
    { return m_zero; }

    // used on decding
    HuffmanNode* getSubNode(const Byte     bit,
                            const Byte     curbit,
                            const Symbol   symbol,
                            const uint32_t count, // total number of symbols
                            const Byte     numbits)                           ;

    void                print                       (Stream& s)         const ;



///////////////////////////////////////////////////////////////////////////////
    private:
///////////////////////////////////////////////////////////////////////////////

    HuffmanNode*    m_parent                                                  ;
    bool            m_isLeaf                                                  ;

    // used if not leaf
    HuffmanNode*    m_zero                                                    ;
    HuffmanNode*    m_one                                                     ;

    // only used if leaf
    Symbol          m_symbol                                                  ;
    size_t          m_count                                                   ;
};


///////////////////////////////////////////////////////////////////////////////
// When used by qsort, SFComp sorts the array so that the symbol with the lowest
// frequency is first. Any nullptr entries will be sorted to the end of the list.
struct CompareHuffmanNode
{

    //! Compares 2 nodes, and returns (hn1 < hn2) value
    bool operator()(const HuffmanNode* hn1, const HuffmanNode* hn2)
    {
        //lss operator
        bool bLess = true;
        if(!hn1 && !hn2)
        {
            bLess = false;
        }
        else if(hn1 && hn2)
        {
            bLess = (hn1->count() < hn2->count());
        }
        else if(hn1)
        {
            bLess = true;
        }
        else
        {
            bLess = false;
        }
        return bLess;
    }
};

#endif



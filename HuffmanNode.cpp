#include <vector>
#include "Util.h"
#include "Symbol.h"
#include "HuffmanNode.h"

///////////////////////////////////////////////////////////////////////////////

HuffmanNode::HuffmanNode(bool bLeaf)
    :   m_parent(0)
    ,   m_isLeaf(bLeaf)
    ,   m_zero(0)
    ,   m_one(0)
    ,   m_symbol(0) // no-symbol
    ,   m_count(0)
{
}

///////////////////////////////////////////////////////////////////////////////

HuffmanNode::HuffmanNode(const Symbol&  _symbol,
                         const size_t   _count)
    :   m_parent(0)
    ,   m_isLeaf(true)
    ,   m_zero(0)
    ,   m_one(0)
    ,   m_symbol(_symbol)
    ,   m_count(_count)
{
}

///////////////////////////////////////////////////////////////////////////////

HuffmanNode::HuffmanNode(HuffmanNode*   _zero,
                         HuffmanNode*   _one)
    :   m_parent(0)
    ,   m_isLeaf(false)
    ,   m_zero(_zero)
    ,   m_one(_one)
    ,   m_symbol(0) // no-symbol
    ,   m_count(0)
{
    if(m_zero && m_one)
    {
        m_count = m_zero->count() + m_one->count();
        m_zero->setParent(this);
        m_one->setParent(this);
    }
}

///////////////////////////////////////////////////////////////////////////////

HuffmanNode::~HuffmanNode( )
{
    delete m_one;
    delete m_zero;
}

///////////////////////////////////////////////////////////////////////////////

void HuffmanNode::clear( )
{
    m_count  = 0;
    m_isLeaf = 1;
    m_parent = 0;
}

///////////////////////////////////////////////////////////////////////////////

// used on decoding
HuffmanNode* HuffmanNode::getSubNode(const Byte     bit,
                                     const Byte     curbit,
                                     const Symbol   symbol,
                                     const uint32_t count, // total number of symbols
                                     const Byte     numbits)
{
    HuffmanNode* node = bit ? m_one : m_zero;
    if(!node)
    {
        node = (curbit == (unsigned char)(numbits-1))
                        ? new HuffmanNode(symbol, count)
                        : new HuffmanNode(NON_LEAF);
    }
    if(bit) m_one = node; else m_zero = node;
    node->setParent(this);
    return node;
}

///////////////////////////////////////////////////////////////////////////////

static void tree_print(const HuffmanNode*p , std::vector<std::string>& v, std::string s)
{
    if(p->isLeaf())
    {
        s = toString(p->symbol()) + std::string(" ") + s;
        v.push_back(s);
    }
    else
    {
        if(p->one())
        {
            tree_print(p->one(), v, s+std::string("1"));
        }
        if(p->zero())
        {
            tree_print(p->zero(), v, s+std::string("0"));
        }
        if(!p->one() && !p->zero()) { fprintf(stdout, "whattt>??\n"); }
    }
}

///////////////////////////////////////////////////////////////////////////////

void HuffmanNode::print(Stream& s)const
{
    std::vector<std::string> v;
    std::string str;

    tree_print(this, v, str);
    for(size_t i=0; i<v.size(); i++)
    {
        if(!v[i].empty())
        {
            s.printf(_print("%03u: %s", m_symbol.toInt(), v[i].c_str()));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////







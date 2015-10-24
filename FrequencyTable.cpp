#include <assert.h>
#include <vector>
#include <algorithm>

#include "HuffmanNode.h"
#include "Symbol.h"
#include "Util.h"

#include "FrequencyTable.h"

FrequencyTable::FrequencyTable( )
{
    m_totalCount = 0;
    m_nbSymbols  = 0;
}

///////////////////////////////////////////////////////////////////////////////

FrequencyTable::~FrequencyTable( )
{
    for(size_t i=0; i<m_table.size(); i++)
    {
        if(m_table[i]) delete m_table[i];
    }
}

///////////////////////////////////////////////////////////////////////////////

void FrequencyTable::clearFrequencies( )
{
    for(size_t i=0; i<m_table.size(); i++)
    {
        if(m_table[i]) m_table[i]->clear( );
    }
}

///////////////////////////////////////////////////////////////////////////////

void FrequencyTable::sort( )
{
    std::vector<int> toDelete;
    for(size_t i=0; i<m_table.size(); i++)
    {
        if(!m_table[i]) toDelete.push_back(i);
    }
    for(size_t i=0; i<toDelete.size(); i++)
    {
        m_table.erase(m_table.begin() + toDelete[i]-i);
    }
    CompareHuffmanNode diff;
    std::sort(m_table.begin(), m_table.end(), diff);
}

///////////////////////////////////////////////////////////////////////////////

void FrequencyTable::setNodeAtBegin(HuffmanNode* node)
{
    m_table[0] = node;
    m_table[1] = nullptr;
}

///////////////////////////////////////////////////////////////////////////////

void FrequencyTable::addSymbol(const Symbol& symbol)
{
    HuffmanNode* node = findSymbol(symbol);
    if(!node)
    {
        node = new HuffmanNode(symbol, 0);
        m_table.push_back(node);
        m_nbSymbols++;
    }
    node->addCount();
    m_totalCount++;
}

///////////////////////////////////////////////////////////////////////////////
//TBD: Slow
HuffmanNode* FrequencyTable::findSymbol(const Symbol& symbol)
{
    HuffmanNode* node = nullptr;
    for(size_t i=0; i<m_table.size(); i++)
    {
        if(m_table[i]->symbol() == symbol)
        {
            node = m_table[i];
            break;
        }
    }
    return node;
}

///////////////////////////////////////////////////////////////////////////////

void FrequencyTable::updateFreqTable(Stream& in, bool initTable)
{
    // Set all frequencies to 0.
    if(initTable)
    {
        clearFrequencies();
        m_totalCount = 0;
    }
    size_t iNbSymbol = 0;
    // Count the frequency of each symbol in the input file.
    bool bOk = true;
    while(bOk)
    {
        iNbSymbol++;
        Symbol s = in.readSymbol();
        if(in.isOpen())
        {
            addSymbol(s);
        }
        else
        {
            break;
        }
        if(iNbSymbol > INT_MAX)
        {
            fprintf(stdout, "Error: Max number of symbols '%lu\n", iNbSymbol);
            break;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////

HuffmanNode* FrequencyTable::operator[](const size_t i)const
{
    assert(i<m_table.size()); return m_table[i];
}

///////////////////////////////////////////////////////////////////////////////

size_t FrequencyTable::size( )const
{
    return m_table.size();
}

///////////////////////////////////////////////////////////////////////////////

size_t FrequencyTable::totalCount( )const
{
    return m_totalCount;
}

///////////////////////////////////////////////////////////////////////////////

void FrequencyTable::printFrequencyTable(Stream& out)const
{
    out.printf("# Frequency Table (symbol, count, prob)");
    for(size_t i=0; i<m_table.size(); ++i)
    {
        const HuffmanNode* node = m_table[i];
        if(node)
        {
            double total = totalCount();
            out << _print("%03d '%s' %04ld %-5.2g\n",
                          toInt(node->symbol()),
                          toString(node->symbol()).c_str(),
                          node->count(),
                          (node->count()/total)*100.0);
        }
    }
}

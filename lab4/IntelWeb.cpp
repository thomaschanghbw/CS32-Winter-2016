#include "IntelWeb.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <functional>

using namespace std;



////Overload == of InteractionTuple
//bool InteractionTuple::operator==(const InteractionTuple &other) const
//{ return (first == other.first
//          && second == other.second
//          && third == other.third);
//}
//

bool predicateForInteractionTuple(InteractionTuple a, InteractionTuple b)
{
    if(a.context == b.context)
    {
        if(a.from == b.from)
        {
            if(b.to < a.to)
                return false;
            else return true;
        }
        else {
            if(b.from < a.from)
                return false;
            else return true;
        }
    }
    if(b.context < a.context) {
        return false; }
    return true; }


namespace std {
    
    template <>
    struct hash<InteractionTuple>
    {
        std::size_t operator()(const InteractionTuple& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;
            
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:
            
            return ((hash<string>()(k.from)
                     ^ (hash<string>()(k.to) << 1)) >> 1)
            ^ (hash<string>()(k.context) << 1);
        }
    };
    
}


IntelWeb::IntelWeb()
{
}

IntelWeb::~IntelWeb()
{
    close();
}


bool IntelWeb::createNew(const string& filePrefix, unsigned int maxDataItems)
{
    int numBuckets = 2 * maxDataItems;
    close();
    if(!m_mapNormal.createNew(filePrefix + "-normal.dat", numBuckets))
        return false;
    if(!m_mapReverse.createNew(filePrefix + "-reverse.dat", numBuckets))
        return false;
    if(!m_mapPrevalence.createNew(filePrefix + "-prevalence.dat", numBuckets))
        return false;
    return true;
}



bool IntelWeb::openExisting(const std::string& filePrefix)
{
    close();
    if(!m_mapNormal.openExisting(filePrefix + "-normal.dat") || !m_mapReverse.openExisting(filePrefix + "-reverse.dat") || !m_mapPrevalence.openExisting(filePrefix + "-prevalence.dat"))
    {
        close();
        return false;
    }
    return true;
}



void IntelWeb::close()
{
    m_mapNormal.close();
    m_mapReverse.close();
    m_mapPrevalence.close();
}


bool IntelWeb::ingest(const std::string& telemetryFile)
{
    // Open the file for input
    ifstream inf(telemetryFile);
		  // Test for failure to open
    if ( ! inf)
    {
        return false;
    }
		  // Read each line.  The return value of getline is treated
		  // as true if a line was read, false otherwise (e.g., because
		  // the end of the file was reached).
    string line;
    while (getline(inf, line))
    {
        // To extract the information from the line, we'll
        // create an input stringstream from it, which acts
        // like a source of input for operator>>
        istringstream iss(line);
        string key;
        string value;
        string context;
        // The return value of operator>> acts like false
        // if we can't extract a word followed by a number
        if ( ! (iss >> context >> key >> value) )
        {
            return false;
        }
        
        // Add data to expenses map
        m_mapNormal.insert(key, value, context);
        m_mapReverse.insert(value, key, context);
        //Add prevalence
        DiskMultiMap::Iterator it = m_mapPrevalence.search(key);
        int incr = 0;
        if(it.isValid())
        {
        MultiMapTuple m = (*it);
         incr = stoi(m.value);
        }
        incr++;
        m_mapPrevalence.erase(key, to_string( incr-1), "prevalence");
        m_mapPrevalence.insert(key, to_string(incr), "prevalence");
        //Add prevalence for the value now
        incr = 0;
        it = m_mapPrevalence.search(value);
        if(it.isValid())
        {
            MultiMapTuple m = (*it);
            incr = stoi(m.value);
        }
        incr++;
        m_mapPrevalence.erase(value, to_string( incr-1), "prevalence");
        m_mapPrevalence.insert(value, to_string(incr), "prevalence");
        
    }
    return true;
}



unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators, unsigned int minPrevalenceToBeGood, std::vector<std::string>& badEntitiesFound, std::vector<InteractionTuple>& interactions )
{
    int discoveredEntities = indicators.size();
    vector<InteractionTuple> bad_interactions;
    unordered_set<string> checked_entities;
    queue<string> toCheck;
    badEntitiesFound = indicators;
    interactions.clear();
    for (int i =0; i < indicators.size(); i++)
    {
        checked_entities.insert(indicators[i]);
        for(DiskMultiMap::Iterator it1 = m_mapNormal.search(indicators[i]); it1.isValid(); ++it1)
        {
            discoveredEntities += crawlParse(it1, bad_interactions, checked_entities, toCheck, minPrevalenceToBeGood, false, true);
        }
       for(DiskMultiMap::Iterator it1 = m_mapReverse.search(indicators[i]); it1.isValid(); ++it1)
        {
            discoveredEntities += crawlParse(it1, bad_interactions, checked_entities, toCheck, minPrevalenceToBeGood, true, true);
        }
    }
    while(!toCheck.empty())
    {
        string entity = toCheck.front();
        toCheck.pop();
        for(DiskMultiMap::Iterator it1 = m_mapNormal.search(entity); it1.isValid(); ++it1)
        {
            discoveredEntities += crawlParse(it1, bad_interactions, checked_entities, toCheck, minPrevalenceToBeGood, false, false);
        }
        for(DiskMultiMap::Iterator it1 = m_mapReverse.search(entity); it1.isValid(); ++it1)
        {
            discoveredEntities += crawlParse(it1, bad_interactions, checked_entities, toCheck, minPrevalenceToBeGood, true, false);
        }
    }
    //Copy our set into our vectors
    for(unordered_set<string>::iterator it = checked_entities.begin(); it != checked_entities.end(); it++)
    {
        badEntitiesFound.push_back(*it);
    }
    for(int i = 0; i < bad_interactions.size(); i++)
    {
        interactions.push_back(bad_interactions[i]);
    }
    sort(badEntitiesFound.begin(), badEntitiesFound.end());
    sort(interactions.begin(), interactions.end(), predicateForInteractionTuple);
    auto it = interactions.begin();
    auto it2 = interactions.begin();
    for(; it != interactions.end(); it++)
    {
        it2 = it;
        it2++;
        if(it2 == interactions.end())
            break;
    while (it->from == (it2)->from && it->to == (it2)->to && it->context == (it2)->context)
    {
        it2 = interactions.erase(it2);
    }
    }
    return discoveredEntities;
}



int IntelWeb::crawlParse(DiskMultiMap::Iterator it1, vector<InteractionTuple>& bad_interactions, unordered_set<string>& checked_entities, queue<string>& toCheck, unsigned int minPrevalenceToBeGood, bool flip, bool preval)
{
    MultiMapTuple mmt = (*it1);
    if(flip)
        bad_interactions.push_back(InteractionTuple(mmt.value, mmt.key, mmt.context));
    else {
        bad_interactions.push_back(InteractionTuple(mmt.key, mmt.value, mmt.context)); }
    if(checked_entities.find(mmt.value) == checked_entities.end())
    {
        DiskMultiMap::Iterator it2 = m_mapPrevalence.search(mmt.value);
        if(!it2.isValid())
            return 0;
        MultiMapTuple mmt2 = (*it2);
        if(stoi(mmt2.value) < minPrevalenceToBeGood || preval)
        {
        toCheck.push(mmt.value);
        checked_entities.insert(mmt.value);
        return 1;
        }
    }
    return 0;
}



bool IntelWeb::purge(const std::string& entity)
{
    bool purgedAtLeastOne = false;
    DiskMultiMap::Iterator it = m_mapNormal.search(entity);
    queue<string> toPurge;
    while(it.isValid())
    {
        MultiMapTuple mmt = (*it);
        toPurge.push(mmt.value);
        ++it;
        m_mapNormal.erase(entity, mmt.value, mmt.context);
        purgedAtLeastOne = true;
    }
    while(!toPurge.empty())
    {
        string temp = toPurge.front();
        toPurge.pop();
        it = m_mapReverse.search(temp);
        for(bool stop = false; !stop; ++it)
        {
            MultiMapTuple mmt = (*it);
            if(mmt.value == entity)
            {
                stop = true;
                m_mapReverse.erase(mmt.key, entity, mmt.context);
            }
        }
    }
    
    it = m_mapReverse.search(entity);
    while(it.isValid())
    {
        MultiMapTuple mmt = (*it);
        toPurge.push(mmt.value);
        ++it;
        m_mapReverse.erase(entity, mmt.value, mmt.context);
        purgedAtLeastOne = true;
    }
    while(!toPurge.empty())
    {
        string temp = toPurge.front();
        toPurge.pop();
        it = m_mapNormal.search(temp);
        for(bool stop = false; !stop; ++it)
        {
            MultiMapTuple mmt = (*it);
            if(mmt.value == entity)
            {
                stop = true;
                m_mapNormal.erase(mmt.key, entity, mmt.context);
            }
        }

}
            return purgedAtLeastOne;
}


#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include <string>
#include <vector>
#include "BinaryFile.h"
#include "DiskMultiMap.h"
#include <unordered_set>
#include <queue>


class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators,
                       unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound,
                       std::vector<InteractionTuple>& interactions
                       );
    bool purge(const std::string& entity);
    
private:
    // Your private member declarations will go here
    DiskMultiMap m_mapNormal;
    DiskMultiMap m_mapReverse;
    DiskMultiMap m_mapPrevalence;
    int crawlParse(DiskMultiMap::Iterator it1, vector<InteractionTuple>& bad_interactions, unordered_set<string>& checked_entities, queue<string>& toCheck, unsigned int minPrevalenceToBeGood, bool flip, bool preval);
};



#endif // INTELWEB_H_

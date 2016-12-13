#include "DiskMultiMap.h"

using namespace std;

DiskMultiMap::DiskMultiMap()
{}

DiskMultiMap::~DiskMultiMap()
{
    //if the file is not closed
        //close the file
    if(bf.isOpen())
        bf.close();
}

bool DiskMultiMap::createNew(const string &filename, unsigned int numBuckets)
{
    if(numBuckets ==0)
        return false;
    if(bf.isOpen())
    {
        bf.close();
    }
    if(bf.createNew(filename))
    {

        Header header;
//        cout << "Size of Header: " << sizeof(header) << endl;
        header.numBuckets = numBuckets;
        if(!bf.write(header, 0))
            return false;
        for (int bckt = 0; bckt < numBuckets; bckt++)
        {
            BinaryFile::Offset off = -1;
            if(!bf.write(off, bf.fileLength()))
                return false;
        }
        bf.read(header, 0);
//        cout << "File Length Should be 24: " << bf.fileLength() << endl << "Number of Buckets Should be 4: " << header.numBuckets << endl;
        return true;
    }
    return true;
}

bool DiskMultiMap::openExisting(const std::string &filename)
{
    if(bf.isOpen())
        bf.close();
    if(bf.openExisting(filename))
        return true;
    else
        return false;
}

void DiskMultiMap::close()
{
    if (bf.isOpen())
        bf.close();
}

bool DiskMultiMap::insert(const std::string &key, const std::string &value, const std::string &context)
{
    if(!bf.isOpen())
        return false;
    if (strlen(key.c_str()) > 120 || strlen(value.c_str()) > 120 || strlen(context.c_str()) > 120)
        return false;
    //Find the bucket for our key
    Header header;
    bf.read(header, 0);
    int bucketNumber = hashMe(key, header.numBuckets);
    //First find an open spot to put our key and values, then connect this spot to our bucket
    if (header.locationOfFirstFreeDiskSpace == -1)
    {
        if(!addEntry(bf.fileLength(), bucketNumber, Entry(key, value, context)))
            return false;
//        cout << "location of first free disk space doesnt exist! Yay! " << endl << "FileLength should be 391 I think: " << bf.fileLength() ;
    }
    //else we take this empty spot, and tell header where our next empty spot is, if there is one
    else
    {
        Entry temp_Entry;
        bf.read(temp_Entry, header.locationOfFirstFreeDiskSpace);
        int loc =header.locationOfFirstFreeDiskSpace;
        if(header.locationOfLastFreeDiskSpace == header.locationOfFirstFreeDiskSpace)
            header.locationOfLastFreeDiskSpace = -1;
        header.locationOfFirstFreeDiskSpace = temp_Entry.next;
        if(!bf.write(header, 0))
            return false;
        if(!addEntry(loc, bucketNumber, Entry(key,value,context)))
            return false;
    }
    return true;
    
}

//Adds an entry at our desired location, and tags it to the end of its bucket
bool DiskMultiMap::addEntry(const int& location, const int& bucket_num, const Entry& my_Entry)
{
//    cout << "Size of Entry: " << sizeof(my_Entry) << endl;
//    cout << "Size of key: " << sizeof(my_Entry.key) << " Size of value: " << sizeof(my_Entry.str1) << " Size of context: " << sizeof(my_Entry.str2) << "Size of next: " << sizeof(my_Entry.next);
    if(!bf.write(my_Entry, location))
        return false;
    int bucket_position = (4*bucket_num) + 8;
    BinaryFile::Offset temp_offset, temp_offset2;
    bf.read(temp_offset, bucket_position);
    //If there is nothing in the bucket, set this bucket to "point" to our entry at location
    if (temp_offset == -1)
    {
        temp_offset2 = location;
        if(!bf.write(temp_offset2, bucket_position))
            return false;
        return true;
    }
    else {
        temp_offset2 = temp_offset;
        Entry temp_Entry;
        //temp_offset2 is always one pointer in front of temp_offset. temp_offset will end up pointing to the entry with no next
        while (temp_offset2 != -1)
        {
            bf.read(temp_Entry, temp_offset2);
            temp_offset = temp_offset2;
            temp_offset2 = temp_Entry.next;
        }
        temp_Entry.next = location;
        if(!bf.write(temp_Entry, temp_offset))
            return false;
    }
    return true;
}



//Erases the value-context pairs with the corresponding key
int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
    if(key.length() > 120)
        return 0;
    Header h;
    BinaryFile::Offset offset;
    Entry entry;
    int numberOfDeletedEntries = 0;
    bf.read(h, 0);
    //Retrieve location of the right bucket
    int bucketNumber = hashMe(key, h.numBuckets);
    int bucket_position = (4*bucketNumber) +8;
    bf.read(offset, bucket_position);
    //Keeps track of the location of our offset
    int offsetPosition = bucket_position;
    while(offset != -1)
    {
        bf.read(entry, offset);
        //If we find a matching key-value-context group, erase it's contents, remove it from the bucket, and add it to the linked list of empty locations
        if(strcmp(entry.key, key.c_str()) == 0 && strcmp(entry.str1, value.c_str()) == 0 && strcmp(entry.str2, context.c_str()) == 0)
        {
            //Temorary offset to store current location
            BinaryFile::Offset temp_offset = offset;
            entry.key[0] = entry.str1[0] = entry.str2[0] = '\0';
            //Increment the offset
            offset = entry.next;
            bf.write(offset, offsetPosition);
            //Update tracker of unused locations, first checking if we have any to begin with
            if(h.locationOfFirstFreeDiskSpace == -1)
            {
                h.locationOfFirstFreeDiskSpace = h.locationOfLastFreeDiskSpace = temp_offset;
                bf.write(h, 0);
            }
            else{
                Entry temp_entry;
                bf.read(temp_entry, h.locationOfLastFreeDiskSpace);
                temp_entry.next = temp_offset;
                bf.write(temp_entry, h.locationOfLastFreeDiskSpace);
                h.locationOfLastFreeDiskSpace = temp_offset;
                bf.write(h, 0);
            }
            //Increase count of deleted entries
            numberOfDeletedEntries++;
        }
        else
        {
            //Set position to the next Entry's next member variable, which comes after these three values
            offsetPosition = offset + sizeof(entry.key) + sizeof(entry.str1) + sizeof(entry.str2);
            offset = entry.next;
        }
    }
    return numberOfDeletedEntries;
}



//Hash function
int DiskMultiMap::hashMe(const string& hashString, const int& numBuckets)
{
    hash<string> str_hash;
    unsigned int hashValue = str_hash(hashString);
    unsigned int buck;
    if(numBuckets == 0)
    {
         buck = hashValue % 1;
    }
    else
    unsigned int buck = hashValue % numBuckets;
//    cout << buck;
    return buck;
}



//Only use this when using the read function
DiskMultiMap::Entry::Entry()
{ next = -1;}



//Otherwise use this version
DiskMultiMap::Entry::Entry( const string& key_string, const string& str1_string,  const string& str2_string)
{
    strcpy(key, key_string.c_str());
    strcpy(str1, str1_string.c_str());
    strcpy(str2, str2_string.c_str());
    next = -1;
}



//Header
DiskMultiMap::Header::Header()
{
    locationOfFirstFreeDiskSpace = -1;
    locationOfLastFreeDiskSpace = -1;
}

/********************
Iterator
********************/
DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
    //Retrieve the header
    Header h;
    bf.read(h, 0);
    //get the bucket number and its position in our disk
    int myBucket = hashMe(key, h.numBuckets);
    int bucket_position = (4*myBucket) + 8;
    //Get the offset value at this position
    BinaryFile::Offset offset;
    bf.read(offset, bucket_position);
    //If it is -1, then there is definitely no match
    if(offset == -1)
        return Iterator();
    //Go through everything in this bucket to look for a match, if no match, return a bad iterator
    Entry e;
    while(offset != -1)
    {
        bf.read(e, offset);
        if(strcmp(e.key, key.c_str()) == 0)
            return Iterator(true, offset, &bf);
        offset = e.next;
    }
    return Iterator();
}
DiskMultiMap::Iterator::Iterator() : m_validState(false), m_positionOfIterator(-1), m_bf(nullptr)
{}
DiskMultiMap::Iterator::Iterator(bool validState, int positionOfIterator, BinaryFile* bf) : m_validState(validState), m_positionOfIterator(positionOfIterator), m_bf(bf)  {}


bool DiskMultiMap::Iterator::isValid() const
{return m_validState;}



DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    if(!isValid())
        return *this;
    Entry entry;
    m_bf->read(entry, this->m_positionOfIterator);
    char key[120+1];
    strcpy(key, entry.key);
    BinaryFile::Offset offset = entry.next;
    while(offset != -1)
    {
        m_bf->read(entry, offset);
        if(strcmp(entry.key, key) == 0)
        {
            m_positionOfIterator = offset;
            return *this;
        }
        offset = entry.next;
    }
    this->m_validState = false;
    return *this;
}


MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    MultiMapTuple mmt;
    if(!this->m_validState)
        return mmt;
    Entry entry;
    m_bf->read(entry, this->m_positionOfIterator);
    mmt.key = entry.key;
    mmt.value = entry.str1;
    mmt.context = entry.str2;
    return mmt;
    
}







//
//
//
//
//void DiskMultiMap::TESTERFUNCTION()
//{
//    createNew("test.dat", 1);
//    insert("Thomas", "Jerry", "Chang");
//    insert("Thomas", "Jeremey", "Hello");
//    insert("Thomas", "Curry", "Rice");
//    insert("Thomas", "Jerry", "Chang");
//    erase("Thomas", "Jeremey", "Hello");
//    erase("Thomas", "Curry", "Rice");
//    Header test_header;
//    bf.read(test_header, 0);
//    cout << "This value should be 384: " << test_header.locationOfFirstFreeDiskSpace << endl;
//    cout << "This value should be 752: " << test_header.locationOfLastFreeDiskSpace << endl;
//}
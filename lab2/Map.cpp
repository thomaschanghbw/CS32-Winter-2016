//
//  Map.cpp
//  CS32 P2
//
//  Created by Thomas Chang on 1/25/16.
//  Copyright © 2016 CS32. All rights reserved.
//

#include "Map.h"
using namespace std;

/**************************
 Uses HW1 solutions as a base
 **************************/

Map::Map()
: m_size(0)
{
    head = nullptr;
    tail = nullptr;
}
Map::~Map()
{
    Node* p = head;
    while ( p!= nullptr && tail != nullptr && p!= tail->next)
    {
        Node* n = p->next;
        delete p;
        p = n;
    }
}
Map& Map::operator=(const Map& rhs)
{
    if (this != &rhs)
    {
    Node* p = head;
    while ( p!= nullptr)
    {
        Node* n = p->next;
        delete p;
        p = n;
    }
        head = nullptr;
        tail = nullptr;
    p = rhs.head;
        if ( p == nullptr)
        {
        head = nullptr;
        tail = nullptr;
            m_size = rhs.m_size;
            return *this;
        }
    Node* n =head;
    Node* o = nullptr;
    while (p!=nullptr)
    {
        n = new Node;
        n->m_key = p->m_key;
        n->m_value = p->m_value;
        n->next = nullptr;
        n->prev = o;
        if (n->prev != nullptr)
            n->prev->next = n;
        if (head == nullptr)
            head = n;
        o = n;
        p=p->next;
    }
        tail = n;
}
    m_size = rhs.m_size;
    return *this;
}
Map::Map(const Map& other)
{
    //Special case where list is empty
    if (other.head == nullptr)
    {
        head = nullptr;
        tail = nullptr;
        this->m_size = other.m_size;
    }
    //If the list is not empty
    if (other.head != nullptr)
    {
        //Setup head
    Node* p = other.head;
    head = new Node;
    Node* n = head;
    n->prev = nullptr;
        //Make a copy of the list up 
    while(p != nullptr)
    {
        n->m_key = p->m_key;
        n->m_value = p->m_value;
        n->next = new Node;
        n->next->prev = n;
        n=n->next;
        p=p->next;
    }
        //In the above, an extra Node is dynamically allocated. Delete it
        tail = n->prev;
        delete n;
        this->m_size = other.m_size;
    }
}
int Map::size() const
{
    return m_size;
}

bool Map::empty() const
{
    return size() == 0;
}

bool Map::contains(const KeyType& key) const
{
    return find(key) != nullptr;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
    return doInsertOrUpdate(key, value, true /* insert */, false /* no update */);
}

bool Map::update(const KeyType& key, const ValueType& value)
{
    return doInsertOrUpdate(key, value, false /* no insert */, true /* update */);
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
    return doInsertOrUpdate(key, value, true /* insert */, true /* update */);
}

bool Map::erase(const KeyType& key)
{
    Node* pos = find(key);
    if (pos == nullptr)  // not found
        return false;
    // If first item in the list
    if (pos->prev == nullptr)
    {
        m_size--;
        Node* temp = head;
        head = head->next;
        if (head != nullptr)
        head->prev = nullptr;
        if (tail->prev == nullptr)
            tail=nullptr;
        delete temp;
        return true;
    }
    // If last item in the list
    if (pos->next == nullptr)
    {
        m_size--;
        Node* temp = tail;
        tail = tail->prev;
        tail->next = nullptr;
        delete temp;
        return true;
    }

    if (head == tail)
    {
        delete pos;
        head = nullptr;
        tail = nullptr;
        return true;
    }
    // Move last array item to replace the one to be erased
    m_size--;
    Node* temp = pos;
    pos->next->prev = temp->prev;
    pos->prev->next = temp-> next;
    delete temp;
    return true;
}

bool Map::get(const KeyType& key, ValueType& value) const
{
    Node* pos = find(key);
    if (pos == nullptr)  // not found
        return false;
    value = pos->m_value;;
    return true;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
    if (i < 0  ||  i >= m_size) // if it turns out m_size unnecessary, remove this chunk
        return false;
    int j = 0;
    for (Node* t = head;t!=nullptr && j <= i; t = t->next, j++)
    {
        if (j == i)
        {
            key = t->m_key;
            value = t->m_value;
            return true;
        }
    }
    return false;
}

void Map::swap(Map& other)
{
    // Swap element by swapping pointers
    
    Node* temp;
    temp = head;
    head = other.head;
    other.head = temp;
    temp = tail;
    tail = other.tail;
    other.tail=temp;
    
    // Swap sizes
    
    int t = m_size;
    m_size = other.m_size;
    other.m_size = t;
}

Map::Node* Map::find(const KeyType& key) const
{
    // Only one element in the list, return pointer to head
    if (head == tail && head != nullptr)
    {
        if (head->m_key == key)
            return head;
        return nullptr;
    }
    // Do a linear search through the array.
    Node* p = head;
    while (p!= nullptr)
    {
        if (p->m_key == key)
            return p;
        p = p->next;
    }
//    for (Node* p = head; p!= nullptr && p->next != nullptr; (p = p->next))
//        if (p->m_key == key)
//            return p;
    return nullptr;
}

bool Map::doInsertOrUpdate(const KeyType& key, const ValueType& value,
                           bool mayInsert, bool mayUpdate)
{
    Node* p = find(key);
    if (p != nullptr)  // found
    {
        if (mayUpdate)
            p->m_value = value;
        return mayUpdate;
    }
    if (mayUpdate && !mayInsert)
        return false;
    // If user wants to insert but the element already exists
    if (p != nullptr && mayInsert && !mayUpdate)
        return false;
    // If list was empty
    if (tail == nullptr)
    {
        tail = new Node;
        tail->m_key = key;
        tail->m_value = value;
        tail->next = nullptr;
        tail->prev = nullptr;
        head = tail;
        m_size++;
        return true;
    }

    p = tail;
    tail = new Node;
    tail->m_key = key;
    tail->m_value = value;
    tail->next = nullptr;
    tail->prev = p;
    p->next = tail;
    m_size++;
    return true;
}

// Non-class functions
bool combine(const Map& m1, const Map& m2, Map& result)
{
    result = m1;
    KeyType temp_key;
    ValueType temp_val;
    ValueType check_val;
    bool will_return = true;  //Holds whether the function will return true or false
    
    for (int i = 0; i <m2.size(); i++)
    {
        m2.get(i, temp_key, temp_val);
        if (m1.get(temp_key, check_val))
        {
            if (temp_val != check_val)
            {
                will_return = false;
                result.erase(temp_key);
            }
        }
        else {
            result.insert(temp_key, temp_val);
        }
        
    }
    return will_return;
}

void subtract(const Map& m1, const Map& m2, Map& result)
{
    result = m1;
    KeyType temp_key;
    ValueType temp_val;
    for (int i = 0; i<m2.size(); i++)
    {
        m2.get(i, temp_key, temp_val);
        if (m1.contains(temp_key))
        {
            result.erase(temp_key);
        }
        
    }
}

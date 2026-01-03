#ifndef HashMap_H
#define HashMap_H
#include <iostream>
#include <stdexcept>
#include <utility>
using namespace std;

template <typename K>
struct Hash;

template <>
struct Hash<int>
{
    size_t operator()(int key) const
    {
        return static_cast<size_t>(key);
    }
};

template <>
struct Hash<std::string>
{
    size_t operator()(const std::string &key) const
    {
        size_t hash = 0;
        for (char c : key)
            hash = hash * 31 + c;
        return hash;
    }
};
template <typename K, typename V, typename HashFunc = Hash<K>>
struct HashNode
{
    K key;
    V value;
    HashNode *next;

    HashNode(const K &k, const V &v)
        : key(k), value(v), next(nullptr) {}
};

template <typename K, typename V, typename HashFunc = Hash<K>>
class HashMap
{
private:
    HashNode<K, V, HashFunc> **table;
    size_t capacity;
    size_t size;
    HashFunc hashFunc;

public:
    HashMap(size_t cap = 16)
        : capacity(cap), size(0)
    {
        table = new HashNode<K, V, HashFunc> *[capacity];
        for (size_t i = 0; i < capacity; i++)
            table[i] = nullptr;
    }
    ~HashMap()
    {
        clear();
        delete[] table;
    }

    void insert(const K &key, const V &value)
    {
        size_t index = hashFunc(key) % capacity;
        HashNode<K, V, HashFunc> *node = table[index];
        while (node)
        {
            if (node->key == key)
            {
                node->value = value;
                return;
            }
            node = node->next;
        }
        HashNode<K, V, HashFunc> *newNode = new HashNode<K, V, HashFunc>(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        size++;
    }
    V get(const K &key) const
    {
        size_t index = hashFunc(key) % capacity;
        HashNode<K, V, HashFunc> *node = table[index];
        while (node)
        {
            if (node->key == key)
                return node->value;
            node = node->next;
        }
        throw out_of_range("Key not found");
    }
    bool containsKey(const K &key) const
    {
        size_t index = hashFunc(key) % capacity;
        HashNode<K, V, HashFunc> *node = table[index];
        while (node)
        {
            if (node->key == key)
                return true;
            node = node->next;
        }
        return false;
    }
    void remove(const K &key)
    {
        size_t index = hashFunc(key) % capacity;
        HashNode<K, V, HashFunc> *node = table[index];
        HashNode<K, V, HashFunc> *prev = nullptr;
        while (node)
        {
            if (node->key == key)
            {
                if (prev)
                    prev->next = node->next;
                else
                    table[index] = node->next;
                delete node;
                size--;
                return;
            }
            prev = node;
            node = node->next;
        }
    }
    size_t Size() const
    {
        return size;
    }

    void clear()
    {
        for (size_t i = 0; i < capacity; i++)
        {
            HashNode<K, V, HashFunc> *node = table[i];
            while (node)
            {
                HashNode<K, V, HashFunc> *temp = node;
                node = node->next;
                delete temp;
            }
            table[i] = nullptr;
        }
        size = 0;
    }
    class Iterator
    {
    private:
        HashNode<K, V, HashFunc> **table;
        size_t capacity;
        size_t index;
        HashNode<K, V, HashFunc> *current;

        void moveToNextValid()
        {
            while (!current && index + 1 < capacity)
            {
                index++;
                current = table[index];
            }
        }

    public:
        Iterator(HashNode<K, V, HashFunc> **tbl,
                 size_t cap,
                 size_t idx,
                 HashNode<K, V, HashFunc> *node)
            : table(tbl), capacity(cap), index(idx), current(node)
        {
            if (!current)
                moveToNextValid();
        }

        HashNode<K, V, HashFunc> &operator*()
        {
            return *current;
        }

        HashNode<K, V, HashFunc> *operator->()
        {
            return current;
        }

        Iterator &operator++()
        {
            if (current)
                current = current->next;

            if (!current)
                moveToNextValid();

            return *this;
        }

        bool operator==(const Iterator &other) const
        {
            return current == other.current;
        }

        bool operator!=(const Iterator &other) const
        {
            return !(*this == other);
        }
    };
    Iterator begin()
    {
        for (size_t i = 0; i < capacity; i++)
        {
            if (table[i])
                return Iterator(table, capacity, i, table[i]);
        }
        return end();
    }
    Iterator end()
    {
        return Iterator(table, capacity, capacity, nullptr);
    }
};

#endif
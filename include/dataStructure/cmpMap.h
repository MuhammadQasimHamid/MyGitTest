#ifndef CMPMAP_H
#define CMPMAP_H

#include <iostream>
#include <unordered_map>
using namespace std;

template <typename T1, typename T2>
struct cmpPair
{
private:
    T1 val1;
    bool val1Exists_;
    T2 val2;
    bool val2Exists_;

public:
    cmpPair() : val1Exists_(false), val2Exists_(false) {}

    void setVal1(const T1 &value)
    {
        val1 = value;
        val1Exists_ = true;
    }

    void setVal2(const T2 &value)
    {
        val2 = value;
        val2Exists_ = true;
    }

    bool val1Exists() const { return val1Exists_; }
    bool val2Exists() const { return val2Exists_; }

    T1 getVal1() const { return val1; }
    T2 getVal2() const { return val2; }
};

template <typename KEYT, typename VAL1T, typename VAL2T>
class cmpMap
{
    // private:
    
    public:
    unordered_map<KEYT, cmpPair<VAL1T, VAL2T>> umap;
    void addVal1(const KEYT &key, const VAL1T &val1)
    {
        umap[key].setVal1(val1);
    }

    void addVal2(const KEYT &key, const VAL2T &val2)
    {
        umap[key].setVal2(val2);
    }

    bool hasVal1(const KEYT &key) const
    {
        auto it = umap.find(key);
        return it != umap.end() && it->second.val1Exists();
    }

    bool hasVal2(const KEYT &key) const
    {
        auto it = umap.find(key);
        return it != umap.end() && it->second.val2Exists();
    }

    VAL1T getVal1(const KEYT &key) const
    {
        return umap.at(key).getVal1();
    }

    VAL2T getVal2(const KEYT &key) const
    {
        return umap.at(key).getVal2();
    }
};

#endif

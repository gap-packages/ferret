#ifndef PERM_HPP_PDWE
#define PERM_HPP_PDWE

#include "vec1.hpp"
#include "library.hpp"
#include "shared_ptr.hpp"


//#define EAGER_MERGE


class Permutation;

class PermSharedData : public shared_ptr_base
{
#ifndef EAGER_MERGE
    vec1<Permutation> perms;
 #endif
    // use data_m[0] as length
    // rest of data_m is permutation
    // this '1' is misleading, we allocate enough memory
    // to hold whole permutation.
    // Therefore, this must be the last member!
    int data_m[1];


    PermSharedData(int length)
    { data_m[0] = length; }
public:

    int size()
    { return data_m[0]; }

    int& operator[](int i);

    int& raw(int i);

    friend PermSharedData* makePermSharedData(int i);

    template<typename T>
    friend PermSharedData* makePermSharedDataFromContainer(const T& data);

    friend void decrementPermSharedDataCount();
    friend void incrementPermSharedDataCount();

    friend PermSharedData* makePermSharedDataFromPermList(const vec1<Permutation>& data);

};

inline PermSharedData* makePermSharedData(int len)
{
    PermSharedData* psd = (PermSharedData*)calloc(sizeof(PermSharedData) + len*sizeof(int),1);
    new (psd) PermSharedData(len);
    return psd;
}

template<typename T>
PermSharedData* makePermSharedDataFromContainer(const T& data)
{
    PermSharedData* psd = (PermSharedData*)malloc(sizeof(PermSharedData) + data.size()*sizeof(int));
    psd = new (psd) PermSharedData(data.size());
    for(int i : range1(data.size()))
        (psd->data_m)[i] = data[i];
    return psd;
}



inline void decrementPermSharedDataCount(PermSharedData* psm)
{
    int count = psm->decrement_counter();
    if(count == 0)
    {
        psm->~PermSharedData();
        free(psm);
    }
}

inline void incrementPermSharedDataCount(PermSharedData* psm)
{ psm->increment_counter(); }


class Permutation
{
    PermSharedData* psm;
public:

    Permutation() : psm(0) {}

    Permutation(const Permutation& p)
    : psm(p.psm)
    {
        if((long)psm > 1)
            incrementPermSharedDataCount(psm);
    }

    Permutation& operator=(const Permutation& p)
    {
        // These have to be in this order, in case of self-assignment.
        if(p.psm)
            incrementPermSharedDataCount(p.psm);

        if(psm)
            decrementPermSharedDataCount(psm);

        psm = p.psm;

        return *this;
    }

private:

    template<typename Container>
    static bool isPermutation(Container con)
    {
        std::sort(con.begin(), con.end());
        for(int i : range1(con.size()))
            if(con[i] != i)
                return false;
        return true;
    }

public:

    void store_perm_from_vec(const vec1<int>& v)
    {
        D_ASSERT(isPermutation(v));
        if(v.size() > 0)
        {
            psm = makePermSharedDataFromContainer(v);
        }
        else
        {
            psm = 0;
        }
        D_ASSERT(validate());
    }

    explicit Permutation(const vec1<int>& v)
    { store_perm_from_vec(v); }

    explicit Permutation(PermSharedData* _psm) : psm(_psm)
    { }

    explicit Permutation(vec1<int> v, int size)
    {
        while(v.size() < size)
        {
            v.push_back(v.size() + 1);
        }
        store_perm_from_vec(v);
    }

    int operator[](int i) const
    {
        D_ASSERT(i >= 1);
        if(!psm || i > psm->size())
        {
            return i;
        }
        return (*psm)[i];
    }

    // This method allows writing to the permutation
    // This is VERY dangerous. This should only be used
    // after creating a permutation with
    int& raw(int i) const
    {
    	D_ASSERT(i >= 1 && psm && i <= psm->size());
    	return psm->raw(i);
    }

    int size() const
    { if(psm) return psm->size(); else return 0; }

    ~Permutation()
    {
        if(psm)
            decrementPermSharedDataCount(psm);
    }

    friend bool operator==(const Permutation& p, const Permutation& q)
    {

        int maxsize = std::max(p.size(), q.size());
        for(int i : range1(maxsize))
        {
            if(p[i] != q[i])
                return false;
        }
        return true;
    }

    std::string cycle() const
    {
        std::ostringstream oss("");
        std::vector<bool> checked(size() + 1, false);
        bool some_printed = false;
        for(int i : range1(size()))
        {
            if(!checked[i] && (*this)[i] != i)
            {
                some_printed = true;
                oss << "(";
                int pos = i;
                int first = true;
                while(!checked[pos])
                {
                    if(first) first=false; else oss << ",";
                    oss << pos;
                    checked[pos] = true;
                    pos = (*this)[pos];
                }
                oss << ")";
            }
        }
        if(!some_printed)
            oss << "()";
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& o, const Permutation& p)
    {
        o << "[";
        for(int i : range1(p.size()))
        {
            if(i != 1)
                o << ",";
            o << p[i];
        }
        return o << "]";
    }

    bool validate() const
    {
    	vec1<int> p;
    	for(int i : range1(size()))
    		p.push_back((*this)[i]);
    	std::sort(p.begin(), p.end());
    	for(int i : range1(size()))
    	{
    		if(p[i] != i)
    			return false;
    	}
    	return true;
    }
};

inline Permutation identityPermutation()
{
    static Permutation p(vec1<int>(0));
    return p;
}

inline Permutation invertPermutation(const Permutation& p)
{
    vec1<int> vec(p.size());
    for(int i : range1(p.size()) )
        vec[p[i]] = i;
    return Permutation(vec, p.size());
}


inline int& PermSharedData::operator[](int i)
{
    D_ASSERT(i > 0 && i <= size());
#ifndef EAGER_MERGE
    if(data_m[i] == 0)
    {
        int pos = i;
        for(int j : range1(perms.size()) )
        {
            pos = perms[j][pos];
        }
        data_m[i] = pos;
    }
#endif
    return data_m[i];
}

inline int& PermSharedData::raw(int i)
{
    D_ASSERT(i > 0 && i <= size());
    return data_m[i];
}

inline Permutation getRawPermutation(int n)
{
	return Permutation(makePermSharedData(n));
}

#ifdef EAGER_MERGE

inline Permutation mergePermutations(const vec1<Permutation>& v)
{
    if(v.size() == 0)
        return Permutation();
    if(v.size() == 1)
        return v[1];

    int max_moved = 0;
    for(int i : range1(v.size()))
    {
        max_moved = std::max(max_moved, v[i].size());
    }

    vec1<int> vec(max_moved);

    for(int i : range1(max_moved))
    {
        int pos = i;
        for(int j = 1; j <= v.size(); ++j)
        {
            pos = v[j][pos];
        }
        vec[i] = pos;
    }

    return Permutation(vec);
}

/*
template<typename T>
PermSharedData* makePermSharedDataFromPermList(const vec1<Permutation>& data)
{ return makePermSharedDataFromContainer(mergePermutations(data)); }
*/
#else

inline PermSharedData* makePermSharedDataFromPermList(const vec1<Permutation>& data)
{
    int maxval = 0;
    for(int i : range1(data.size()))
        maxval = std::max(maxval, data[i].size());
    PermSharedData* psd = makePermSharedData(maxval);
    psd->perms = data;

    return psd;
}

inline Permutation mergePermutations(const vec1<Permutation>& v)
{
    if(v.size() == 0)
        return Permutation();
    if(v.size() == 1)
        return v[1];

    PermSharedData* psm = makePermSharedDataFromPermList(v);

    return Permutation(psm);
}



#endif

class PermutationStack
{
    vec1<Permutation> v;
public:
    PermutationStack() { }

    PermutationStack(const Permutation& p)
    { v.push_back(p); }

    void addPerm(const Permutation& p)
    { v.push_back(p); }

    int operator[](int val) const
    {
        for(int i : range1(v.size()))
        {
            val = v[i][val];
        }
        return val;
    }

    Permutation getPerm() const
    { return mergePermutations(v); }
};


// Mapping GAP <-> C++ 

#ifndef YAPB_NO_GAP

namespace GAPdetail {
    
template<>
struct GAP_getter<Permutation>
{
    Permutation operator()(Obj rec) const
    {
        if(TNUM_OBJ(rec) == T_PERM2)
        {
            UInt deg = DEG_PERM2(rec);
            Permutation p = getRawPermutation(deg);
            vec1<int> v(deg);
            UInt2* ptr = ADDR_PERM2(rec);
            for(UInt i = 0; i < deg; ++i)
                p.raw(i+1) = ptr[i] + 1;
            D_ASSERT(p.validate());
            return p;
        }
        else if(TNUM_OBJ(rec) == T_PERM4)
        {
            UInt deg = DEG_PERM4(rec);
            Permutation p = getRawPermutation(deg);
            UInt4* ptr = ADDR_PERM4(rec);
            for(UInt i = 0; i < deg; ++i)
                p.raw(i+1) = ptr[i] + 1;
            return p;
        }
        else
            throw GAPException("Invalid attempt to read perm");
    }
};

template<>
struct GAP_maker<Permutation>
{
    Obj operator()(const Permutation& p) const
    {
        UInt4 deg = p.size();
        // ignore tperm2 for now.
        Obj prod = NEW_PERM4(deg);
        UInt4* pt = ADDR_PERM4(prod);
        for(UInt i = 0; i < deg; ++i)
            pt[i] = p[i+1] - 1;
        return prod;
    }
};

}

#endif

#endif

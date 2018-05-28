#ifndef __EnumSet_hpp_2018_04_24_10_08
#define __EnumSet_hpp_2018_04_24_10_08

#include <type_traits>
#include <cstdint>
#include <cstdarg>
#include <initializer_list>
#include "Hash.hpp"

template<typename E> class EnumSet:public Hashable{
    static_assert(std::is_enum<E>::value,"Enum Set cannot be used except by an enum");
    typedef E value_type;
    typedef std::underlying_type<E>::type underlying_type;
    uint64_t map;
    static uint64_t getBit(value_type v){
        return 1LL<<(static_cast<const std::underlying_type<E>::type&>(v));
    }
    
    class iterator{
    private:
        typedef E value_type;
        typedef const EnumSet<E>& owner_type;
        static value_type getValue(int bit){
            return static_cast<const E&>(bit);
        }
        uint64_t state;
        int bit;
        int nextSetBit(int bit){
            while((1uLL<<(++bit)&state)!=0)
                if(bit>64)
                    return -1;
            return bit;
        }
    public:
        iterator(uint64_t map):state(map),bit(0){
            bit = nextSetBit(0);
        }
        iterator(uint64_t map,int bit){}
        iterator& operator++(int){
            bit = nextSetBit(bit);
            return *this;
        }
        value_type operator*()const{
            if(bit<0)
                throw "Iterator finished";
            return getValue(bit);
        }
        bool operator==(const iterator& i){
            return map==i.map&&bit==i.bit;
        }
    };
    
public:
    EnumSet():map(0){}
    EnumSet(E val1):map(0){
       add(val1);
    }
    EnumSet(E val1,E val2):map(0){
        add(val1);
        add(val2);
    }
    EnumSet(E val1,E val2,E val3):map(0){
      add(val1);
      add(val2);
      add(val3);
    }
    template<size_t N> EnumSet(E(&vals)[N]){
      for(E v:vals)
          add(v);
    }
    template<size_t N> EnumSet(array<E,N>& vals){
     for(E v:vals)
        add(v);
    }
    /*EnumSet(const initializer_list<E>& e){
        for(E v:e)
            add(v);
    }*/
    EnumSet(uint64_t map):map(map){}
    bool add(value_type v){
        if(contains(v))
            return false;
        else
            map |=getBit(v);
        return true;
    }
    uint64_t toMap()const{
        return map;
    }
    bool contains(value_type v)const{
        return (map&getBit(v))!=0;
    }
    bool remove(value_type v){
        if(!contains(v))
            return false;
        else   
            map &=~getBit(v);
        return true;
    }
    iterator begin()const{
        return iterator(map);
    }
    iterator end()const{
        return iterator(map,-1);
    }
    int32_t hashCode()const{
        int hash = 0;
        for(value_type v:*this){
            hash *= 31;
            hash += hashcode<underlying_type>(static_cast<const underlying_type&>(v));
        }
        return hash;
    }
};

#endif

#pragma once

#include <iostream>
#include <ostream>
#include <sstream>

#define NAME_VALUE(v)#v," : ",(v)

namespace zt {
    template<typename T>
    class Range{
    private:
    const T _start,_end,_step;
    public:
        constexpr Range()=delete;
        constexpr Range(const T start,const T end,const T step)noexcept:_start(start),_end(end),_step(step){}
        constexpr Range(const T start,const T end)noexcept:Range(start,end,1){}
        constexpr Range(const T end)noexcept:Range(0,end,1){}

        struct Iterator{
            T current;
            const T step;
            
            constexpr Iterator()=delete;
            constexpr Iterator(const T start,const T step)noexcept:current(start),step(step){}
            constexpr bool operator!=(const Iterator &other)const noexcept{
                return current<other.current;
            }
            constexpr Iterator& operator++()noexcept{
                current+=step;
                return *this;
            }
            constexpr T operator*()const noexcept{
                return current;
            }
        };

        constexpr Iterator begin()const{
            return Iterator(_start,_step);
        }
        constexpr Iterator end()const{
            return Iterator(_end,_step);
        }
    };

    template<typename ...Args>
    inline void raw_print(std::ostream &os,const Args&...args){
        std::ostringstream ss;
        ((ss<<args),...);
        os<<ss.str();
        return;
    }

    template<typename ...Args>
    inline void print(const Args&...args){
        raw_print(std::cout,args...);
        return;
    }
    inline void print(){
        return;
    }

    template<typename ...Args>
    inline void eprint(const Args&...args){
        raw_print(std::cerr,args...);
        return;
    }

    inline void eprint(){
        return;
    }

    // template <class ...Args>
    // inline void check_fstream_isopen(const Args&...args)noexcept(false){
    //     bool is_open=true;
    //     (((!args.is_open())?is_open=false:true),...);
    //     if(is_open==false){
    //         throw std::runtime_error("cannot open file stream ");
    //     }
    // }
}
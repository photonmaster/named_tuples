#ifndef NAMED_TUPLE_H
#define NAMED_TUPLE_H
#include <tuple>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <string>
#include <functional>
#include <typelist.h>

using std::tuple;


#define DEF_SYMBOL(x)  \
    struct x { \
    static constexpr char tag_name[] = #x; \
    }; \
    constexpr char x::tag_name[]; \


template<class T, class U,const char* tag_name = T::tag_name>
struct tag
{
    using first = T;
    using second = U;
};



template <typename ...Ts>
struct object : public std::tuple<typename Ts::second...>
{
    template<typename... Args> constexpr object(Args &&...args):
        std::tuple<typename Ts::second...>(std::forward<Args>(args)...) {}

    constexpr object() = default;

    constexpr object(object&&) = default;
    constexpr object(const object&) = default;

    object &operator=(object&&) = default;
    object &operator=(const object&) = default;

    using tags = tmp::typelist<typename Ts::first...>;
    static constexpr size_t object_size = sizeof...(Ts);



    template<typename T>
    auto&
    get()
    {
        return std::get<tmp::index<tags,T>::value>(*this);
    }

    template<typename T>
    constexpr const auto&
    operator[](const T&&) const
    {
        return std::get<tmp::index<tags,T>::value>(*this);
    }

    template<typename T>
    constexpr auto&
    operator[](const T&&)
    {
        return std::get<tmp::index<tags,T>::value>(*this);
    }



    template<typename T>
    constexpr const char*
    name() const
    {
        return tmp::at<tags,tmp::index<tags,T>::value>::tag_name;

    }

    template<int I>
    constexpr const char*
    name() const
    {
        return tmp::at<tags,I>::tag_name;
    }

    //TODO: remove this
    static constexpr const char* descriptions_[object_size] = {Ts::first::tag_name...};
};

//for_each

namespace details
{
//c++14 has index_sequence
template<size_t... Is>
struct index_sequence {
};

template<size_t N, size_t... Is>
struct gen_index_sequence : gen_index_sequence<N - 1, N - 1, Is...> {

};

template<size_t... Is>
struct gen_index_sequence<0, Is...> : index_sequence<Is...>{};

//generic for_each
template <typename T, typename F, std::size_t ...Is>
void for_each_impl(T&& t, F&& f, index_sequence<Is...>) {
    auto l = { (f (t.template name<Is>(), std::get<Is>(std::forward<T>(t)) ), 0)... }; //std::initialiser_list
}
}//namespace details


template <typename T, typename F>
void for_each(T&& tuple, F&& f) {
    constexpr size_t N = tuple.object_size;
    details::for_each_impl(std::forward<T>(tuple), std::forward<F>(f), details::gen_index_sequence<N>{});
}



//atach
//takes in a std::tuple and a typlist
//and outputs an object



namespace tmp{
template<class L1, class L2>//typelist, tuple
struct attach_impl;

template<template<class...> class L1,template<class...> class L2, class...A1,class...A2>
struct attach_impl<L1<A1...>,L2<A2...>> {
    using type = object<tag<A1,A2>...>;
};

template<class L1, class L2>
using attach = attach_impl<L1,L2>;
}

template <typename...L1,typename L2> //TODO: overload for rv reference
decltype(auto)
attach(const L2& tuple)
{
    typename tmp::attach<tmp::typelist<L1...>,L2>::type obj(tuple);
    return obj;
}


//detach: remove tag description from named tuple. That is degenerate to simple tuple.

namespace tmp{
template<class L>//object
struct dettach_impl;

template<class...A1,class...A2>
struct dettach_impl<object<tag<A1,A2>...>> {
    using type = std::tuple<A2...>;
};

template<class L>
using dettach = typename dettach_impl<L>::type;
}




template<class...T>
decltype(auto) merge(T&&... args)
{
    return std::tuple_cat((tmp::dettach<T>)(std::forward<T>(args))...);
}












//template <typename...L1,typename...L2,typename L> //TODO: overload for rv reference
//decltype(auto)
//dettach(const L& object)
//{
//    typename tmp::dettach<object<tag<L1,L2>...>::type tuple(object);
//    return obj;
//}









//select a part of a object

//namespace tmp{
//template<class L1, class L2>//typelist, tuple
//struct select_impl;

//template<template<class...> class L1,template<class...> class L2, class...A1,class...A2>
//struct select_impl<L1<A1...>,L2<A2...>> {
//    using type = object<tag<A1,A2>...>;
//};

//template<class L1, class L2>
//using attach = attach_impl<L1,L2>;

//}


//template <typename...L1,typename L2> //TODO: overload for rv reference
//decltype(auto)
//attach(const L2& tuple)
//{
//    typename tmp::attach<tmp::typelist<L1...>,L2>::type obj(tuple);
//    return obj;
//}




//template <typename... L1, typename ...L2>
//decltype(auto)
//merge(object<L1...>& t1,object<L2...>& t2)
//{
//    using type = typename tmp::merge<tmp::typelist<typename L1::first...>, tmp::typelist<typename L2::first...>>;


//    return   std::make_tuple(t1, t2);
//}




//get follows a path from a parent object to subobjects UNDER CONSTRUCTION


//template<typename L>
//struct get_impl;


//template<template <class...> Tl, class... A>
//struct get_impl<Tl<A...>> {

//    using path = tmp::reverse<Tl>;

//    template<typename L>
//    auto apply(L&& ls){


//    }

//    template<typename L>
//    auto operator()(L&& ls) const
//    {
//        return apply(tmp::at<path,std::integral_constant<size_t, sizeof ... (A) - 1>::value>, std::forward<L>(ls));
//    }


//}


template<typename ... Fs>
struct compose_impl
{
    compose_impl(Fs&& ... fs) : functionTuple(std::forward_as_tuple(fs ...)) {}

    template<size_t N, typename ... Ts>
    auto apply(std::integral_constant<size_t, N>, Ts&& ... ts) const
    {
        return apply(std::integral_constant<size_t, N - 1>(), std::get<N>(functionTuple)(std::forward<Ts>(ts)...));
    }

    template<typename ... Ts>
    auto apply(std::integral_constant<size_t, 0>, Ts&& ... ts) const
    {
        return std::get<0>(functionTuple)(std::forward<Ts>(ts)...);
    }

    template<typename ... Ts>
    auto operator()(Ts&& ... ts) const
    {
        return apply(std::integral_constant<size_t, sizeof ... (Fs) - 1>(), std::forward<Ts>(ts)...);
    }

    std::tuple<Fs ...> functionTuple;
};

template<typename ... Fs>
auto compose(Fs&& ... fs)
{
    return compose_impl<Fs ...>(std::forward<Fs>(fs) ...);
}







//template < typename Func, typename T >
//T  compose_impl( Func &&, T &&x, std::integral_constant<std::size_t, 0> )
//{ return std::forward<T>(x); }

//template < typename Func, typename T, std::size_t N >
//T compose_impl( Func &&f, T &&x, std::integral_constant<std::size_t, N> )
//{
//    return compose_impl( std::forward<Func>(f),
//     std::forward<Func>(f)(std::forward<T>( x )),
//     std::integral_constant<std::size_t, N-1>{} );
//}

//template < std::size_t Repeat = 1, typename Func, typename T >
//T  compose( Func &&f, T &&x )
//{
//    return compose_impl( std::forward<Func>(f), std::forward<T>(x),
//     std::integral_constant<std::size_t, Repeat>{} );
//}










//template<template<class...> class L, class T1, class... T>
//struct at_impl<L<T1, T...>, 0>
//{
//    using type = T1;
//};


//template <class L1,class L2>
//struct get_impl;


//template <template <class...> L1,class A1,class... T,class L2>
//struct get_impl<L1<A1,T...>,L2>{
//    using type = A1;

//}



//template <typename...L1,typename L2>
//get(const L2& object){
//    object.get<L1>

//}



//template <typename...L1,typename L2>
//get<class A1, class A2>(const L2& object){
//    object.get<L1>




//template <typename...L1,typename L2>
//get<>(const L2& object){
//    object.get<L1>


//}







#endif // NAMED_TUPLE_H

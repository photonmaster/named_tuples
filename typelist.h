#ifndef TYPELIST_H
#define TYPELIST_H

#include <type_traits> //intergral_constant

#include <type_traits>
#include <typeinfo>

#include <memory>
#include <string>
#include <cstdlib>


//Thanks to Peter Dimov:
//http://pdimov.com/cpp2/simple_cxx11_metaprogramming.html
//http://pdimov.com/cpp2/simple_cxx11_metaprogramming_2.html

namespace tmp{

template<class... T> struct typelist {};

//size
namespace detail {
template<class T> struct size;
template<class... T>
struct size<typelist<T...>>
{
    using type = std::integral_constant<std::size_t, sizeof...(T)>;
};
}

template<class T> using size = typename detail::size<T>::type;



template<class... T>
using length = std::integral_constant<std::size_t,sizeof...(T)>;


namespace detail {
template<class L, class... T>
struct push_front;

template<template <class...> class L,class...A,  class... T>
struct push_front<L<A...>,T...>{

    using type = L<T...,A...>;
};
}
template<class L, class... T>
using push_front = detail::push_front<L,T...>;


//transform
//It takes a metafunction and aplies it on a list producing a new list...
namespace detail {
template<template<class...> class F, class L> struct transform;
template<template<class...> class F, template<class...> class L, class... T>
struct transform<F, L<T...>>
{
    using type = L<F<T>...>;
};
}

template<template<class...> class F, class L>
using transform = typename detail::transform<F, L>::type;


//transform with a metafunction taking two parameters

namespace detail {

template<template <class...> class F, class L1, class L2>
struct transform_2;

template<template<class...> class F, template<class...> class L1,template<class...> class L2, class...A1,class...A2>
struct transform_2<F,L1<A1...>,L2<A2...>> {
    using type = L1<F<A1,A2>...>;
};

}

template<template <class...> class F, class L1, class L2>
using transform_2 = detail::transform_2<F,L1,L2>;



//merge, takes a series of lists and creates one typlist holding the types of all of them

namespace detail {

template<class... L>
struct merge;

template <>
struct merge<>{
    using type = typelist<>;
};

template <template <class...> class L, class...A>
struct merge<L<A...>>{
    using type = L<A...>;
};

template <template <class...> class L1, template <class...> class L2, class...A1, class... A2, class...R>
struct merge<L1<A1...>,L2<A2...>,R...>{
    using type = merge<L1<A1...,A2...>, R...>;
};

}

template <class... L>
using merge = typename detail::merge<L...>::type; //recursion: type added

namespace detail {
//at (recursive form)
//you give an index and the type is returned...
template<class L, std::size_t I> struct at;

template<template<class...> class L, class T1, class... T>
struct at<L<T1, T...>, 0>
{
    using type = T1;
};

template<template<class...> class L, class T1, class... T, std::size_t I>
struct at<L<T1, T...>, I>
{
    using type = typename at<L<T...>, I-1>::type;
};
}

template<class L, std::size_t I> using at = typename detail::at<L, I>::type;


namespace detail {

template<std::size_t N> using size_t = std::integral_constant<std::size_t, N>;

}



namespace detail{
template<class L, class V> struct index;


template<template<class...> class L, class V> struct index<L<>, V>
{
    using type = size_t<0>;
};

template<template<class...> class L, class... T, class V>
struct index<L<V, T...>, V>
{
    using type = size_t<0>;
};

template<template<class...> class L, class T1, class... T, class V>
struct index<L<T1, T...>, V>
{
    using type = size_t<1 + index<L<T...>, V>::type::value>;
};
}

template<class L, class V> using index = typename detail::index<L, V>::type;

//append
namespace detail
{
template<class L, class A> struct append;

template<template<class...> class L,class...T, class A>
struct append<L<T...>,A>
{
    using type = L<T...,A>;
};
}

template<class L, class A>
using append = detail::append<L,A>;


//reverse
//given a typelist<A1,A2,A3> the typlist<A3,A2,A1> is produced

namespace detail
{
template <class L>
struct reverse;

template<template <class...> class L, class... A, class A1>
struct reverse<L<A1,A...>>
{
    using type = typename append<typename reverse< L<A...>>::type,A1>::type;

};

template<template <class...> class L>
struct reverse<L<>>
{
    using type = L<>;
};
}

template <class L>
using reverse = typename detail::reverse<L>::type;


//merging two tuples:

//template< typename L1, typename L2>
//struct merge;

//template< typename ... A1, typename ... A2 >
//struct merge< typelist< A1 ... >, typelist<A2 ... > >
//{
//    using type  = typelist< A1 ..., A2 ... >;

//};

}//namespace


#endif // TYPELIST_H

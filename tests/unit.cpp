#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "named_tuple.h"
#include "typelist.h"
#include <typeinfo>
#include "json_encoder.h"

#ifndef _MSC_VER
#   include <cxxabi.h>
#endif

using namespace std;



//Thanks Howard Hinnart:  http://stackoverflow.com/questions/18369128/how-can-i-see-the-type-deduced-for-a-template-type-parameter
template <class T>
std::string
type_name()
{
    typedef typename std::remove_reference<T>::type TR;
    std::unique_ptr<char, void(*)(void*)> own
            (
            #ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                    nullptr, nullptr),
            #else
                nullptr,
            #endif
                std::free
                );
    std::string r = own != nullptr ? own.get() : typeid(TR).name();
    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}


namespace symbols{
DEF_SYMBOL(points)
DEF_SYMBOL(x)
DEF_SYMBOL(y)
DEF_SYMBOL(z)
}

namespace s = symbols;

typedef object<
tag<s::x,int>,
tag<s::y,int>,
tag<s::z,int>
> point;

typedef object<
tag<s::points,std::vector<point>>
> shape;


TEST_CASE("typelist")
{
    SECTION("transform")
    {
        typedef tmp::typelist<int,int,int> input;
        typedef tmp::typelist<int*,int*,int*> expected_output;
        typedef tmp::transform<std::add_pointer,input> output;
        REQUIRE(bool(std::is_same<int,long>::value) == true);
    }

    SECTION("transform_2")
    {
        typedef std::pair<std::tuple<char,int,double>,std::tuple<string,string,string>> input;
        typedef std::tuple<std::pair<char,std::string>,std::pair<int,std::string>,std::pair<double,std::string>> expected_output;
        typedef tmp::transform_2<std::pair,input::first_type, input::second_type>::type output;
        REQUIRE((std::is_same<output,expected_output>::value) == true);
    }

    SECTION("merge")
    {
        using input_1 =  tmp::typelist<char, int,float,double>;
        using input_2 = std::pair<long int, long long>;
        using expected_output = tmp::typelist<char,int,float,double,long int, long long>;
        using output = tmp::merge<input_1,input_2>;
        REQUIRE((std::is_same<output,expected_output>::value) == true);
    }


    SECTION("append")
    {
        using input =  tmp::typelist<char, int,float,double>;
        using output = tmp::append<input,long long>::type;
        using expected_output = tmp::typelist<char, int,float,double, long long>;
        REQUIRE((std::is_same<output,expected_output>::value) == true);
    }

    SECTION("append 2")
    {
        using input =  tmp::typelist<>;
        using output = tmp::append<input,long long>::type;
        using expected_output = tmp::typelist<long long>;
        REQUIRE((std::is_same<output,expected_output>::value) == true);
    }

    SECTION("reverse")
    {
        //testing tmp::reverse
        std::cout << "testing tmp::reverse" << std::endl;
        using input =  tmp::typelist<int,double>;
        using output = tmp::reverse<input>;
        using expected_output = tmp::typelist<double, int>;
        REQUIRE((std::is_same<output,expected_output>::value) == true);
    }
}


TEST_CASE("named_tuples")
{
    SECTION("simple")
    {

        shape triangle;
        point a{1,1,1};
        std::cout << a[s::x()] << std::endl;
        std::cout << a[s::y()] << std::endl;
        std::cout << a[s::z()] << std::endl;

       // shape triangle{points}
    }

    SECTION("attach")
    {


        std::vector<int> x = {10,20,30,40,50,40,60,42,6,2,67,31,43,1,12,7,39,18,1,3};
        std::vector<int> y = {1,2,3,4,5,4,6,42,6,2,67,31,43,1,12,7,39,18,1};
        std::vector<int> z = {1,2,3,4,5,4,6,42,6,2,67,31,43,1,12,7,39,18,1};

        auto points = attach<s::x,s::y,s::z>(make_tuple(x,y,z));
        std::cout << "points object:\n " << json_encode(points) << '\n';
    }
}



//int main()
//{

//    //testing detach
//    std::cout << "testing detach" << std::endl;
//    using type_ = tmp::dettach<playRequest>;

//    auto f = attach<s::x,s::y,s::z,s::z,s::z,s::z>(std::tuple_cat((tmp::dettach<playRequest>)playRequest(),make_tuple(2,22)));
//    std::cout << "after cat:\n " << json_encode(f) << '\n';
//    std::cout << "output:" <<  '\n' << type_name<decltype(f)>() << '\n';

//    //testing merge
//    std::cout << "testing merge" << std::endl;
//    //using type_ = tmp::dettach<playRequest>;

//    auto test = merge(playRequest(), playRequest());
//    //   std::cout << "after cat:\n " << json_encode(test) << '\n';
//    std::tuple_size<std::tuple<int,int>>::value;

//    a.get<s::authorization>() = "dfgdfkdfkgjdlfgjdlfgghlfgsdksdgkd";
//    std::cout << "auth:\n " << a.get<s::authorization>() << '\n';
//    std::cout << "playRequest:\n " << json_encode(a) << '\n';

//    a[s::ticketData()][s::gameID()] = 123;
//    a[s::authorization()] = "fsdfsdf";
//    std::cout << "playRequest:\n " << json_encode(a) << '\n';
//    std::cin.get();
//}


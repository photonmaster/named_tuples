#ifndef JSON_ENCODER_H
#define JSON_ENCODER_H

#include "named_tuple.h"
#include <sstream>



//taken mostrly from https://github.com/matt-42/iod


// Encode \o into a json string.
template <typename T>
inline std::string json_encode(const std::vector<T>& v);

template <typename... T>
inline std::string json_encode(const object<T...>& o);


namespace json_internals
{

template <typename S, typename ...A>
inline void json_encode_(const object<A...>& obj, S& stream);

template <typename T, typename S>
inline void json_encode_(const T& t, S& ss)
{
    ss << t;
}

template <typename S>
inline void json_encode_(const char* t, S& ss)
{
    ss << '"' << t << '"';
}


template <typename S>
inline void json_encode_(const std::string& t, S& ss)
{
    json_encode_(t.c_str(), ss);
}

// Forward declaration.
template <typename T, typename S>
inline void json_encode_(const std::vector<T>& array, S& ss)
{
    ss << '[';
    for (const auto& t : array)
    {
        json_encode_(t, ss);
        if (&t != &array.back())
            ss << ',';
    }
    ss << ']';
}

template <typename S, typename ...A>
inline void json_encode_(const object<A...>& obj, S& stream)
{
    stream << '{';
    bool first = true;
    for_each(obj,[&] (const char* name, const auto& m){
        if (!first) { stream << ','; }
        first = false;
        json_encode_(name, stream);
        stream << ':';
        json_encode_(m, stream);
    });
    stream << '}';
}
} //namespace



template<typename ...T>
inline std::string json_encode(const object<T...> &obj)
{

    std::stringstream stream;
    json_internals::json_encode_(obj, stream);
    return stream.str();
}



  template <typename T>
  inline std::string json_encode(const std::vector<T>& v)
  {
    std::stringstream stream;
    json_internals::json_encode_(v, stream);
    return stream.str();
  }


#endif // JSON_ENCODER_H

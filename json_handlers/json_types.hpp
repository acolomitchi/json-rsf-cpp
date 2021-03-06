/*
 * json_types.hpp
 *
 *  Created on: 22 Aug 2016
 *      Author: acolomitchi
 */

#ifndef JSON_TYPES_HPP_
#define JSON_TYPES_HPP_

#include <string>
#include <type_traits>
#include <boost/optional.hpp>

#include "../common/exceptions.hpp"
#include "../common/date_types.hpp"

namespace jsonrsf {

template <typename T> struct array_utils {
  static constexpr bool is_array=false;
  static std::size_t size(const T& x) {
    assert(is_array); // this should stop it while running a debug build
    // otherwise return an invalid value
    return static_cast<std::size_t>(-1);
  }
};

template <typename T>
struct array_utils<std::vector<T>> {
  static constexpr bool is_array=true;

  static std::size_t size(const std::vector<T>& storage) {
    return storage.size();
  }

  static void push_back(std::vector<T>& dest, const T& elem) {
    dest.push_back(elem);
  }
  static void push_back(std::vector<T>& dest, T&& elem) {
    dest.push_back(elem);
  }

  static T& back(std::vector<T>& dest) {
    return dest.back();
  }

  static std::vector<T>* addr(std::vector<T>& dest) {
    return &dest;
  }
};

template <typename T>
struct array_utils<boost::optional<std::vector<T>>> {
  static constexpr bool is_array=true;

  static std::size_t size(const boost::optional<std::vector<T>>& storage) {
    return storage ? (*storage).size() : 0;
  }
  static void push_back(boost::optional<std::vector<T>>& dest, const T& elem) {
    if(false==dest) {
      throw state_error("Attempt to push an element in a non-initialized optional array");
    }
    (*dest).push_back(elem);
  }
  static void push_back(boost::optional<std::vector<T>>& dest, T&& elem) {
    if(false==dest) {
      throw state_error("Attempt to push an element in a non-initialized optional array");
    }
    (*dest).push_back(elem);
  }
  static T& back(boost::optional<std::vector<T>>& dest) {
    return (*dest).back();
  }
  static std::vector<T>* addr(boost::optional<std::vector<T>>& dest) {
    return dest ? dest.get_ptr() : NULL;
  }
};


template <typename T> struct is_json_primitive {
  static constexpr bool value=
         std::is_arithmetic<T>::value
      || std::is_enum<typename std::remove_cv<T>::type>::value
      || std::is_same<std::string, typename std::remove_cv<T>::type>::value
      || std::is_same<
             jsonrsf::datetime_type, // used for datetime
             typename std::remove_cv<T>::type
         >::value
      || std::is_same< // used for date
             jsonrsf::date_type,
             typename std::remove_cv<T>::type
         >::value
      || std::is_same< // used for time
             jsonrsf::daytime_type,
             typename std::remove_cv<T>::type
         >::value
   ;
};

template <typename T> struct is_temporal {
  static constexpr bool value=
       std::is_same<
           jsonrsf::datetime_type, // used for datetime
           typename std::remove_cv<T>::type
       >::value
    || std::is_same< // used for date
           jsonrsf::date_type,
           typename std::remove_cv<T>::type
       >::value
    || std::is_same< // used for time
           jsonrsf::daytime_type,
           typename std::remove_cv<T>::type
       >::value
    ;
};

template <typename T> struct handled_by_num :
  public std::integral_constant<bool,
         std::is_arithmetic<T>::value
      || std::is_enum<typename std::remove_cv<T>::type>::value
  >
{
};


template <typename T>
struct handled_by_num<boost::optional<T>> :
  public std::integral_constant<bool,
         std::is_arithmetic<T>::value
      || std::is_enum<typename std::remove_cv<T>::type>::value
  >
{
};

template <typename T>
struct handled_by_num<std::vector<T>> :
  public std::integral_constant<bool,
         std::is_arithmetic<T>::value
      || std::is_enum<typename std::remove_cv<T>::type>::value
  >
{
};

template <typename T>
struct handled_by_num<boost::optional<std::vector<T>>> :
  public std::integral_constant<bool,
         std::is_arithmetic<T>::value
      || std::is_enum<typename std::remove_cv<T>::type>::value
  >
{
};


template <typename T> struct handled_by_str :
  public std::integral_constant<bool,
       std::is_same<T, std::string>::value
    || std::is_same<T, char32_t>::value
    || is_temporal<T>::value
  >
{
};

template <typename T>
struct handled_by_str<boost::optional<T>>
{
  static constexpr bool value=handled_by_str<T>::value;
};

template <typename T>
struct handled_by_str<std::vector<T>> :
  public std::integral_constant<bool, handled_by_str<T>::value>
{
};

template <typename T>
struct handled_by_str<boost::optional<std::vector<T>>>
{
  static constexpr bool value=handled_by_str<T>::value;
};


template <typename T> struct handled_by_primitive;

template <typename T> struct handled_by_primitive
{
  static constexpr bool value=is_json_primitive<T>::value;
};

template <typename T> struct handled_by_primitive<boost::optional<T>>
{
  static constexpr bool value=is_json_primitive<T>::value;
};

template <typename T> struct handled_by_primitive<std::vector<T>>
{
  static constexpr bool value=is_json_primitive<T>::value;
};

template <typename T> struct handled_by_primitive<boost::optional<std::vector<T>>>
{
  static constexpr bool value=is_json_primitive<T>::value;
};



// top level storage types
template <
  typename T, bool asArray=false,
  bool asRequired=true, typename E=void
>
struct deduce_storage_type_base;

template <typename T, bool asRequired>
struct deduce_storage_type_base<
  T, false, asRequired,
  typename std::enable_if<is_json_primitive<T>::value>::type
> {
  using type=typename std::conditional<
    asRequired,
    T,
    boost::optional<T>
  >::type;

};

template <typename T, bool asRequired>
struct deduce_storage_type_base<
  T, true, asRequired,
  typename std::enable_if<is_json_primitive<T>::value>::type
> {
  using type=typename std::conditional<
    asRequired,
    std::vector<T>,
    boost::optional<std::vector<T>>
  >::type;
};

template <class T, bool asRequired>
struct deduce_storage_type_base<
  T, false, asRequired,
  typename std::enable_if<std::is_class<T>::value>::type
> {
  using type=typename std::conditional<
    asRequired,
    std::shared_ptr<T>, // just in case we want different...
    std::shared_ptr<T>  // ptr types between required/optional
  >::type;
};

template <class T, bool asRequired>
struct deduce_storage_type_base<
  T, true, asRequired,
  typename std::enable_if<std::is_class<T>::value>::type
> {
  using type=typename std::conditional<
    asRequired,
    std::vector<std::shared_ptr<T>>,
    boost::optional<std::vector<std::shared_ptr<T>>>
  >::type;
};

template <typename T, bool asArray=false, bool asRequired=true>
struct deduce_storage_type {
  using type=
      typename deduce_storage_type_base<T, asArray, asRequired>::type
  ;
};


} // namespace jsonrsf

#endif /* JSON_TYPES_HPP_ */

/*
 * assign_ops.hpp
 *
 *  Created on: 22 Aug 2016
 *      Author: acolomitchi
 */

#ifndef HANDLER_OPS_HPP_
#define HANDLER_OPS_HPP_

#include <stdexcept>
#include <vector>

#include "../common/utf8.hpp"
#include "../common/json_types.hpp"
#include "../common/exceptions.hpp"

namespace jsonrsf {
namespace sax {

//
// Null handlers

template <typename T> struct required_null_assigner {
  void operator()(T& target) {
    throw invalid_input("Attempt to assign null to a required value");
  }
};

template <typename T, bool enable, bool strict>
struct null_assigner_base;

template <typename T, bool strict>
struct null_assigner_base<T, true, strict> : public required_null_assigner<T> {
//  void operator()(T& target) {
//    throw invalid_input("Attempt to assign null to a required value");
//  }
};

template <typename T, bool strict>
struct null_assigner_base<boost::optional<T>, true, strict>
{
  void operator()(boost::optional<T>& target) {
    if(strict && target) {
      throw invalid_input("Attempt to reset to null an optional that has a value");
    }
    target=boost::none;
  }
};

template <typename T, bool strict>
struct null_assigner_base<std::vector<T>, true, strict>
{
  void operator()(std::vector<T>& target) {
    throw invalid_input("Attempt to insert null into an array");
  }
};

template <typename T, bool strict>
struct null_assigner_base<boost::optional<std::vector<T>>, true, strict>
{
  void operator()(boost::optional<std::vector<T>>& target) {
    // if target exists, it's an attempt to record a null into an array
    //   If so then if strict then throw else do nothing
    // if target doesn't exists, null insertion is likely to be requested
    //   before target is created: the correct reaction is to do nothing
    if(target && strict) {
      throw invalid_input("Attempt to insert null into an array");
    }
  }
};

template <class T, bool strict>
struct null_assigner_base<std::shared_ptr<T>, true, strict>
{
  void operator()(std::shared_ptr<T>& target) {
    if(strict && NULL!=target.get()) {
      throw invalid_input("Attempt to reset to null an optional structure that already has a value");
    }
    target.reset();
  }
};

template <class T, bool strict>
struct null_assigner_base<std::unique_ptr<T>, true, strict>
{
  void operator()(std::unique_ptr<T>& target) {
    if(strict && NULL!=target.get()) {
      throw invalid_input("Attempt to reset to null an optional structure that already has a value");
    }
    target.reset();
  }
};


// deals with all required-s
template <typename T, bool strict=true>
struct null_assigner :
    public null_assigner_base<T, true, true>
{
};

// specializations for optionals
template <typename T, bool strict>
struct null_assigner<boost::optional<T>, strict>:
    public null_assigner_base<boost::optional<T>, true, strict>
{
};

template <class T, bool strict>
struct null_assigner<std::shared_ptr<T>, strict>:
    public null_assigner_base<std::shared_ptr<T>, std::is_class<T>::value, strict>
{
};


template <class T, bool strict>
struct null_assigner<std::unique_ptr<T>, strict>:
    public null_assigner_base<std::unique_ptr<T>, std::is_class<T>::value, strict>
{
};


template <typename T, bool forceRequired=false> struct deduce_null_handler;

template <typename T> struct deduce_null_handler<T, false>
{
  using type=null_assigner<T, true>;
};

template <typename T>
struct deduce_null_handler<T, true> {
  using type=required_null_assigner<T>;
};


// Value handlers/transformers/converters
// forward declarations
template <typename T> struct deduce_val_handler;

template <typename T>
struct value_storer  {
  using value_type=
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type;
  using storage_type=
    typename std::enable_if<
        std::is_arithmetic<T>::value,
        T
    >::type;


  template <typename U> void operator()(storage_type& target, const U& value) {
    target=static_cast<value_type>(value);
  }
  // disable the rest of them
  void operator()(storage_type& target, const std::string& value) {
  }
  void operator()(storage_type& target, std::string&& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};

template <typename T>
struct value_storer<boost::optional<T>> {
  using value_type=
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type;
  using storage_type=
    typename std::enable_if<
        std::is_arithmetic<T>::value,
        boost::optional<T>
    >::type;

  template <typename U> void operator()(storage_type& target, const U& value) {
    target=static_cast<value_type>(value);
  }
  // disable the rest of them
  void operator()(storage_type& target, const std::string& value) {
  }
  void operator()(storage_type& target, std::string&& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};


template <>
struct value_storer<std::string> {
  using value_type=std::string;
  using storage_type=std::string;

  void operator()(storage_type& target, const value_type& value) {
    target=value;
  }
  void operator()(storage_type& target, value_type&& value) {
    target=value;
  }
  // disable the rest of them
  template <typename U> void operator()(storage_type& target, const U& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};

template <>
struct value_storer<boost::optional<std::string>> {
  using value_type=std::string;
  using storage_type=boost::optional<std::string>;

  void operator()(storage_type& target, const value_type& value) {
    target=value;
  }
  void operator()(storage_type& target, value_type&& value) {
    target=value;
  }
  // disable the rest of them
  template <typename U> void operator()(storage_type& target, const U& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};

template <typename ch32DestT> bool cnvSingleChar32(
    ch32DestT& dest, const char* utf8Src, std::size_t length
) {
  char32_t buff;
  const char* end=NULL;
  std::size_t len=1;
  bool toRet=utf8_services::buffUtf8ToUtf32(len, &buff, utf8Src, &end);
  if(toRet) {
    std::size_t cnvLen=end-utf8Src;
    if(cnvLen!=length) {
      // for a single character, we must consume the input exactly
      std::string msg("Failed to convert to a single char value (provided <");
      msg.append(utf8Src, utf8Src+length).append(">)");
      throw std::domain_error(msg.c_str());
    }
  }
  else {
    std::string msg("Malformed UTF-8 encoding (provided <");
    msg.append(utf8Src, utf8Src+length).append(">)");
    throw std::domain_error(msg.c_str());
  }
  dest=buff;
  return toRet;
}

template <>
struct value_storer<char32_t> {
  using value_type=std::string;
  using storage_type=char32_t;

  void operator()(storage_type& target, const value_type& value) {
    char32_t converted;
    // the next will throw if in error (return false only if the source is null and c_str() will never be null)
    cnvSingleChar32<char32_t>(converted, value.c_str(), value.length());
    target=converted;
  }
  void operator()(storage_type& target, value_type&& value) {
    char32_t converted;
    // the next will throw if in error (return false only if the source is null and c_str() will never be null)
    cnvSingleChar32<char32_t>(converted, value.c_str(), value.length());
    target=converted;
  }
  // disable the rest of them
  template <typename U> void operator()(storage_type& target, const U& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};

template <>
struct value_storer<boost::optional<char32_t>> {
  using value_type=std::string;
  using storage_type=boost::optional<char32_t>;

  void operator()(storage_type& target, const value_type& value) {
    char32_t converted;
    // the next will throw if in error (return false only if the source is null and c_str() will never be null)
    cnvSingleChar32<char32_t>(converted, value.c_str(), value.length());
    target=converted;
  }
  void operator()(storage_type& target, value_type&& value) {
    char32_t converted;
    // the next will throw if in error (return false only if the source is null and c_str() will never be null)
    cnvSingleChar32<char32_t>(converted, value.c_str(), value.length());
    target=converted;
  }
  // disable the rest of them
  template <typename U> void operator()(storage_type& target, const U& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};

// FIXME value storers for datetime, date, time.
///   In terms of std::chrono??? Or in terms of
//    https://github.com/HowardHinnant/date ???


// value_storers for structs/classes
template<typename T>
struct value_storer<std::shared_ptr<T>> {
  using elem_type=typename std::enable_if<std::is_class<T>::value,T>::type;
  using value_type=std::shared_ptr<elem_type>;
  using storage_type=std::shared_ptr<elem_type>;

  void operator()(storage_type& target, const value_type& value) {
    target=value;
  }
  void operator()(storage_type& target, value_type&& value) {
    target=value;
  }
  // disable the rest of them
  template <typename U> void operator()(storage_type& target, const U& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};

template<typename T>
struct value_storer<std::unique_ptr<T>> {
  using elem_type=typename std::enable_if<std::is_class<T>::value,T>::type;
  using value_type=std::unique_ptr<elem_type>;
  using storage_type=std::unique_ptr<elem_type>;

  void operator()(storage_type& target, const value_type& value) {
    target=value;
  }
  void operator()(storage_type& target, value_type&& value) {
    target=value;
  }
  // disable the rest of them
  template <typename U> void operator()(storage_type& target, const U& value) {
  }
  static bool init_storage(storage_type& target) {
    // do-nothing method - the assigment will be enough
    return true; // inited enough, so return true
  }
};


// val_handlers for json_primitives
template <typename T>
struct deduce_val_handler {
  using type=
    typename std::enable_if<
      is_json_primitive<T>::value,
      value_storer<T>
    >::type;
};
template <typename T>
struct deduce_val_handler<boost::optional<T>> {
  using type=
    typename std::enable_if<
      is_json_primitive<T>::value,
      value_storer<boost::optional<T>>
    >::type;
};

template <typename T>
struct deduce_val_handler<std::shared_ptr<T>> {
  using type=
    typename std::enable_if<
      std::is_class<T>::value,
      value_storer<std::shared_ptr<T>>
    >::type;
};

template <typename T>
struct deduce_val_handler<std::unique_ptr<T>> {
  using type=
    typename std::enable_if<
      std::is_class<T>::value,
      value_storer<std::unique_ptr<T>>
    >::type;
};

// array storers
template <class value_storer, bool optionalVec>
struct array_storer_base;

template <class value_storer>
struct array_storer_base<value_storer, false> {
  using value_type=typename value_storer::value_type;
  using storage_type=std::vector<value_type>;

  template <typename U> void operator()(storage_type& target, const U& value) {
    value_type val;
    value_storer h;
    h(val, value);
    target.push_back(val);
  }
};

template <class value_storer>
struct array_storer_base<value_storer, true> {
  using value_type=typename value_storer::value_type;
  using storage_type=boost::optional<std::vector<value_type>>;

  template <typename U> void operator()(storage_type& target, const U& value) {
    if(false==target) {
      // if one chooses to be lenient here and initialize the storage
      //   nasty things may happen in different contexts.
      // Think at the GenericHandlerBase, expecting an array, but getting a single
      //  value, without ArrayBegin being invoked. In such conditions, it is likely
      //  that the EndArray will never come and the handler doesn't get a chance
      //  to pop itself from the handler stack.
      throw std::runtime_error("Storage vector not initialized");
    }
    value_type val;
    value_storer h;
    h(val, value);
    (*target).push_back(val);
  }
};

template <typename T> struct array_storer;

template <typename T>
struct array_storer<std::vector<T>> {
  using value_handler=typename deduce_val_handler<T>::type;
  using base_storer_type=array_storer_base<value_handler, false>;

  using value_type=typename base_storer_type::value_type;
  using storage_type=typename base_storer_type::storage_type;

  template <typename U> void operator()(storage_type& target, const U& value) {
    base_storer_type delegate;
    delegate(target, value);
  }

  template <bool strict=true> static bool init_storage(storage_type& target) {
    return (false==strict || target.empty());
  }
};

template <typename T>
struct array_storer<boost::optional<std::vector<T>>> {
  using value_handler=typename deduce_val_handler<T>::type;
  using base_storer_type=array_storer_base<value_handler, true>;

  using value_type=typename base_storer_type::value_type;
  using storage_type=typename base_storer_type::storage_type;

  template <typename U> void operator()(storage_type& target, const U& value) {
    base_storer_type delegate;
    delegate(target, value);
  }
  template <bool strict=true> static bool init_storage(storage_type& target) {
    if(false==target) {
      std::vector<value_type> empty;
      target=std::move(empty);
      return true;
    }
    return (false==strict || target);
  }
};

template <typename T>
struct deduce_val_handler<std::vector<T>> {
  using type=array_storer<std::vector<T>>;
};
template <typename T>
struct deduce_val_handler<boost::optional<std::vector<T>>> {
  using type=array_storer<boost::optional<std::vector<T>>>;
};


} // namespace sax
} // namespace jsonrsf


#endif /* HANDLER_OPS_HPP_ */

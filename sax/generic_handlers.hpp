/*
 * basic_handlers.hpp
 *
 *  Created on: 27 Aug 2016
 *      Author: acolomitchi
 */

#ifndef GENERIC_HANDLERS_HPP_
#define GENERIC_HANDLERS_HPP_

#ifdef __cplusplus

#include <vector>
#include <memory>
#include <sstream>
#include <vector>
#include "primitive_ops.hpp"

namespace jsonrsf {
namespace sax {

// Forward declaration
class GenericSaxHandlerBase;

class handler_stack :
private std::vector<std::unique_ptr<GenericSaxHandlerBase>> {
  using super=std::vector<std::unique_ptr<GenericSaxHandlerBase>>;
public:

  using iterator=super::const_iterator;

  using value_type=
      typename std::vector<std::unique_ptr<GenericSaxHandlerBase>>::value_type
  ;
  handler_stack() :
    std::vector<std::unique_ptr<GenericSaxHandlerBase>>()
  {
  }
  ~handler_stack() {
    super::clear();
  }

  bool empty() const {
    return super::empty();
  }

  void clear() {
    super::clear();
  }

  const value_type& top() const {
    if(this->empty()) {
      throw std::runtime_error("empty");
    }
    return super::back();
  }

  void push(value_type&& value) {
    if(NULL==value.get()) {
      throw std::invalid_argument("cowardly refusing to push an empty handler");
    }
    super::push_back(std::move(value));
  }
  value_type pop() {
    if(this->empty()) {
      throw std::runtime_error("empty");
    }
    value_type toRet=std::move(super::back());
    super::pop_back();
    return toRet;
  }

  iterator begin() const {
    return super::cbegin();
  }
  iterator end() const {
    return super::cend();
  }
};

class GenericSaxHandlerBase
{
protected:
  handler_stack& trace_;

  void removeSelfIfTopTrace() {
    if(false==this->trace_.empty() && this==this->trace_.top().get()) {
      this->trace_.pop();
    }
  }

  GenericSaxHandlerBase(handler_stack& trace) : trace_(trace)
  {
  }
public:
  virtual ~GenericSaxHandlerBase() {
    this->removeSelfIfTopTrace();
  }
  virtual void handleNull() {
    throw invalid_input("`null` provided for a required value");
  }
  virtual void handleBool(bool value) {
    throw invalid_input("Unexpected call with bool value");
  }
  virtual void handleByte(int8_t value) {
    throw invalid_input("Unexpected call with int8_t value");
  }
  virtual void handleUByte(uint8_t value) {
    throw invalid_input("Unexpected call with uint8_t value");
  }
  virtual void handleShort(int16_t value) {
    throw invalid_input("Unexpected call with int16_t value");
  }
  virtual void handleUShort(uint16_t value) {
    throw invalid_input("Unexpected call with uint16_t value");
  }
  virtual void handleInt(int32_t value) {
    throw invalid_input("Unexpected call with int32_t value");
  }
  virtual void handleUInt(uint32_t value) {
    throw invalid_input("Unexpected call with uint32_t value");
  }
  virtual void handleLong(int64_t value) {
    throw invalid_input("Unexpected call with int64_t value");
  }
  virtual void handleULong(uint64_t value) {
    throw invalid_input("Unexpected call with uint64_t value");
  }
  virtual void handleFloat(float value) {
    throw invalid_input("Unexpected call with float value");
  }
  virtual void handleDouble(double value) {
    throw invalid_input("Unexpected call with double value");
  }
  virtual void handleString(const char* str, std::size_t length, bool copy) {
    throw invalid_input("Unexpected call with string value");
  }
  virtual void handleObjStart() {
    throw invalid_input("Unexpected call with ObjStart");
  }
  virtual void handleKey(const char* str, std::size_t length, bool copy) {
    throw invalid_input("Unexpected call with Key");
  }

  virtual void handleKey(const std::string& key) {
    this->handleKey(key.c_str(), key.length(), true);
  }
  virtual void handleObjEnd() {
    throw invalid_input("Unexpected call with ObjEnd");
  }
  virtual void handleArrayStart() {
    throw invalid_input("Unexpected call with ArrayStart");
  }
  virtual void handleArrayEnd() {
    throw invalid_input("Unexpected call with ArrayEnd");
  }

  virtual void inferErrorLocation(std::string& resultHere)=0;
};

// forward declaration
template <typename T, bool asRequired=true>
struct deduce_sax_handler;

template <
  typename T,
  class null_handler,
  class value_handler
>
class GenericPrimitivesSaxHandler :
  public GenericSaxHandlerBase
{
public:
  using value_type=typename std::enable_if<
    handled_by_primitive<T>::value,
    typename value_handler::value_type
  >::type;
  using target_type=typename std::enable_if<
    handled_by_primitive<T>::value,
    typename value_handler::storage_type
  >::type;
protected:
  target_type& dest_;

  // switched to true if is handling arrays and a call into handleArrayStart was made
  bool arrayStartReceived_;


  template <typename U> bool handleNum(U& value) {
    if(handled_by_num<value_type>::value) {
      value_handler h;
      h(this->dest_, value);
      if(false==sax_array_utils<target_type>::is_array) {
        this->removeSelfIfTopTrace();
      }
      return true;
    }
    return false;
  }
public:
  GenericPrimitivesSaxHandler(target_type& dest, handler_stack& trace) :
    GenericSaxHandlerBase(trace), dest_(dest), arrayStartReceived_(false)
  {
  }

  virtual ~GenericPrimitivesSaxHandler() {
  }

  virtual void handleNull() {
    null_handler h;
    h(this->dest_);
    // if we got to here without exception, null was accepted
    this->removeSelfIfTopTrace();
  }
  virtual void handleBool(bool value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleBool(value);
    }
  }
  virtual void handleByte(int8_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleByte(value);
    }
  }
  virtual void handleUByte(uint8_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleUByte(value);
    }
  }
  virtual void handleShort(int16_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleShort(value);
    }
  }
  virtual void handleUShort(uint16_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleUShort(value);
    }
  }
  virtual void handleInt(int32_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleInt(value);
    }
  }
  virtual void handleUInt(uint32_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleUInt(value);
    }
  }
  virtual void handleLong(int64_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleLong(value);
    }
  }
  virtual void handleULong(uint64_t value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleULong(value);
    }
  }
  virtual void handleFloat(float value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleFloat(value);
    }
  }
  virtual void handleDouble(double value) {
    if(false==this->handleNum(value)) {
      GenericSaxHandlerBase::handleDouble(value);
    }
  }
  virtual void handleString(const char* val, std::size_t len, bool copy) {
    if(handled_by_str<value_type>::value) {
      if(NULL==val) {
        this->handleNull();
      }
      else {
        std::string v;
        if(std::string::npos==len) {
          std::string aux(val);
          v=std::move(aux);
        }
        else {
          std::string aux(val, len);
          v=std::move(aux);
        }
        value_handler h;
        h(this->dest_, std::move(v));
        if(false==copy) {
          delete[] val;
        }
        if(false==sax_array_utils<target_type>::is_array) {
          this->removeSelfIfTopTrace();
        }
      }
    }
    else {
      GenericSaxHandlerBase::handleString(val, len, copy);
    }
  }

  void handleString(const std::string& val) {
    this->handleString(val.c_str(), val.length(), true);
  }

  virtual void handleArrayStart() {
    if(false==sax_array_utils<target_type>::is_array) {
      GenericSaxHandlerBase::handleArrayStart();
    }
    else {
      if(
           this->arrayStartReceived_
        || false==value_handler::init_storage(this->dest_)
      ) {
        throw std::runtime_error("Array start signaled twice: arrays of arrays are not supported");
      }
      this->arrayStartReceived_=true;
    }
  }
  virtual void handleArrayEnd() {
    if(false==sax_array_utils<target_type>::is_array) {
      GenericSaxHandlerBase::handleArrayStart();
    }
    else {
      // job done.
      this->removeSelfIfTopTrace();
    }

  }

  virtual void inferErrorLocation(std::string& resultHere) {
    if(sax_array_utils<target_type>::is_array) {
      // this is a piece of code assumed to be optimized out for levels > -O0
      std::size_t pos=sax_array_utils<target_type>::size(this->dest_);
      if(this->trace_.top().get()!=this) {
        assert(pos>0);
        // I'm not on the top of the trace...
        pos=pos-1; //... so the error must have happened while processing the last element
      }
      // else: I'm top of the trace, the error must have happened
      //       while attempting an operation on a new element to be inserted
      std::ostringstream appender;
      appender << "[" << pos << "]";
      resultHere.append(appender.str());
    }
    // else do nothing, cannot infer anything on simple primitive values
  }
};

template <class T, bool asRequired=true> class GenericStructArraySaxHandler :
  public GenericSaxHandlerBase
{
public:
  using value_type=typename deduce_storage_type<T, false, true>::type;
  using storage_type=typename deduce_storage_type<T, true, asRequired>::type;

protected:
  storage_type& dest_;
  bool arrayStartReceived_;

public:
  GenericStructArraySaxHandler(storage_type& target, handler_stack& trace) :
    GenericSaxHandlerBase(trace), dest_(target), arrayStartReceived_(false)
  {
  }

  virtual ~GenericStructArraySaxHandler() {
  }

  virtual void handleNull() {
    using null_handler_type=typename deduce_null_handler<storage_type, asRequired>::type;
    null_handler_type nullHandler;
    nullHandler(this->dest_);
  }

  virtual void handleArrayStart() {
    using initer_type=typename deduce_val_handler<storage_type>::type;
    initer_type initer;
    if(
         this->arrayStartReceived_
      || false==initer.init_storage(this->dest_)
    ) {
     throw std::runtime_error("Array start signaled twice: arrays of arrays are not supported");
    }
    this->arrayStartReceived_=true;
  }
  virtual void handleArrayEnd() {
    this->removeSelfIfTopTrace();
  }
  virtual void handleObjStart() {
    // need to create a handler for the element,
    // push if on the stack and delegate the objStart to the handler
    sax_array_utils<storage_type>::push_back(this->dest_, value_type());

    using elemHandlerType=typename deduce_sax_handler<value_type>::type;
    value_type& last=sax_array_utils<storage_type>::back(this->dest_);
    elemHandlerType* handler=new elemHandlerType(last, this->trace_);
    this->trace_.push(std::unique_ptr<elemHandlerType>(handler));
    this->trace_.top()->handleObjStart();
  }

  virtual void inferErrorLocation(std::string& resultHere) {
    std::size_t pos=sax_array_utils<storage_type>::size(this->dest_);
    if(this->trace_.top().get()!=this) {
      assert(pos>0);
      // I'm not on the top of the trace...
      pos=pos-1; //... so the error must have happened while processing the last element
    }
    // else: I'm top of the trace, the error must have happened
    //       while attempting an operation on a new element to be inserted
    std::ostringstream appender;
    appender << "[" << pos << "]";
    resultHere.append(appender.str());
  }
};

template <typename T, bool asRequired>
struct deduce_sax_handler {
  using type=GenericPrimitivesSaxHandler<
    T,
    typename deduce_null_handler<T>::type,
    typename deduce_val_handler<T>::type
  >;
};

template <typename T, bool asRequired>
struct deduce_sax_handler<std::shared_ptr<T>, asRequired> {
  using type=
      typename std::enable_if<
        std::is_class<T>::value,
        typename deduce_sax_handler<T, asRequired>::type
      >::type;
};

template <typename T>
struct deduce_sax_handler<std::vector<std::shared_ptr<T>>> {
  // trigger SFINAE if not a class or missing sax_handler
  using elem_type_class=typename std::enable_if<std::is_class<T>::value, T>::type;
  using elem_type_handler=typename deduce_sax_handler<T, true>::type;

  using type=GenericStructArraySaxHandler<T, true>;
};

template <typename T>
struct deduce_sax_handler<boost::optional<std::vector<std::shared_ptr<T>>>> {
  // trigger SFINAE if not a class or missing sax_handler
  using elem_type_class=typename std::enable_if<std::is_class<T>::value, T>::type;
  using elem_type_handler=typename deduce_sax_handler<T, true>::type;

  using type=GenericStructArraySaxHandler<T, false>;
};


template <class T, class null_handler> class GenericStructSaxHandler :
  public GenericSaxHandlerBase
{
protected:
  std::shared_ptr<T>& dest_;

  boost::optional<std::string> currKey_;
  virtual GenericSaxHandlerBase* createHandlerForCurrKey()=0;

  GenericStructSaxHandler(std::shared_ptr<T> &target,  handler_stack& trace) :
    GenericSaxHandlerBase(trace), dest_(target) {

  }

public:

  virtual ~GenericStructSaxHandler() {

  }

  virtual void handleNull() {
    null_handler h;
    h(this->dest_);
    // if we got to here without exception, null was accepted
    this->removeSelfIfTopTrace();
  }

  virtual void handleObjStart() {
    if(this->dest_.get()) {
      throw invalid_input("Object already bound to a value");
    }
    this->dest_.reset(new T());
  }
  virtual void handleKey(const char* str, std::size_t length, bool copy) {
    // doh, old version of boost delivered by Ubuntu
    //this->currKey_.emplace(str, length);
    this->currKey_=std::string(str, length);

    GenericSaxHandlerBase* handler=this->createHandlerForCurrKey();
    if(NULL==handler) {
      std::string msg("Invalid or unhandled key/property name (received `");
      msg.append(this->currKey_.get()).append("`)");
      throw invalid_input(msg.c_str());
    }
    std::unique_ptr<GenericSaxHandlerBase> toPush(handler);
    this->trace_.push(std::move(toPush));
  }
  virtual void handleObjEnd() {
    if(NULL==this->dest_.get()) {
      throw new invalid_input("Object value not set, yet we reached the end of object");
    }
    this->currKey_=boost::none;
    this->removeSelfIfTopTrace();
  }

  virtual void inferErrorLocation(std::string& resultHere) {
    if(this->currKey_) {
      resultHere.append("/").append(this->currKey_.get());
    }
  }
};


} // namespace sax
} // namespace jsonrsf

#endif /* __cplusplus */

#endif /* GENERIC_HANDLERS_HPP_ */

/*
 * JsonSaxAdaptor.hpp
 *
 *  Created on: 7 Sep 2016
 *      Author: acolomitchi
 */

#ifndef SAX_SAXADAPTOR_HPP_
#define SAX_SAXADAPTOR_HPP_


#include "generic_handlers.hpp"

#ifdef __cplusplus

namespace jsonrsf {
namespace sax {

template <typename T, bool asArray=false> class SaxAdaptor
{
public:
  using storage_type=typename deduce_storage_type<T, asArray, true>::type;

  SaxAdaptor() : data_(), trace_()
  {
    using first_handler_type=
      typename deduce_sax_handler<T>::type
    ;
    GenericSaxHandlerBase* first=
      new first_handler_type(this->data_, this->trace_)
    ;
    this->trace_.push(std::unique_ptr<GenericSaxHandlerBase>(first));
  }

  virtual ~SaxAdaptor() {
  }

  storage_type& getData() {
    return this->data_;
  }

  virtual void handleNull() {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `null` after completion");
    }
    this->trace_.top()->handleNull();
  }

  virtual void handleBool(bool value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `bool` after completion");
    }
    this->trace_.top()->handleBool(value);
  }
  virtual void handleByte(int8_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `byte` after completion");
    }
    this->trace_.top()->handleByte(value);
  }
  virtual void handleUByte(uint8_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `ubyte` after completion");
    }
    this->trace_.top()->handleUByte(value);
  }
  virtual void handleShort(int16_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `short` after completion");
    }
    this->trace_.top()->handleShort(value);
  }
  virtual void handleUShort(uint16_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `ushort` after completion");
    }
    this->trace_.top()->handleUShort(value);
  }
  virtual void handleInt(int32_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `int` after completion");
    }
    this->trace_.top()->handleInt(value);
  }
  virtual void handleUInt(uint32_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `uint` after completion");
    }
    this->trace_.top()->handleUInt(value);
  }
  virtual void handleLong(int64_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `long` after completion");
    }
    this->trace_.top()->handleLong(value);
  }
  virtual void handleULong(uint64_t value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `ulong` after completion");
    }
    this->trace_.top()->handleULong(value);
  }
  virtual void handleFloat(float value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `float` after completion");
    }
    this->trace_.top()->handleFloat(value);
  }
  virtual void handleDouble(double value) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `double` after completion");
    }
    this->trace_.top()->handleDouble(value);
  }
  virtual void handleString(const char* str, std::size_t length, bool copy) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `sring` after completion");
    }
    this->trace_.top()->handleString(str, length, copy);
  }
  virtual void handleObjStart() {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `obj-start` after completion");
    }
    this->trace_.top()->handleObjStart();
  }
  virtual void handleKey(const char* str, std::size_t length, bool copy) {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `key/prop` after completion");
    }
    this->trace_.top()->handleKey(str, length, copy);
  }

  virtual void handleObjEnd() {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `obj-end` after completion");
    }
    this->trace_.top()->handleObjEnd();
  }
  virtual void handleArrayStart() {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `array-start` after completion");
    }
    this->trace_.top()->handleArrayStart();
  }
  virtual void handleArrayEnd() {
    if(this->trace_.empty()) {
      throw new invalid_input("Got `array-end` after completion");
    }
    this->trace_.top()->handleArrayEnd();
  }

  virtual std::string inferErrorLocation() {
    std::string toRet;
    for(auto it=this->trace_.begin();it!=this->trace_.end(); it++) {
      (*it)->inferErrorLocation(toRet);
    }
    return toRet;
  }

protected:
  storage_type     data_;
  handler_stack    trace_;
};


} // namespace sax
} // namespace jsonrsf

#endif /* __cplusplus */


#endif /* SAX_SAXADAPTOR_HPP_ */

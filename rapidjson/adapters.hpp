/*
 * adapter.hpp
 *
 *  Created on: 8 Sep 2016
 *      Author: acolomitchi
 */

#ifndef RAPIDJSON_ADAPTERS_HPP_
#define RAPIDJSON_ADAPTERS_HPP_

#include <iostream>

#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include "../json_handlers/JsonAdapter.hpp"
#include "../json_handlers/emitter.hpp"

#ifdef __cplusplus

namespace jsonrsf {
namespace rapidjson {

namespace rj=::rapidjson;

template <typename T, bool asArray=false> class RapidJsonAdapter :
    public ::jsonrsf::JsonAdapter<T, asArray>,
    public rj::BaseReaderHandler<rj::UTF8<>, RapidJsonAdapter<T,asArray>>
{
protected:
  std::string lastErrorMsg_;

  void captureErrMsg(const std::runtime_error& err) {
    assert(this->lastErrorMsg_.empty());
    this->lastErrorMsg_="'";
    this->lastErrorMsg_
      .append(err.what())
      .append("'")
      .append(" on ")
      .append(this->inferErrorLocation())
    ;
  }
public:
  RapidJsonAdapter() :
    ::jsonrsf::JsonAdapter<T, asArray>(), lastErrorMsg_()
  {
  }
  ~RapidJsonAdapter() { }

  bool hasError() {
    return false==this->lastErrorMsg_.empty();
  }

  const std::string& errorMsg() const {
    return this->lastErrorMsg_;
  }

  bool Null() {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleNull();
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool Bool(bool b) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleBool(b);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool Int(int i) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleInt(i);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool Uint(unsigned u) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleUInt(u);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
  return toRet;
  }
  bool Int64(int64_t i) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleLong(i);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool Uint64(uint64_t u) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleULong(u);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool Double(double d) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleDouble(d);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool String(const char* str, ::rapidjson::SizeType length, bool copy) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleString(str, length, copy);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool StartObject() {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleObjStart();
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool Key(const char* str, ::rapidjson::SizeType length, bool copy) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleKey(str, length, copy);
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool EndObject(::rapidjson::SizeType memberCount) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleObjEnd();
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool StartArray() {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleArrayStart();
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }
  bool EndArray(::rapidjson::SizeType elementCount) {
    bool toRet=this->lastErrorMsg_.empty();
    if(toRet) {
      try {
        this->handleArrayEnd();
      }
      catch(std::runtime_error& err) {
        this->captureErrMsg(err);
        toRet=false;
      }
    }
    return toRet;
  }

};

template <bool indented, class OutStream, class DestEncoding> struct rjwriter_kind;

template <class OutStream, class DestEncoding>
struct rjwriter_kind<false, OutStream, DestEncoding>
{
  using type=rj::Writer<OutStream, rj::UTF8<>, DestEncoding>;
  ;
};
template <class OutStream, class DestEncoding>
struct rjwriter_kind<true, OutStream, DestEncoding>
{
  using type=rj::PrettyWriter<OutStream, rj::UTF8<>, DestEncoding>;
};

template <
  bool pretty_print=false,
  class DestEncoding=rj::UTF8<>
>
class RapidJsonEmitter :
  public Emitter
{
protected:
  rj::OStreamWrapper oStream_;
  typename rjwriter_kind<
    pretty_print,
    ::rapidjson::OStreamWrapper,
     DestEncoding
   >::type writer_;

public:
  RapidJsonEmitter(std::ostream& out) :
    oStream_(out), writer_(oStream_)
  {
  }

  ~RapidJsonEmitter() {
  }

  // emits a null
  virtual void emit(void) {
    this->writer_.Null();
  }

  virtual void emit(bool v) {
    this->writer_.Bool(v);
  }

  virtual void emit(int8_t v) {
    this->writer_.Int(v);
  }
  virtual void emit(uint8_t v) {
    this->writer_.Uint(v);
  }
  virtual void emit(int16_t v) {
    this->writer_.Int(v);
  }
  virtual void emit(uint16_t v) {
    this->writer_.Uint(v);
  }
  virtual void emit(int32_t v) {
    this->writer_.Int(v);
  }
  virtual void emit(uint32_t v) {
    this->writer_.Uint(v);
  }
  virtual void emit(int64_t v) {
    this->writer_.Int64(v);
  }
  virtual void emit(uint64_t v) {
    this->writer_.Uint64(v);
  }

  virtual void emit(float v) {
    this->writer_.Double(v);
  }
  virtual void emit(double v) {
    this->writer_.Double(v);
  }

  virtual void emit(const std::string& v) {
    this->writer_.String(v.c_str(), v.length(), true);
  }

  virtual void emitObjStart() {
    this->writer_.StartObject();
  }
  virtual void emitPropName(const std::string& v) {
    this->writer_.Key(v.c_str(), v.length(), true);
  }
  virtual void emitObjEnd() {
    this->writer_.EndObject();
  }
  virtual void emitArrayStart() {
    this->writer_.StartArray();
  }
  virtual void emitArrayEnd() {
    this->writer_.EndArray();
  }

};

} // rapidjson ns
} // jsonrsf ns

#endif /* __cplusplus */


#endif /* RAPIDJSON_ADAPTERS_HPP_ */

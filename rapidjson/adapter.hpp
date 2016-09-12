/*
 * adapter.hpp
 *
 *  Created on: 8 Sep 2016
 *      Author: acolomitchi
 */

#ifndef RAPIDJSON_ADAPTER_HPP_
#define RAPIDJSON_ADAPTER_HPP_

#include <rapidjson/rapidjson.h>

#include "../sax/SaxAdaptor.hpp"

#ifdef __cplusplus

namespace jsonrsf {
namespace rapidjson {

template <typename T, bool asArray=false> class RapidJsonSaxAdapter :
    public ::jsonrsf::SaxAdaptor<T, asArray>
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
  RapidJsonSaxAdapter() :
    ::jsonrsf::SaxAdaptor<T, asArray>(), lastErrorMsg_()
  {
  }
  ~RapidJsonSaxAdapter() { }

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

} // rapidjson ns
} // jsonrsf ns

#endif /* __cplusplus */


#endif /* RAPIDJSON_ADAPTER_HPP_ */

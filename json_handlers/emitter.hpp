/*
 * emiters.hpp
 *
 *  Created on: 12 Sep 2016
 *      Author: acolomitchi
 */

#ifndef JSON_HANDLERS_EMITTER_HPP_
#define JSON_HANDLERS_EMITTER_HPP_

#include <iostream>
#include <type_traits>

namespace jsonrsf {

class Emitter {
public:
  virtual ~Emitter() {}

  // emits a null
  virtual void emit(void)=0;

  virtual void emit(bool v)=0;

  virtual void emit(int8_t v)=0;
  virtual void emit(uint8_t v)=0;
  virtual void emit(int16_t v)=0;
  virtual void emit(uint16_t v)=0;
  virtual void emit(int32_t v)=0;
  virtual void emit(uint32_t v)=0;
  virtual void emit(int64_t v)=0;
  virtual void emit(uint64_t v)=0;

  virtual void emit(float v)=0;
  virtual void emit(double v)=0;

  virtual void emit(const std::string& v)=0;

  virtual void emitObjStart()=0;
  virtual void emitPropName(const std::string& val)=0;
  virtual void emitObjEnd()=0;
  virtual void emitArrayStart()=0;
  virtual void emitArrayEnd()=0;
};

} // namespace jsonrsf


#endif /* JSON_HANDLERS_EMITTER_HPP_ */

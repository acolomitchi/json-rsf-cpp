/*
 * exceptions.hpp
 *
 *  Created on: 25 Aug 2016
 *      Author: acolomitchi
 */

#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#ifdef __cplusplus
#include <stdexcept>
#include <string>

/**
 * When the state of a class is incompatible with the called operation
 * (e.g. protocol handling automata should use this when the sequence of operation
 *   violates the protocol. For instance, trying to Tx or Rx on a non-yet-opened
 *   connection, or received data coming out of order, etc)
 */
class state_error : public std::runtime_error {
public:
  explicit state_error (const std::string& what_arg) :
    runtime_error(what_arg)
  {
  }
  explicit state_error (const char* what_arg)  :
    runtime_error(what_arg)
  {
  }
  virtual ~state_error() { }
};

/*
 * The equivalent std::invalid_argument exception, but as a
 * std::run_time error (the std::invalid_argument is a logic_error,
 * presumably indicating a condition the programmers can detect
 * in the code rather than at runtime).
 */
class invalid_input : public std::runtime_error
{
public:
  explicit invalid_input (const std::string& what_arg) :
    runtime_error(what_arg)
  {
  }
  explicit invalid_input (const char* what_arg)  :
    runtime_error(what_arg)
  {
  }
  virtual ~invalid_input() { }

};

/**
 * Used for:
 *   1. not yet implemented (totally or for some particular use cases that
 *      should be supported); and/or
 *   2. method should not have been called at all (e.g. an overwrite of a virtual
 *      method to deny an operation otherwise available in other derived classes).
 * Note: it's <i>not</i> a runtime_error, execution paths leading to
 *   this could theoretically be predicted on the bases of the source code.
 */
class not_supported_error : public std::logic_error {
public:
  explicit not_supported_error (const std::string& what_arg) :
    logic_error(what_arg)
  {
  }
  explicit not_supported_error (const char* what_arg)  :
    logic_error(what_arg)
  {
  }
  virtual ~not_supported_error() { }
};

#endif /* __cplusplus */

#endif /* EXCEPTIONS_HPP_ */

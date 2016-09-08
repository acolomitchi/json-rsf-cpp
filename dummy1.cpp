/*
 * dummy1.cpp
 *
 *  Created on: 22 Aug 2016
 *      Author: acolomitchi
 */

#include "typeout.hpp"

#include <unordered_map>

#include "sax/SaxAdaptor.hpp"

#include "rapidjson/adapter.hpp"

namespace sx=jsonrsf::sax;

template <typename T> std::ostream& operator << (std::ostream& o, const std::vector<T>& val) {
  o << "[";
  bool firstOut=false;
  for(auto e : val) {
    if(firstOut) {
      o << ",";
    }
    else {
      firstOut=true;
    }
    o << e;
  }
  o << "]";
  return o;
};

struct a_struct {
  int alpha;
};

struct test_struct {
  char a;
  boost::optional<int> b;
  std::shared_ptr<test_struct> c;
  boost::optional<std::vector<std::shared_ptr<a_struct>>> d;
};



template <bool asRequired=true>
class AStructHandler :
    public sx::GenericStructSaxHandler<
      a_struct,
      typename sx::deduce_null_handler<std::shared_ptr<a_struct>, asRequired>::type
>{
public:
  using elem_type=std::shared_ptr<a_struct>;
  using null_handler=typename sx::deduce_null_handler<elem_type, asRequired>::type;
  using base_type=sx::GenericStructSaxHandler<a_struct, null_handler>;

  AStructHandler(elem_type& target, sx::handler_stack& trace) :
    base_type(target, trace)
  {
  }

  virtual ~AStructHandler() {

  }
protected:

  static const std::unordered_map<std::string, int> _prop_ix_dict;

  virtual sx::GenericSaxHandlerBase* createHandlerForCurrKey();
};

template <bool req> const std::unordered_map<std::string, int>
AStructHandler<req>::_prop_ix_dict={ {"alpha", 0} };

template <bool req> sx::GenericSaxHandlerBase* AStructHandler<req>::
createHandlerForCurrKey() {
  sx::GenericSaxHandlerBase* toRet=NULL;
  if(this->currKey_) {
    auto pos=AStructHandler::_prop_ix_dict.find(this->currKey_.get());
    if(pos!=AStructHandler::_prop_ix_dict.end()) {
      switch(pos->second) {
        case 0:
          toRet=new sx::deduce_sax_handler<decltype(a_struct::alpha)>::type(
            (*(this->dest_)).alpha, this->trace_
          );
          break;
        default:
          break;
      }
      if(NULL==toRet) {
        throw invalid_input("Unhandled property with name "+this->currKey_.get());
      }
    }
  }
  return toRet;
}

namespace jsonrsf { namespace sax {
  template <bool asRequired>
  struct deduce_sax_handler<a_struct, asRequired> {
    using type=AStructHandler<asRequired>;
  };
}}

template <bool asRequired=false>
class TestStructHandler :
    public sx::GenericStructSaxHandler<
      test_struct,
      typename sx::deduce_null_handler<std::shared_ptr<test_struct>, asRequired>::type
    >
{
  using null_handler=typename sx::deduce_null_handler<std::shared_ptr<test_struct>, asRequired>::type;
  using base_type=sx::GenericStructSaxHandler<test_struct, null_handler>;
public:
  TestStructHandler(std::shared_ptr<test_struct>& target, sx::handler_stack& trace) :
    base_type(target,  trace)
  {

  }

  virtual ~TestStructHandler() {

  }
protected:
  static const std::unordered_map<std::string, int> _prop_ix_dict;

  virtual sx::GenericSaxHandlerBase* createHandlerForCurrKey();
};

template <bool req> const std::unordered_map<std::string, int>
TestStructHandler<req>::_prop_ix_dict=
      {
        {"a", 0},
        {"b", 1},
        {"c", 2},
        {"d", 3}
      }
  ;

namespace jsonrsf { namespace sax {
template <bool asRequired>
  struct deduce_sax_handler<test_struct, asRequired> {
    using type=TestStructHandler<asRequired>;
  };
}}


template <bool req> inline sx::GenericSaxHandlerBase* TestStructHandler<req>::
createHandlerForCurrKey() {
  sx::GenericSaxHandlerBase* toRet=NULL;
  if(this->currKey_) {
    auto pos=TestStructHandler::_prop_ix_dict.find(this->currKey_.get());
    if(pos!=TestStructHandler::_prop_ix_dict.end()) {
      switch(pos->second) {
        case 0:
          toRet=new sx::deduce_sax_handler<decltype(test_struct::a)>::type(
            (*(this->dest_)).a, this->trace_
          );
          break;
        case 1:
          toRet=new sx::deduce_sax_handler<decltype(test_struct::b)>::type(
            (*(this->dest_)).b, this->trace_
          );
          break;
        case 2:
          toRet=new sx::deduce_sax_handler<decltype(test_struct::c), true>::type(
            (*(this->dest_)).c, this->trace_
          );
          break;
        case 3:
          toRet=new sx::deduce_sax_handler<decltype(test_struct::d)>::type(
            (*(this->dest_)).d, this->trace_
          );
          break;
        default:
          break;
      }
    }
    if(NULL==toRet) {
      throw invalid_input("Unhandled property with name "+this->currKey_.get());
    }
  }
  return toRet;
};


#include  <stdint.h>
int main() {

//  typedef decltype(std::declval<test_struct>().~test_struct()) tstdes;
//  outname<tstdes>("tsdes");

  ::jsonrsf::rapidjson::RapidJsonSaxAdapter<test_struct> rjsonHandler;

  sx::SaxAdaptor<test_struct> adapter;

  adapter.handleObjStart();
  adapter.handleKey("a", 1, true);
  adapter.handleInt(32);
  adapter.handleKey("b", 1, true);
  adapter.handleDouble(42.35);

  adapter.handleKey("d", 1, true);
  adapter.handleArrayStart();
  adapter.handleObjStart();
  adapter.handleKey("alpha",5, true);
  adapter.handleInt(42);
  adapter.handleObjEnd();
  adapter.handleObjStart();
  adapter.handleKey("alpha",5, true);
  adapter.handleInt(43);
  adapter.handleObjEnd();
  adapter.handleArrayEnd();

  adapter.handleObjEnd();

  auto parsed=adapter.getData();
  assert(parsed->a==' ');
  assert(parsed->b.get()==42);
  assert(parsed->d.get().size()==2);
  return 0;
}



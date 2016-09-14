/*
 * dummy1.cpp
 *
 *  Created on: 22 Aug 2016
 *      Author: acolomitchi
 */

#include <locale>

#include "typeout.hpp"

#include <unordered_map>

#include "json_handlers/emitter.hpp"
#include "json_handlers/JsonAdapter.hpp"
#include "rapidjson/adapters.hpp"

namespace sx=jsonrsf;

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
  jsonrsf::datetime_type beta;
  jsonrsf::daytime_type  gamma;
  jsonrsf::date_type delta;
};

struct test_struct {
  std::vector<int> a;
  std::string b;
  std::shared_ptr<test_struct> c;
  boost::optional<std::vector<std::shared_ptr<a_struct>>> d;
};



template <bool asRequired=true>
class AStructHandler :
    public sx::GenericStructJsonHandler<
      a_struct,
      typename sx::deduce_null_handler<std::shared_ptr<a_struct>, asRequired>::type
>{
public:
  using elem_type=std::shared_ptr<a_struct>;
  using null_handler=typename sx::deduce_null_handler<elem_type, asRequired>::type;
  using base_type=sx::GenericStructJsonHandler<a_struct, null_handler>;

  AStructHandler(elem_type& target, sx::handler_stack& trace) :
    base_type(target, trace)
  {
  }

  virtual ~AStructHandler() {

  }

  virtual void emit(sx::Emitter& dest, bool skipEmptyProps=true);
protected:

  static const std::unordered_map<std::string, int> _prop_ix_dict;

  virtual sx::GenericJsonHandlerBase* createHandlerForKey(const std::string& key);
};

template <bool req> const std::unordered_map<std::string, int>
AStructHandler<req>::_prop_ix_dict={
    {"alpha", 0},
    {"beta", 1},
    {"gamma", 2},
    {"delta", 3}
};

template <bool req> sx::GenericJsonHandlerBase* AStructHandler<req>::
createHandlerForKey(const std::string& key) {
  sx::GenericJsonHandlerBase* toRet=NULL;
  auto pos=AStructHandler::_prop_ix_dict.find(key);
  if(pos!=AStructHandler::_prop_ix_dict.end()) {
    switch(pos->second) {
    case 0:
      toRet=new sx::deduce_json_handler<decltype(a_struct::alpha)>::type(
        (*(this->dest_)).alpha, this->trace_
      );
      break;
    case 1:
      toRet=new sx::deduce_json_handler<decltype(a_struct::beta)>::type(
        (*(this->dest_)).beta, this->trace_
      );
      break;
    case 2:
      toRet=new sx::deduce_json_handler<decltype(a_struct::gamma)>::type(
        (*(this->dest_)).gamma, this->trace_
      );
      break;
    case 3:
      toRet=new sx::deduce_json_handler<decltype(a_struct::delta)>::type(
        (*(this->dest_)).delta, this->trace_
      );
      break;
    default:
      break;
    }
  }
  if(NULL==toRet) {
    throw invalid_input("Unhandled property with name "+key);
  }
  return toRet;
}

template <bool req> void AStructHandler<req>::
emit(sx::Emitter& target, bool flag) {
  if(NULL!=this->dest_) {
    target.emitObjStart();
    target.emitPropName("alpha");
    sx::deduce_json_handler<decltype(a_struct::alpha)>::type(
      (*(this->dest_)).alpha, this->trace_
    ).emit(target, flag);
    target.emitPropName("beta");
    sx::deduce_json_handler<decltype(a_struct::beta)>::type(
      (*(this->dest_)).beta, this->trace_
    ).emit(target, flag);
    target.emitPropName("gamma");
    sx::deduce_json_handler<decltype(a_struct::gamma)>::type(
      (*(this->dest_)).gamma, this->trace_
    ).emit(target, flag);
    target.emitPropName("delta");
    sx::deduce_json_handler<decltype(a_struct::delta)>::type(
      (*(this->dest_)).delta, this->trace_
    ).emit(target, flag);
    target.emitObjEnd();
  }
  else {
    target.emit(); // null
  }
}

namespace jsonrsf {
  template <bool asRequired>
  struct deduce_json_handler<a_struct, asRequired> {
    using type=AStructHandler<asRequired>;
  };
}

template <bool asRequired=false>
class TestStructHandler :
    public sx::GenericStructJsonHandler<
      test_struct,
      typename sx::deduce_null_handler<std::shared_ptr<test_struct>, asRequired>::type
    >
{
  using null_handler=typename sx::deduce_null_handler<std::shared_ptr<test_struct>, asRequired>::type;
  using base_type=sx::GenericStructJsonHandler<test_struct, null_handler>;
public:
  TestStructHandler(std::shared_ptr<test_struct>& target, sx::handler_stack& trace) :
    base_type(target,  trace)
  {

  }

  virtual ~TestStructHandler() {

  }

  void emit(sx::Emitter& target, bool skipNullProps=true);

protected:
  static const std::unordered_map<std::string, int> _prop_ix_dict;

  virtual sx::GenericJsonHandlerBase* createHandlerForKey(const std::string& key);
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

namespace jsonrsf {
template <bool asRequired>
  struct deduce_json_handler<test_struct, asRequired> {
    using type=TestStructHandler<asRequired>;
  };
}


template <bool req> inline sx::GenericJsonHandlerBase* TestStructHandler<req>::
createHandlerForKey(const std::string& key) {
  sx::GenericJsonHandlerBase* toRet=NULL;
  auto pos=TestStructHandler::_prop_ix_dict.find(key);
  if(pos!=TestStructHandler::_prop_ix_dict.end()) {
    switch(pos->second) {
      case 0:
        toRet=new sx::deduce_json_handler<decltype(test_struct::a)>::type(
          (*(this->dest_)).a, this->trace_
        );
        break;
      case 1:
        toRet=new sx::deduce_json_handler<decltype(test_struct::b)>::type(
          (*(this->dest_)).b, this->trace_
        );
        break;
      case 2:
        toRet=new sx::deduce_json_handler<decltype(test_struct::c), true>::type(
          (*(this->dest_)).c, this->trace_
        );
        break;
      case 3:
        toRet=new sx::deduce_json_handler<decltype(test_struct::d)>::type(
          (*(this->dest_)).d, this->trace_
        );
        break;
      default:
        break;
    }
  }
  if(NULL==toRet) {
    throw invalid_input("Unhandled property with name "+key);
  }
  return toRet;
};

template <bool req> void TestStructHandler<req>::
emit(sx::Emitter& target, bool flag) {
//  int a;
//  std::string b;
//  std::shared_ptr<test_struct> c;
//  boost::optional<std::vector<std::shared_ptr<a_struct>>> d;
  if(NULL!=this->dest_) {
    target.emitObjStart();
    target.emitPropName("a");
    sx::deduce_json_handler<decltype(test_struct::a)>::type(
      (*(this->dest_)).a, this->trace_
    ).emit(target, flag);
    target.emitPropName("b");
    sx::deduce_json_handler<decltype(test_struct::b)>::type(
      (*(this->dest_)).b, this->trace_
    ).emit(target, flag);
    if(NULL!=(*(this->dest_)).c || flag) {
      target.emitPropName("c");
      sx::deduce_json_handler<decltype(test_struct::c)>::type(
        (*(this->dest_)).c, this->trace_
      ).emit(target, flag);
    }
    if(NULL!=(*(this->dest_)).d || flag) {
      target.emitPropName("d");
      sx::deduce_json_handler<decltype(test_struct::d)>::type(
        (*(this->dest_)).d, this->trace_
      ).emit(target, flag);
    }
    target.emitObjEnd();
  }
  else {
    target.emit(); // null
  }
}

#include  <stdint.h>
#include <algorithm>

#include <rapidjson/reader.h>
#include "rapidjson/adapters.hpp"

template <typename T> struct dummy_base {
  using type=T;
};

template <typename T> struct dummy : public dummy_base<T> {

};


int main() {

  namespace jr=jsonrsf::rapidjson;
  namespace r=rapidjson;

  using adapter_t=jr::RapidJsonAdapter<test_struct>;
  adapter_t adaptor;

  r::Reader reader;

  std::string toParse="{\"a\" : [3.14, 42], \"b\":\"\\u00b0.m\\u00b0<\"}";
  r::StringStream in(toParse.c_str());

  auto res=reader.Parse(in, adaptor);
  if(res.IsError()) {
    std::cout << adaptor.errorMsg() << res.Code()
              << " at " << res.Offset() << " in ctx:"
              << adaptor.inferErrorLocation()
              << std::endl;
  }

  std::cout << adaptor.getData()->a << " - >" << adaptor.getData()->b << std::endl;

  std::ostringstream out;
  jr::RapidJsonEmitter<true> writer(out);

  adaptor.emitData(writer, false);
  std::cout << "Emitted: " << std::endl << out.str() << std::endl;

  return 0;
}




#if 0
//int main() {
//
////  typedef decltype(std::declval<test_struct>().~test_struct()) tstdes;
////  outname<tstdes>("tsdes");
//
//


//  namespace d=date;
//
//  std::string toParse="T08:45:05Z+1000";
//  std::string fmt="T%TZ%z";
//
//  std::string abbrev;
//  std::chrono::minutes offset;


  //  auto recovered=
//    std::chrono::time_point_cast<
//      std::chrono::system_clock,
//      std::chrono::milliseconds
//    >
//    (std::chrono::milliseconds(secs*1000+millies)
//  );



//  std::istringstream input;
//  d::local_time<std::chrono::milliseconds> parsedDate;
//  input.clear();
//  input.str(toParse);
//  d::parse(input, fmt.c_str(), parsedDate, offset);
//  if(input.fail()) {
//    std::cout <<"Failed to parse input date:`"<<toParse<<"`"<<std::endl;
//  }
//  else {
//    std::cout <<d::format("T%T", parsedDate) << " with offset "<< offset.count() << std::endl;
//  }
//
//  ::jsonrsf::rapidjson::RapidJsonSaxAdapter<test_struct> rjsonHandler;

//  sx::SaxAdaptor<test_struct> adapter;
//
//  adapter.handleObjStart();
//  adapter.handleKey("a", 1, true);
//  adapter.handleInt(32);
//  adapter.handleKey("b", 1, true);
//  adapter.handleDouble(42.35);
//
//  adapter.handleKey("d", 1, true);
//  adapter.handleArrayStart();
//  adapter.handleObjStart();
//  adapter.handleKey("alpha",5, true);
//  adapter.handleInt(42);
//  adapter.handleObjEnd();
//  adapter.handleObjStart();
//  adapter.handleKey("alpha",5, true);
//  adapter.handleInt(43);
//  adapter.handleKey("beta",4, true);
//  adapter.handleString(std::string("2016-09-12T12:52:45Z+1000"));
//  adapter.handleKey("gamma",5, true);
//  adapter.handleString(std::string("T12:52:45Z+1000"));
//  adapter.handleKey("delta",5, true);
//  adapter.handleString(std::string("2016-09-12"));
//  adapter.handleObjEnd();
//  adapter.handleArrayEnd();
//
//  adapter.handleObjEnd();
//
//  auto parsed=adapter.getData();
//  assert(parsed->a==' ');
//  assert(parsed->b.get()==42);
//  assert(parsed->d.get().size()==2);
  return 0;
}

#endif

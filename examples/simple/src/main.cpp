#include <oneList.h>
#include <hapi.h>
using namespace hapi;

#include <iostream>
using namespace std;

template<typename O, typename... OO>
struct StaticBody:List<O,OO...> {

  using Base=List<O,OO...>;
  using Base::Base;

  using Head=typename Base::Head;
  using Tail=::StaticBody<OO...>;
  using Base::head;
  using Base::tail;

  template<template<typename...> class T> using Build=T<O,OO...>;

  template<Sz i> constexpr const std::enable_if_t<Head::template hasId<i>(),Head> withId() const {return head;}
  template<Sz i> constexpr std::enable_if_t<Head::template hasId<i>(),Head> withId() {return head;}
  template<Sz i> constexpr const auto withId() const {return ((Tail&)tail).template withId<i>();}
  
};

template<typename O>
struct StaticBody<O>:List<O> {

  using Base=List<O>;
  using Head=typename Base::Head;
  using Tail=typename Base::Tail;
  using Base::head;
  // using Base::tail;

  template<Sz i>static constexpr bool hasId() {return Head::template hasId<i>()||Tail::template hasId<i>();}

  template<Sz i> constexpr const std::enable_if_t<Head::template hasId<i>(),Head> withId() const {return head;}
  template<Sz i> constexpr std::enable_if_t<Head::template hasId<i>(),Head> withId() {return head;}
  
};

template<typename... OO>
constexpr auto staticBody(OO&&... oo) {
  return StaticBody<std::decay_t<OO>...>{std::forward<OO>(oo)...};
}

template<typename Cfg=Nil>
struct ItemAPI:Cfg {
  //make all this item types id-able
  static constexpr bool hasId(Sz sid) {return false;}
};

template<typename... OO>
struct ItemDef:APIOf<ItemAPI<>,OO...>{
  using Base=APIOf<ItemAPI<>,OO...>;
  using Item=ItemDef<OO...>;
};

template<Sz i>
struct Id {
  template<typename O>
  struct Part:O {
    using Base=O;
    static constexpr Sz id{i};
    template<Sz sid>
    static constexpr bool hasId() {return sid==id;}
  };
};

template<auto f>//avr wont support auto here, will have split into <typename F,F&f>
struct Action {
  template<typename O>
  struct Part:O {
    //avr wil require -> trail return type
    template<typename... OO> static constexpr auto act(const OO&... oo) {return f(oo...);}
  };
};

enum id {op1,op2,op3};

void a3() {cout<<"option #3 activated, this is a side effect."<<endl;}

constexpr auto body{staticBody(ItemDef<Id<id::op1>>{},ItemDef<Id<id::op2>>{},ItemDef<Id<id::op3>,Action<a3>>{})};

int main() {
  cout<<foldl(list(1,2,3),[](auto a,auto b){return a+b;},10)<<endl;
  body.template withId<id::op3>().act();
  return 0;
}
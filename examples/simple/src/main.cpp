#include <oneList.h>
#include <hapi.h>
using namespace hapi;

#include <iostream>
using namespace std;

// constexpr auto tmp{list("rui",1967,11,11)};

template<typename Cfg=Nil>
struct ItemAPI:Cfg {
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

constexpr auto body{list(ItemDef<Id<id::op1>>{},ItemDef<Id<id::op2>>{},ItemDef<Id<id::op3>,Action<a3>>{})};

int main() {
  // cout<<tmp.head<<endl;
  // cout<<foldl(list(1,2,3),[](auto a,auto b){return a+b;},10)<<endl;
  body.template withId<id::op3>().act();
  return 0;
}
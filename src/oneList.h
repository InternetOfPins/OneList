#pragma once

/**
 * @file oneList.h
 * @brief Compact heterogeneous runtime list + compile-time TypeList mirror
 * 
 * Designed for AM5 short menus, RPC, and generic zero-overhead use.
 * 
 * @author Rui Azevedo (neu-rah)
 * @contributor Grok (xAI) - design, refinement & API
 * @date 2026-05-10
 * @version 0.3
*/

#include <hapi.h>
using namespace hapi;

#ifdef __AVR__
  #include <avr_std.h>
  using Sz=int;
#else
  #include <type_traits>
  #include <utility>
  using Sz=std::size_t;
#endif


// =============================================
// OneList - Compact heterogeneous runtime list
// + compile-time TypeList mirror
// =============================================

template<typename... Ts>
struct TypeList {
  static constexpr Sz size = sizeof...(Ts);

  template<typename T>
  static constexpr bool Has = (std::is_same_v<T, Ts> || ...);

  //prefix/suffix or insert/append, Grok?
  template<typename T>
  using Suffix = TypeList<Ts..., T>;

  template<typename T>
  using Prefix = TypeList<T, Ts...>;
};

// ====================== List Core ======================

template<typename O, typename... OO>
struct List {
  using Head = O;
  using Tail = List<OO...>;
  using Types = TypeList<O, OO...>;

  Head head;
  Tail tail;

  template<typename... TT>
  constexpr List(Head h, TT... tt) noexcept 
    : head(std::move(h)), tail(std::move(tt)...) {}

  template<typename F,typename Acc>
  constexpr auto foldl(const F& f, Acc acc) const {
    auto next = f(acc, head);
    return tail.foldl(f, std::move(next));
  }
 
  // template<Sz i> static constexpr bool hasId() 
  //   {return Head::template hasId<i>()||Tail::template hasId<i>();}

  // //const--  
  // template<Sz i> constexpr const auto withId() const 
  //   {return tail.template withId<i>();}
 
  // template<Sz i> constexpr const Head withId() const {return head;}
  
  //--
  // template<Sz i> constexpr auto withId() 
  //   {return tail.template withId<i>();}
  // template<Sz i> constexpr Head withId() {return head;}
};

template<typename O>
struct List<O> {
  using Head = O;
  using Tail=void;
  using Types = TypeList<O>;

  Head head;

  constexpr List(Head h) noexcept : head(std::move(h)) {}

  template<typename F,typename Acc>
  constexpr auto foldl(const F& f, Acc acc) const
    {return std::move(f(acc, head));}

  // template<Sz i>static constexpr bool hasId() {return Head::template hasId<i>();}
  // template<Sz i> constexpr Head withId() {return head;}
  // template<Sz i> constexpr const Head withId() const {return head;}
};

// ====================== Factories ======================
template<typename... OO>
constexpr List<std::decay_t<OO>...> list(OO&&... oo) {return List<std::decay_t<OO>...>{std::forward<OO>(oo)...};}

// ====================== Helpers ======================
template<typename L, typename F,typename O>
auto foldl(const L& l,const F& f,O o) {return l.foldl(f,std::move(o));}

template<typename L, typename F>
constexpr void forEach(const L& l, F&& f, int i = 0) {
  f(l.head, i);
  if constexpr (!std::is_same_v<typename L::Tail, void>)
    forEach(l.tail, std::forward<F>(f), i + 1);
}

template<typename Cfg=Nil>
struct ListAPI:Cfg {
  template<typename O> static constexpr bool use() {return true;}
  template<typename F>
  static constexpr auto map(const F&,O&& o) {return o;}//defaults to identity
};

template<typename... OO>
struct ListDef:APIOf<ListAPI<>,OO...>{
  using Base=APIOf<ListAPI<>,OO...>;
  using Base::use;
  using Base::tail;
  // auto run() {
  //   if(use()) return map(f);
  //   return 
  // }
};


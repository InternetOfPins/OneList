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

  template<template<typename...> class T> using Build=T<Ts...>;

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
};

template<typename O>
struct List<O> {
  using Head = O;
  using Tail=void;
  using Types = TypeList<O>;

  Head head;

  constexpr List(Head h) noexcept : head(std::move(h)) {}

};

// ====================== Factories ======================
template<typename... OO>
constexpr auto list(OO&&... oo) {
  return List<std::decay_t<OO>...>{std::forward<OO>(oo)...};
}

// ====================== Helpers ======================
template<typename L, typename F, typename Acc>
constexpr auto foldl(const L& l, F&& f, Acc acc) {
    auto next = f(acc, l.head);
    if constexpr (std::is_same_v<typename L::Tail, void>)
        return next;
    else
        return foldl(l.tail, std::forward<F>(f), std::move(next));
}

template<typename L, typename F>
constexpr void forEach(const L& l, F&& f, int i = 0) {
  f(l.head, i);
  if constexpr (!std::is_same_v<typename L::Tail, void>)
    forEach(l.tail, std::forward<F>(f), i + 1);
}

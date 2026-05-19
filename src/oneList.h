/**
 * @file oneList.h
 * @author Rui Azevedo (neu-rah)
 * @brief Minimal heterogeneous runtime list + TypeList
 * @date 2026-05-19
 * @contributor Grok (xAI) - architecture, cleanup & modern C++ patterns
 */

#pragma once

#include <type_traits>
#include <utility>

namespace hapi::list {

  // ====================== TypeList ======================
  template<typename... Ts>
  struct TypeList {
    static constexpr size_t size = sizeof...(Ts);

    template<typename T>
    static constexpr bool has = (std::is_same_v<std::decay_t<T>, Ts> || ...);
  };

  // ====================== Runtime List ======================
  template<typename O, typename... OO>
  struct List {
    using Head = O;
    using Tail = List<OO...>;
    using Types = TypeList<O, OO...>;

    Head head;
    Tail tail;

    // constexpr List(Head h, Tail t) noexcept 
    //   : head(std::move(h)), tail(std::move(t)) {}

    constexpr List(O o,OO... oo) noexcept :
      head{std::move(o)},
      tail{std::move(oo)...}
      {}

    template<typename Id>
    constexpr const auto& withId() const {
      if constexpr (Head::template has<Id>) return head;
      else return tail.template withId<Id>();
    }
  };

  template<typename O>
  struct List<O> {
    using Head = O;
    using Tail = void;
    using Types = TypeList<O>;

    Head head;

    constexpr List(Head h) noexcept : head(std::move(h)) {}

    template<typename Id>
    constexpr const auto& withId() const {
      static_assert(Types::template has<Id>, "Id not found in List");
      return head;
    }
  };

  // ====================== Factory ======================
  template<typename... OO>
  constexpr auto list(OO&&... oo) {
    return List<std::decay_t<OO>...>{std::forward<OO>(oo)...};
  }

  // ====================== Helpers ======================
  template<typename L, typename F>
  constexpr void forEach(const L& l, F&& f, int i = 0) {
    f(l.head, i);
    if constexpr (!std::is_same_v<typename L::Tail, void>)
      forEach(l.tail, std::forward<F>(f), i + 1);
  }

} // namespace hapi::list
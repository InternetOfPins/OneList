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

#include <type_traits>
#include <utility>

// =============================================
// OneList - Compact heterogeneous runtime list
// + compile-time TypeList mirror
// =============================================

template<typename... Ts>
struct TypeList {
    static constexpr size_t size = sizeof...(Ts);

    template<typename T>
    static constexpr bool Has = (std::is_same_v<T, Ts> || ...);

    template<typename T>
    using Append = TypeList<Ts..., T>;

    template<typename T>
    using Prepend = TypeList<T, Ts...>;
};

// Item with Id (core for AM5 RPC / menu navigation)
template<typename IdT, typename ValueT>
struct Item {
    IdT id;
    ValueT value;

    constexpr Item(IdT i, ValueT v) noexcept 
        : id(std::move(i)), value(std::move(v)) {}
};

// ====================== List Core ======================
template<typename O, typename... OO>
struct List {
    using Head = O;
    using Tail = List<OO...>;
    using Types = TypeList<O, OO...>;

    Head head;
    Tail tail;

    constexpr List(Head h, Tail t) noexcept 
        : head(std::move(h)), tail(std::move(t)) {}

    template<typename SearchId>
    constexpr const auto* withId(SearchId sid) const noexcept {
        if constexpr (std::is_same_v<std::decay_t<Head>, Item<SearchId, typename Head::ValueT>>) {
            if (head.id == sid) return &head.value;
        }
        return tail.withId(sid);
    }
};

template<typename O>
struct List<O> {
    using Head = O;
    using Tail = void;
    using Types = TypeList<O>;

    Head head;

    constexpr List(Head h) noexcept : head(std::move(h)) {}

    template<typename SearchId>
    constexpr const auto* withId(SearchId) const noexcept { return nullptr; }
};

// ====================== Factories ======================
template<typename... OO>
constexpr auto list(OO&&... oo) {
    return List<std::decay_t<OO>...>{std::forward<OO>(oo)...};
}

template<typename... OO>
constexpr auto staticBody(OO&&... oo) {
    return list(std::forward<OO>(oo)...);
}

template<typename IdT, typename ValueT>
constexpr Item<IdT, ValueT> item(IdT id, ValueT value) {
    return {std::move(id), std::move(value)};
}

// ====================== Helpers ======================
template<typename L, typename F>
constexpr void forEach(const L& l, F&& f, int i = 0) {
    f(l.head, i);
    if constexpr (!std::is_same_v<typename L::Tail, void>)
        forEach(l.tail, std::forward<F>(f), i + 1);
}

template<typename L, typename F, typename Acc>
constexpr auto foldl(const L& l, F&& f, Acc acc) {
    auto next = f(acc, l.head);
    if constexpr (std::is_same_v<typename L::Tail, void>)
        return next;
    else
        return foldl(l.tail, std::forward<F>(f), std::move(next));
}


# OneList

Compact **heterogeneous runtime list** with compile-time type mirror for tiny embedded systems.

Designed for short menus (AM5), RPC/command systems, configuration, and generic zero-overhead programming in C++.

---

## Philosophy

- **Tiny memory & stack footprint** — ideal for AVR, ARM Cortex-M, ESP8266, etc.
- **Strong compile-time introspection** via `::Types`
- **Simple & military-sharp API**
- **Menu-first** but general purpose

---

## Features

- Runtime heterogeneous list (`List<...>`)
- Compile-time `TypeList` mirror for rules, validation and HAPI integration
- ~~Built-in `Item<Id, Value>` for RPC / menu actions~~ moving to external composition.
- `Buid<...>` any given composable type.
- `withId()` fast lookup (compile-time friendly)
- `forEach`, `foldl`
- Full `constexpr` support where possible
- No dynamic allocation

---

## Basic Usage

```cpp
#include "oneList.h"

constexpr auto menu = staticBody(
    item("led",    []{ digitalWrite(13, !digitalRead(13)); }),
    item("status", "System OK"),
    item("reset",  []{ reboot(); }),
    item("temp",   23.5f)
);

// Runtime lookup by Id (great for RPC / buttons)
constexpr auto* action = menu.withId("led");
if (action) (*action)();

// Type-level introspection
using TL = decltype(menu)::Types;
static_assert(TL::Has<Item<const char*, float>>);
static_assert(TL::size == 4);
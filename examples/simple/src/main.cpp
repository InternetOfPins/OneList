#include <oneList.h>

constexpr bool digitalRead(int) {return false;}
constexpr void digitalWrite(int,bool) {}
constexpr void reboot() {}

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
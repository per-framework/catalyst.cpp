#pragma once

#include "catalyst_v0/merge_map.hpp"
#include "catalyst_v0/once.hpp"

template <class Function, class Observable>
auto map(Function &&function, Observable &&observable) {
  return merge_map(
      [function = std::forward<Function>(function)](auto &&x) {
        return once(function(std::forward<decltype(x)>(x)));
      },
      std::forward<Observable>(observable));
}

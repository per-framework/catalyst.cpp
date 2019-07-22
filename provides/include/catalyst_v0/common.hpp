#pragma once

#include <type_traits>
#include <utility>

//

namespace std {
template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
}

template <class Forwardable>
using copy_t =
    std::conditional_t<std::is_function_v<std::remove_cvref_t<Forwardable>>,
                       std::remove_cvref_t<Forwardable> *,
                       std::remove_cvref_t<Forwardable>>;

//

template <class Observable, class Observer>
using subscription_t = typename Observable::template subscription_t<Observer>;

template <class Observable> using value_t = typename Observable::value_t;

//

template <class Observable, class Observer>
subscription_t<copy_t<Observable>, copy_t<Observer>>
subscription(Observable &&observable, Observer &&observer) {
  return copy_t<Observable>::subscription(std::forward<Observable>(observable),
                                          std::forward<Observer>(observer));
}

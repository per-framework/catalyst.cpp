#pragma once

#include "catalyst_v0/common.hpp"

template <class Value, class Observer> struct from_array_subscription_t {
  Value *m_values;
  size_t m_n;
  Observer m_observer;

  void start() {
    m_observer.on_start(this);
    for (size_t i = 0, n = m_n; i < n; ++i)
      m_observer.on_value(m_values[i]);
    m_observer.on_end(this);
  }
};

template <class Value> struct from_array_t {
  using value_t = Value;

  template <class Observer>
  using subscription_t = from_array_subscription_t<Value, Observer>;

  static constexpr bool is_synchronous = true;

  Value *m_values;
  size_t m_n;

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return {self.m_values, self.m_n, std::forward<Observer>(observer)};
  }
};

template <class Value, size_t N>
from_array_t<Value> from_array(Value (&values)[N]) {
  return {values, N};
}

template <class Value> from_array_t<Value> from_array(Value *values, size_t n) {
  return {values, n};
}

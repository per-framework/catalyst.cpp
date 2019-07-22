#pragma once

#include "catalyst_v0/common.hpp"

template <class Value, class Observer> struct once_subscription_t {
  Value m_value;
  Observer m_observer;

  void start() {
    m_observer.on_start(this);
    m_observer.on_value(std::move(m_value));
    m_observer.on_end(this);
  }
};

template <class Value> struct once_t {
  using value_t = Value;

  template <class Observer>
  using subscription_t = once_subscription_t<Value, Observer>;

  static constexpr bool is_synchronous = true;

  Value m_value;

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return {std::forward<Self>(self).m_value, std::forward<Observer>(observer)};
  }
};

template <class Value> once_t<copy_t<Value>> once(Value &&value) {
  return {std::forward<Value>(value)};
}

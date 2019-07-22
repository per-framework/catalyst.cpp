#pragma once

#include "catalyst_v0/merge_map.hpp"
#include "catalyst_v0/once.hpp"

#include <optional>

template <class Value, class Observer> struct optional_subscription_t {
  std::optional<Value> m_value;
  Observer m_observer;

  void start() {
    m_observer.on_start(this);
    if (m_value.has_value())
      m_observer.on_value(std::move(m_value).value());
    m_observer.on_end(this);
  }
};

template <class Value> struct optional_t {
  using value_t = Value;

  template <class Observer>
  using subscription_t = optional_subscription_t<Value, Observer>;

  static constexpr bool is_synchronous = true;

  std::optional<Value> m_value;

  optional_t() {}

  template <class Input>
  optional_t(Input &&value) : m_value(std::forward<Input>(value)) {}

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return {std::forward<Self>(self).m_value, std::forward<Observer>(observer)};
  }
};

template <class Value> optional_t<copy_t<Value>> some(Value &&value) {
  return {std::forward<Value>(value)};
}

template <class Value> optional_t<copy_t<Value>> none() {
  return optional_t<copy_t<Value>>();
}

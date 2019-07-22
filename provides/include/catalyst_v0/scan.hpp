#pragma once

#include "catalyst_v0/common.hpp"

template <class Function, class State, class Observer> struct scan_observer_t {
  Function m_function;
  State m_state;
  Observer m_observer;

  void on_start(void *) {
    m_observer.on_start();
    m_observer.on_value(m_state);
  }

  template <class Value> void on_value(Value &&value) {
    m_state = m_function(std::move(m_state), std::forward<Value>(value));
    m_observer.on_value(m_state);
  }

  void on_end(void *subscription) { m_observer.on_end(subscription); }
};

template <class Function, class State, class Observable> struct scan_t {
  using value_t = std::invoke_result_t<Function, State, value_t<Observable>>;

  template <class Observer>
  using subscription_t =
      subscription_t<Observable, scan_observer_t<Function, State, Observer>>;

  static constexpr bool is_synchronous = Observable::is_synchronous;

  Function m_function;
  State m_initial;
  Observable m_observable;

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return Observable::subscription(std::forward<Self>(self).m_observable,
                                    scan_observer_t<Function, State, Observer>{
                                        std::forward<Self>(self).m_function,
                                        std::forward<Self>(self).m_initial,
                                        std::forward<Observer>(observer)});
  }
};

template <class Function, class State, class Observable>
scan_t<copy_t<Function>, copy_t<State>, copy_t<Observable>>
scan(Function &&function, State &&state, Observable &&observable) {
  return {std::forward<Function>(function),
          std::forward<State>(state),
          std::forward<Observable>(observable)};
}

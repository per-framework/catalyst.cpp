#pragma once

#include "catalyst_v0/common.hpp"

#include <algorithm>
#include <vector>

template <class Outer> struct merge_map_inner_observer_t {
  Outer *m_outer;

  void on_start(void *) {}

  template <class Value> void on_value(Value &&value) {
    m_outer->m_observer.on_value(std::forward<Value>(value));
  }

  void on_end(void *subscription) {
    Outer &outer = *m_outer;

    std::swap(
        *static_cast<typename Outer::inner_subscription_t *>(subscription),
        outer.m_subscriptions.back());
    outer.m_subscriptions.pop_back();

    outer.maybe_ended();
  }
};

template <class Function, class Input, class Observer>
struct merge_map_asynchronous_outer_observer_t {
  using inner_observable_t = std::invoke_result_t<Function, Input>;
  using inner_subscription_t = subscription_t<
      inner_observable_t,
      merge_map_inner_observer_t<merge_map_asynchronous_outer_observer_t>>;

  Function m_function;
  Observer m_observer;
  std::vector<inner_subscription_t> m_subscriptions;
  void *m_outer_subscription;

  void maybe_ended() {
    if (m_outer_subscription && m_subscriptions.empty())
      m_observer.on_end(m_outer_subscription);
  }

  void on_start(void *subscription) { m_observer.on_start(subscription); }

  template <class Value> void on_value(Value &&value) {
    m_subscriptions.push_back(subscription(
        m_function(std::forward<Value>(value)),
        merge_map_inner_observer_t<merge_map_asynchronous_outer_observer_t>{
            this}));
    m_subscriptions.back().start();
  }

  void on_end(void *subscription) {
    m_outer_subscription = subscription;
    maybe_ended();
  }
};

template <class Function, class Input, class Observer>
struct merge_map_synchronous_outer_observer_t {
  using inner_observable_t = std::invoke_result_t<Function, Input>;

  Function m_function;
  Observer m_observer;

  void on_start(void *subscription) { m_observer.on_start(subscription); }

  template <class Value> void on_value(Value &&value) {
    subscription(m_function(std::forward<Value>(value)), m_observer).start();
  }

  void on_end(void *subscription) { m_observer.on_end(subscription); }
};

template <class Function, class Observable> struct merge_map_t {
  using inner_observable_t =
      std::invoke_result_t<Function, value_t<Observable>>;

  using value_t = value_t<inner_observable_t>;

  static constexpr bool is_synchronous =
      Observable::is_synchronous && inner_observable_t::is_synchronous;

  template <class Observer>
  using outer_observer_t = std::conditional_t<
      is_synchronous,
      merge_map_synchronous_outer_observer_t<Function, value_t, Observer>,
      merge_map_asynchronous_outer_observer_t<Function, value_t, Observer>>;

  template <class Observer>
  using subscription_t = subscription_t<Observable, outer_observer_t<Observer>>;

  Function m_function;
  Observable m_observable;

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return Observable::subscription(
        std::forward<Self>(self).m_observable,
        outer_observer_t<Observer>{std::forward<Self>(self).m_function,
                                   std::forward<Observer>(observer)});
  }
};

template <class Function, class Observable>
merge_map_t<copy_t<Function>, copy_t<Observable>>
merge_map(Function &&function, Observable &&observable) {
  return {std::forward<Function>(function),
          std::forward<Observable>(observable)};
}

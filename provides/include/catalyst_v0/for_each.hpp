#pragma once

#include "catalyst_v0/common.hpp"

template <class Action> struct for_each_common_observer_t {
  Action m_action;

  void on_start(void *) {}

  template <class Value> void on_value(Value &&value) {
    m_action(std::forward<Value>(value));
  }
};

template <class Action, class Outer>
struct for_each_asynchronous_observer_t : for_each_common_observer_t<Action> {
  void on_end(void *subscription) {
    delete static_cast<typename Outer::subscription_t *>(subscription);
  }
};

template <class Action>
struct for_each_synchronous_observer_t : for_each_common_observer_t<Action> {
  void on_end(void *) {}
};

template <class Action, class Observable>
void for_each(Action &&action, Observable &&observable) {
  struct cycle_t {
    using observer_t = std::conditional_t<
        copy_t<Observable>::is_synchronous,
        for_each_synchronous_observer_t<copy_t<Action>>,
        for_each_asynchronous_observer_t<copy_t<Action>, cycle_t>>;
    using subscription_t = subscription_t<copy_t<Observable>, observer_t>;
  };

  typename cycle_t::subscription_t s = subscription(
      std::forward<Observable>(observable),
      typename cycle_t::observer_t{{std::forward<Action>(action)}});

  if (copy_t<Observable>::is_synchronous)
    s.start();
  else
    (new typename cycle_t::subscription_t(std::move(s)))->start();
}

template <class Observable> void for_each(Observable &&observable) {
  for_each([](auto &&) {}, std::forward<Observable>(observable));
}

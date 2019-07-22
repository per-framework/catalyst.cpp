#pragma once

#include "catalyst_v0/merge_map.hpp"
#include "catalyst_v0/optional.hpp"

template <class Predicate, class Observer> struct filter_observer_t {
  Predicate m_predicate;
  Observer m_observer;

  void on_start(void *subscription) { m_observer.on_start(subscription); }

  template <class Value> void on_value(Value &&value) {
    if (m_predicate(value))
      m_observer.on_value(std::forward<Value>(value));
  }

  void on_end(void *subscription) { m_observer.on_end(subscription); }
};

template <class Predicate, class Observable> struct filter_t {
  using value_t = value_t<Observable>;

  template <class Observer>
  using subscription_t =
      subscription_t<Observable, filter_observer_t<Predicate, Observer>>;

  static constexpr bool is_synchronous = Observable::is_synchronous;

  Predicate m_predicate;
  Observable m_observable;

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return Observable::subscription(std::forward<Self>(self).m_observable,
                                    filter_observer_t<Predicate, Observer>{
                                        std::forward<Self>(self).m_predicate,
                                        std::forward<Observer>(observer)});
  }
};

template <class Predicate, class Observable>
filter_t<copy_t<Predicate>, copy_t<Observable>>
filter(Predicate &&predicate, Observable &&observable) {
  return {std::forward<Predicate>(predicate),
          std::forward<Observable>(observable)};
}

#if 0
template <class Predicate, class Observable>
auto filter(Predicate &&predicate, Observable &&observable) {
  return merge_map(
      [predicate = std::forward<Predicate>(predicate)](auto &&x) {
        return predicate(x) ? some(std::forward<decltype(x)>(x))
                            : none<decltype(x)>();
      },
      std::forward<Observable>(observable));
}
#endif

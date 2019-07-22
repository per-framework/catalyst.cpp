#pragma once

#include "catalyst_v0/common.hpp"

#include <vector>

template <class Value> struct subject_t;

template <class Value> struct subject_observer_t {
  virtual ~subject_observer_t() {}
  virtual void on_end() = 0;
  virtual void on_value(const Value &value) = 0;
};

template <class Value, class Observer>
struct subject_subscription_t final : subject_observer_t<Value> {
  subject_t<Value> *m_subject;
  Observer m_observer;

  template <class ForwardableObserver>
  subject_subscription_t(subject_t<Value> *subject,
                         ForwardableObserver &&observer)
      : m_subject(subject), m_observer(observer) {}

  virtual void on_end() override { m_observer.on_end(this); }

  virtual void on_value(const Value &value) override {
    m_observer.on_value(value);
  };

  void start() {
    if (m_subject->m_ended) {
      m_observer.on_end(this);
    } else {
      m_subject->m_observers.push_back(this);
    }
  }

  ~subject_subscription_t() {
    // Remove from list of subscriptions
  }
};

template <class Value> struct subject_t {
  using value_t = Value;
  template <class Observer>
  using subscription_t = subject_subscription_t<Value, Observer>;

  static constexpr bool is_synchronous = false;

  bool m_ended;
  std::vector<subject_observer_t<Value> *> m_observers;

  subject_t() : m_ended(false) {}

  void on_end() {
    if (!m_ended) {
      m_ended = true;

      std::vector<subject_observer_t<Value>> observers(m_observers);
      for (auto observer : observers)
        observer->on_end();
    }
  }

  void on_value(const Value &value) {
    if (!m_ended) {
      std::vector<subject_observer_t<Value>> observers(m_observers);
      for (auto observer : observers)
        observer->on_value(value);
    }
  }

  template <class Self, class Observer>
  static subscription_t<copy_t<Observer>> subscription(Self &&self,
                                                       Observer &&observer) {
    return {&self, std::forward<Observer>(observer)};
  }
};

template <class Value> subject_t<Value> subject() { return subject_t<Value>(); }

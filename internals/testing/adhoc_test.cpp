#include "catalyst_v0/filter.hpp"
#include "catalyst_v0/for_each.hpp"
#include "catalyst_v0/from_array.hpp"
#include "catalyst_v0/map.hpp"

#include "testing_v1/test.hpp"

using namespace testing_v1;

static int values[] = {3, 1, 4, 1, 5};

auto adhoc_test = test([]() {
  int sum = 0;

  for_each([&](auto x) { sum += x; },
           filter([](auto x) { return x < 5; },
                  map([](auto x) { return x + 1; }, from_array(values))));

  verify(sum == 8);
});

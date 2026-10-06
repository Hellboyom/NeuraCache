#include "../src/eviction/adaptive_cache.h"

#include <cassert>
#include <iostream>

int main()
{
  Predictor predictor;

  AdaptiveCache cache(
      3,
      predictor);

  /*
   * Build different access histories.
   *
   * hot   -> 10 accesses
   * warm  -> 5 accesses
   * cold  -> 1 access
   */
  for (int i = 0; i < 10; ++i)
  {
    predictor.recordAccess("hot");
  }

  for (int i = 0; i < 5; ++i)
  {
    predictor.recordAccess("warm");
  }

  predictor.recordAccess("cold");

  /*
   * Fill the cache.
   */
  cache.insert("hot");
  cache.insert("warm");
  cache.insert("cold");

  assert(cache.size() == 3);

  /*
   * Insert a new key.
   *
   * The adaptive policy should choose
   * the lowest-scoring key as the victim.
   */
  std::string evicted =
      cache.insert("new");

  assert(!evicted.empty());
  assert(evicted == "cold");

  assert(cache.size() == 3);

  std::cout
      << "PASS: adaptive eviction chooses "
      << "lowest prediction score\n";

  std::cout
      << "Evicted: "
      << evicted
      << "\n";

  std::cout
      << "All adaptive cache tests passed!\n";

  return 0;
}
#include "../src/ai/predictor.h"
#include "../src/eviction/adaptive_cache.h"
#include "../src/eviction/lru_cache.h"

#include <iostream>
#include <string>
#include <vector>

struct BenchmarkResult
{
  int hits;
  int misses;
  int evictions;
};

BenchmarkResult runLRU()
{
  const std::size_t capacity = 10;

  LRUCache cache(capacity);

  std::vector<std::string> hotKeys;

  for (int i = 0; i < 5; ++i)
  {
    hotKeys.push_back(
        "hot" + std::to_string(i));
  }

  int hits = 0;
  int misses = 0;
  int evictions = 0;

  /*
   * Initially populate the cache
   * with the hot keys.
   */
  for (const auto &key : hotKeys)
  {
    cache.insert(key);
  }

  /*
   * Repeatedly scan through new cold keys.
   *
   * This is deliberately hostile to LRU.
   */
  for (int round = 0; round < 100; ++round)
  {
    /*
     * Insert 10 new cold keys.
     */
    for (int i = 0; i < 10; ++i)
    {
      std::string key =
          "cold" +
          std::to_string(
              round * 10 + i);

      if (cache.contains(key))
      {
        cache.touch(key);
        hits++;
      }
      else
      {
        misses++;

        if (cache.insert(key).has_value())
        {
          evictions++;
        }
      }
    }

    /*
     * Access all hot keys.
     */
    for (const auto &key : hotKeys)
    {
      if (cache.contains(key))
      {
        cache.touch(key);
        hits++;
      }
      else
      {
        misses++;

        if (cache.insert(key).has_value())
        {
          evictions++;
        }
      }
    }
  }

  return {
      hits,
      misses,
      evictions};
}

BenchmarkResult runAdaptive()
{
  const std::size_t capacity = 10;

  Predictor predictor;

  AdaptiveCache cache(
      capacity,
      predictor);

  std::vector<std::string> hotKeys;

  for (int i = 0; i < 5; ++i)
  {
    hotKeys.push_back(
        "hot" + std::to_string(i));
  }

  int hits = 0;
  int misses = 0;
  int evictions = 0;

  /*
   * Populate the cache with hot keys.
   */
  for (const auto &key : hotKeys)
  {
    cache.insert(key);
  }

  /*
   * Train the predictor on the hot keys.
   */
  for (int i = 0; i < 500; ++i)
  {
    predictor.recordAccess(
        hotKeys[i % hotKeys.size()]);
  }

  /*
   * Same workload as the LRU benchmark.
   */
  for (int round = 0; round < 100; ++round)
  {
    /*
     * Insert 10 new cold keys.
     */
    for (int i = 0; i < 10; ++i)
    {
      std::string key =
          "cold" +
          std::to_string(
              round * 10 + i);

      if (cache.contains(key))
      {
        cache.touch(key);
        hits++;
      }
      else
      {
        misses++;

        std::string evicted =
            cache.insert(key);

        if (!evicted.empty())
        {
          evictions++;
        }
      }

      predictor.recordAccess(key);
    }

    /*
     * Access all hot keys.
     */
    for (const auto &key : hotKeys)
    {
      if (cache.contains(key))
      {
        cache.touch(key);
        hits++;
      }
      else
      {
        misses++;

        std::string evicted =
            cache.insert(key);

        if (!evicted.empty())
        {
          evictions++;
        }
      }

      predictor.recordAccess(key);
    }
  }

  return {
      hits,
      misses,
      evictions};
}

void printResult(
    const std::string &name,
    const BenchmarkResult &result)
{
  int total =
      result.hits + result.misses;

  double hitRate =
      total > 0
          ? static_cast<double>(result.hits) /
                static_cast<double>(total) *
                100.0
          : 0.0;

  std::cout
      << "\n"
      << name
      << "\n"
      << "-----------------------------\n"
      << "Hits: "
      << result.hits
      << "\n"
      << "Misses: "
      << result.misses
      << "\n"
      << "Evictions: "
      << result.evictions
      << "\n"
      << "Hit rate: "
      << hitRate
      << "%\n";
}

int main()
{
  std::cout
      << "====================================\n"
      << "   NeuraCache Adaptive Benchmark\n"
      << "====================================\n";

  BenchmarkResult lru =
      runLRU();

  BenchmarkResult adaptive =
      runAdaptive();

  printResult(
      "Traditional LRU",
      lru);

  printResult(
      "Adaptive Cache",
      adaptive);

  std::cout
      << "\nBenchmark complete.\n";

  return 0;
}
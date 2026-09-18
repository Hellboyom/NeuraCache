#include "../src/metrics/metrics.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

void testInitialValues()
{
  Metrics metrics;

  assert(metrics.getTotalCommands() == 0);
  assert(metrics.getGetCommands() == 0);
  assert(metrics.getSetCommands() == 0);
  assert(metrics.getDelCommands() == 0);
  assert(metrics.getCacheHits() == 0);
  assert(metrics.getCacheMisses() == 0);
  assert(metrics.getEvictions() == 0);
  assert(metrics.getExpiredKeys() == 0);

  std::cout << "PASS: initial metrics" << std::endl;
}

void testCommandCounters()
{
  Metrics metrics;

  metrics.recordCommand();
  metrics.recordCommand();

  metrics.recordGet();
  metrics.recordSet();
  metrics.recordSet();
  metrics.recordDel();

  assert(metrics.getTotalCommands() == 2);
  assert(metrics.getGetCommands() == 1);
  assert(metrics.getSetCommands() == 2);
  assert(metrics.getDelCommands() == 1);

  std::cout << "PASS: command counters" << std::endl;
}

void testCacheCounters()
{
  Metrics metrics;

  metrics.recordCacheHit();
  metrics.recordCacheHit();
  metrics.recordCacheMiss();

  metrics.recordEviction();
  metrics.recordEviction();

  metrics.recordExpiredKey();

  assert(metrics.getCacheHits() == 2);
  assert(metrics.getCacheMisses() == 1);
  assert(metrics.getEvictions() == 2);
  assert(metrics.getExpiredKeys() == 1);

  std::cout << "PASS: cache counters" << std::endl;
}

void testUptime()
{
  Metrics metrics;

  std::uint64_t initialUptime =
      metrics.getUptimeSeconds();

  std::this_thread::sleep_for(
      std::chrono::seconds(1));

  std::uint64_t laterUptime =
      metrics.getUptimeSeconds();

  assert(laterUptime >= initialUptime);

  std::cout << "PASS: uptime" << std::endl;
}

void testInfo()
{
  Metrics metrics;

  metrics.recordCommand();
  metrics.recordGet();
  metrics.recordSet();
  metrics.recordCacheHit();
  metrics.recordCacheMiss();
  metrics.recordEviction();
  metrics.recordExpiredKey();

  std::string output =
      metrics.info();

  assert(
      output.find(
          "total_commands:1") != std::string::npos);

  assert(
      output.find(
          "get_commands:1") != std::string::npos);

  assert(
      output.find(
          "set_commands:1") != std::string::npos);

  assert(
      output.find(
          "cache_hits:1") != std::string::npos);

  assert(
      output.find(
          "cache_misses:1") != std::string::npos);

  assert(
      output.find(
          "evictions:1") != std::string::npos);

  assert(
      output.find(
          "expired_keys:1") != std::string::npos);

  std::cout << "PASS: INFO output" << std::endl;
}

int main()
{
  testInitialValues();
  testCommandCounters();
  testCacheCounters();
  testUptime();
  testInfo();

  std::cout << std::endl;
  std::cout
      << "All Metrics tests passed!"
      << std::endl;

  return 0;
}
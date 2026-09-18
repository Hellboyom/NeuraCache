#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>

class Metrics
{
private:
  std::chrono::steady_clock::time_point startTime;

  std::atomic<std::uint64_t> totalCommands;
  std::atomic<std::uint64_t> getCommands;
  std::atomic<std::uint64_t> setCommands;
  std::atomic<std::uint64_t> delCommands;
  std::atomic<std::uint64_t> cacheHits;
  std::atomic<std::uint64_t> cacheMisses;
  std::atomic<std::uint64_t> evictions;
  std::atomic<std::uint64_t> expiredKeys;

public:
  Metrics();

  void recordCommand();

  void recordGet();

  void recordSet();

  void recordDel();

  void recordCacheHit();

  void recordCacheMiss();

  void recordEviction();

  void recordExpiredKey();

  std::uint64_t getTotalCommands() const;

  std::uint64_t getGetCommands() const;

  std::uint64_t getSetCommands() const;

  std::uint64_t getDelCommands() const;

  std::uint64_t getCacheHits() const;

  std::uint64_t getCacheMisses() const;

  std::uint64_t getEvictions() const;

  std::uint64_t getExpiredKeys() const;

  std::uint64_t getUptimeSeconds() const;

  std::string info() const;
};
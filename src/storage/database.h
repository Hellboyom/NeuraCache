#pragma once

#include "../eviction/lru_cache.h"
#include "../persistence/snapshot.h"
#include "../metrics/metrics.h"

#include <chrono>
#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

class Database
{
public:
  Database();

  explicit Database(
      Metrics &metrics);

  void setMetrics(
      Metrics *metrics);

  void set(
      const std::string &key,
      const std::string &value);

  void set(
      const std::string &key,
      const std::string &value,
      long long ttlSeconds);

  bool get(
      const std::string &key,
      std::string &value);

  bool del(
      const std::string &key);

  bool exists(
      const std::string &key);

  bool expire(
      const std::string &key,
      long long ttlSeconds);

  long long ttl(
      const std::string &key);

  std::size_t size() const;

  void clear();

  void setCapacity(
      std::size_t capacity);

  std::size_t capacity() const;

  bool saveSnapshot(
      const std::string &filename);

  bool loadSnapshot(
      const std::string &filename);

private:
  struct Entry
  {
    std::string value;

    std::optional<
        std::chrono::steady_clock::time_point>
        expiresAt;
  };

  bool isExpired(
      const Entry &entry) const;

  void removeExpired(
      const std::string &key);

  void removeEvictedKeys(
      const std::optional<std::string> &evictedKey);

  std::unordered_map<
      std::string,
      Entry>
      data;

  LRUCache lru;

  Metrics *metrics;

  mutable std::mutex mutex;
};
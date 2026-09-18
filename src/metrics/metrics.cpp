#include "metrics.h"

#include <sstream>

Metrics::Metrics()
    : startTime(std::chrono::steady_clock::now()),
      totalCommands(0),
      getCommands(0),
      setCommands(0),
      delCommands(0),
      cacheHits(0),
      cacheMisses(0),
      evictions(0),
      expiredKeys(0)
{
}

void Metrics::recordCommand()
{
  totalCommands.fetch_add(1);
}

void Metrics::recordGet()
{
  getCommands.fetch_add(1);
}

void Metrics::recordSet()
{
  setCommands.fetch_add(1);
}

void Metrics::recordDel()
{
  delCommands.fetch_add(1);
}

void Metrics::recordCacheHit()
{
  cacheHits.fetch_add(1);
}

void Metrics::recordCacheMiss()
{
  cacheMisses.fetch_add(1);
}

void Metrics::recordEviction()
{
  evictions.fetch_add(1);
}

void Metrics::recordExpiredKey()
{
  expiredKeys.fetch_add(1);
}

std::uint64_t Metrics::getTotalCommands() const
{
  return totalCommands.load();
}

std::uint64_t Metrics::getGetCommands() const
{
  return getCommands.load();
}

std::uint64_t Metrics::getSetCommands() const
{
  return setCommands.load();
}

std::uint64_t Metrics::getDelCommands() const
{
  return delCommands.load();
}

std::uint64_t Metrics::getCacheHits() const
{
  return cacheHits.load();
}

std::uint64_t Metrics::getCacheMisses() const
{
  return cacheMisses.load();
}

std::uint64_t Metrics::getEvictions() const
{
  return evictions.load();
}

std::uint64_t Metrics::getExpiredKeys() const
{
  return expiredKeys.load();
}

std::uint64_t Metrics::getUptimeSeconds() const
{
  auto now =
      std::chrono::steady_clock::now();

  auto elapsed =
      std::chrono::duration_cast<
          std::chrono::seconds>(
          now - startTime);

  return static_cast<std::uint64_t>(
      elapsed.count());
}

std::string Metrics::info() const
{
  std::ostringstream output;

  output
      << "# NeuraCache\r\n"
      << "uptime_seconds:"
      << getUptimeSeconds()
      << "\r\n"
      << "total_commands:"
      << getTotalCommands()
      << "\r\n"
      << "get_commands:"
      << getGetCommands()
      << "\r\n"
      << "set_commands:"
      << getSetCommands()
      << "\r\n"
      << "del_commands:"
      << getDelCommands()
      << "\r\n"
      << "cache_hits:"
      << getCacheHits()
      << "\r\n"
      << "cache_misses:"
      << getCacheMisses()
      << "\r\n"
      << "evictions:"
      << getEvictions()
      << "\r\n"
      << "expired_keys:"
      << getExpiredKeys()
      << "\r\n";

  return output.str();
}
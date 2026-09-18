#pragma once

#include <string>
#include <vector>

struct SnapshotEntry
{
  std::string key;
  std::string value;
  long long ttlMilliseconds;
};

class Snapshot
{
public:
  static bool save(
      const std::string &filename,
      const std::vector<SnapshotEntry> &entries);

  static bool load(
      const std::string &filename,
      std::vector<SnapshotEntry> &entries);
};
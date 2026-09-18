#include "snapshot.h"

#include <cstdint>
#include <fstream>
#include <algorithm>
#include <limits>
#include <utility>

namespace
{

  void writeUint64(
      std::ofstream &file,
      std::uint64_t value)
  {
    file.write(
        reinterpret_cast<const char *>(&value),
        sizeof(value));
  }

  bool readUint64(
      std::ifstream &file,
      std::uint64_t &value)
  {
    file.read(
        reinterpret_cast<char *>(&value),
        sizeof(value));

    return file.good();
  }

  void writeString(
      std::ofstream &file,
      const std::string &value)
  {
    writeUint64(
        file,
        static_cast<std::uint64_t>(
            value.size()));

    if (!value.empty())
    {
      file.write(
          value.data(),
          static_cast<std::streamsize>(
              value.size()));
    }
  }

  bool readString(
      std::ifstream &file,
      std::string &value)
  {
    std::uint64_t length;

    if (!readUint64(file, length))
    {
      return false;
    }

    if (length >
        static_cast<std::uint64_t>(
            std::numeric_limits<std::size_t>::max()))
    {
      return false;
    }

    value.resize(
        static_cast<std::size_t>(
            length));

    if (length > 0)
    {
      file.read(
          &value[0],
          static_cast<std::streamsize>(
              length));
    }

    return file.good();
  }

}

bool Snapshot::save(
    const std::string &filename,
    const std::vector<SnapshotEntry> &entries)
{
  std::ofstream file(
      filename,
      std::ios::binary |
          std::ios::trunc);

  if (!file.is_open())
  {
    return false;
  }

  const char magic[] = "NCACHE01";

  file.write(
      magic,
      sizeof(magic) - 1);

  writeUint64(
      file,
      static_cast<std::uint64_t>(
          entries.size()));

  for (const SnapshotEntry &entry : entries)
  {
    writeString(
        file,
        entry.key);

    writeString(
        file,
        entry.value);

    file.write(
        reinterpret_cast<const char *>(
            &entry.ttlMilliseconds),
        sizeof(entry.ttlMilliseconds));

    if (!file.good())
    {
      return false;
    }
  }

  return true;
}

bool Snapshot::load(
    const std::string &filename,
    std::vector<SnapshotEntry> &entries)
{
  entries.clear();

  std::ifstream file(
      filename,
      std::ios::binary);

  if (!file.is_open())
  {
    return false;
  }

  const char expectedMagic[] = "NCACHE01";

  char magic[sizeof(expectedMagic) - 1];

  file.read(
      magic,
      sizeof(magic));

  if (!file.good())
  {
    return false;
  }

  if (!std::equal(
          magic,
          magic + sizeof(magic),
          expectedMagic))
  {
    return false;
  }

  std::uint64_t entryCount;

  if (!readUint64(
          file,
          entryCount))
  {
    return false;
  }

  if (entryCount > 1000000)
  {
    return false;
  }

  entries.reserve(
      static_cast<std::size_t>(
          entryCount));

  for (
      std::uint64_t i = 0;
      i < entryCount;
      ++i)
  {
    SnapshotEntry entry;

    if (!readString(
            file,
            entry.key))
    {
      entries.clear();
      return false;
    }

    if (!readString(
            file,
            entry.value))
    {
      entries.clear();
      return false;
    }

    file.read(
        reinterpret_cast<char *>(
            &entry.ttlMilliseconds),
        sizeof(entry.ttlMilliseconds));

    if (!file.good())
    {
      entries.clear();
      return false;
    }

    if (entry.ttlMilliseconds < -1)
    {
      entries.clear();
      return false;
    }

    entries.push_back(
        std::move(entry));
  }

  return true;
}
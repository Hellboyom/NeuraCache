#include "../src/persistence/snapshot.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

void testSaveAndLoad()
{
  const std::string filename =
      "test_snapshot.dat";

  std::vector<SnapshotEntry> original =
      {
          {"A",
           "Hello",
           -1},
          {"B",
           "World",
           5000},
          {"C",
           "Value with spaces",
           10000}};

  bool saved =
      Snapshot::save(
          filename,
          original);

  assert(saved);

  std::vector<SnapshotEntry> loaded;

  bool loadedSuccessfully =
      Snapshot::load(
          filename,
          loaded);

  assert(loadedSuccessfully);

  assert(
      loaded.size() ==
      original.size());

  for (std::size_t i = 0;
       i < original.size();
       ++i)
  {
    assert(
        loaded[i].key ==
        original[i].key);

    assert(
        loaded[i].value ==
        original[i].value);

    assert(
        loaded[i].ttlMilliseconds ==
        original[i].ttlMilliseconds);
  }

  std::remove(
      filename.c_str());

  std::cout
      << "PASS: snapshot save and load"
      << std::endl;
}

void testMissingFile()
{
  std::vector<SnapshotEntry> entries;

  bool result =
      Snapshot::load(
          "does_not_exist.dat",
          entries);

  assert(!result);

  std::cout
      << "PASS: missing snapshot"
      << std::endl;
}

void testInvalidFile()
{
  const std::string filename =
      "invalid_snapshot.dat";

  {
    FILE *file =
        std::fopen(
            filename.c_str(),
            "wb");

    assert(file != nullptr);

    const char data[] =
        "INVALID";

    std::fwrite(
        data,
        1,
        sizeof(data) - 1,
        file);

    std::fclose(file);
  }

  std::vector<SnapshotEntry> entries;

  bool result =
      Snapshot::load(
          filename,
          entries);

  assert(!result);

  std::remove(
      filename.c_str());

  std::cout
      << "PASS: invalid snapshot"
      << std::endl;
}

int main()
{
  testSaveAndLoad();
  testMissingFile();
  testInvalidFile();

  std::cout << std::endl;

  std::cout
      << "All Snapshot tests passed!"
      << std::endl;

  return 0;
}
#include "../src/storage/database.h"

#include <cassert>
#include <cstdio>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void testSetAndGet()
{
  Database database;

  database.set("A", "Hello");

  std::string value;

  bool found =
      database.get("A", value);

  assert(found);
  assert(value == "Hello");

  std::cout << "PASS: SET and GET" << std::endl;
}

void testGetMissingKey()
{
  Database database;

  std::string value;

  bool found =
      database.get("missing", value);

  assert(!found);

  std::cout << "PASS: GET missing key" << std::endl;
}

void testDelete()
{
  Database database;

  database.set("A", "Hello");

  bool deleted =
      database.del("A");

  assert(deleted);

  std::string value;

  bool found =
      database.get("A", value);

  assert(!found);

  bool deletedAgain =
      database.del("A");

  assert(!deletedAgain);

  std::cout << "PASS: DEL" << std::endl;
}

void testExists()
{
  Database database;

  database.set("A", "Hello");

  assert(database.exists("A"));
  assert(!database.exists("B"));

  database.del("A");

  assert(!database.exists("A"));

  std::cout << "PASS: EXISTS" << std::endl;
}

void testExpire()
{
  Database database;

  database.set("A", "Hello");

  bool result =
      database.expire("A", 2);

  assert(result);

  long long ttl =
      database.ttl("A");

  assert(ttl >= 0);
  assert(ttl <= 2);

  std::cout << "PASS: EXPIRE" << std::endl;
}

void testExpiredKey()
{
  Database database;

  database.set("A", "Hello", 1);

  std::string value;

  bool foundImmediately =
      database.get("A", value);

  assert(foundImmediately);
  assert(value == "Hello");

  std::this_thread::sleep_for(
      std::chrono::milliseconds(1200));

  bool foundAfterExpiration =
      database.get("A", value);

  assert(!foundAfterExpiration);

  assert(database.ttl("A") == -2);

  std::cout << "PASS: automatic expiration" << std::endl;
}

void testSetWithTTL()
{
  Database database;

  database.set(
      "A",
      "Hello",
      10);

  std::string value;

  bool found =
      database.get("A", value);

  assert(found);
  assert(value == "Hello");

  long long ttl =
      database.ttl("A");

  assert(ttl >= 0);
  assert(ttl <= 10);

  std::cout << "PASS: SET with TTL" << std::endl;
}

void testPersistentSet()
{
  Database database;

  database.set(
      "A",
      "Hello",
      10);

  database.set(
      "A",
      "Updated");

  long long ttl =
      database.ttl("A");

  assert(ttl == -1);

  std::string value;

  bool found =
      database.get("A", value);

  assert(found);
  assert(value == "Updated");

  std::cout << "PASS: SET removes previous TTL" << std::endl;
}

void testLRUEviction()
{
  Database database;

  database.set("A", "1");
  database.set("B", "2");
  database.set("C", "3");

  std::string value;

  bool found =
      database.get("A", value);

  assert(found);

  database.set("D", "4");

  assert(!database.exists("B"));
  assert(database.exists("A"));
  assert(database.exists("C"));
  assert(database.exists("D"));

  std::cout << "PASS: Database LRU eviction" << std::endl;
}

void testFlushDB()
{
  Database database;

  database.set("A", "1");
  database.set("B", "2");
  database.set("C", "3");

  database.clear();

  assert(!database.exists("A"));
  assert(!database.exists("B"));
  assert(!database.exists("C"));

  std::cout << "PASS: FLUSHDB / clear" << std::endl;
}

void testCapacity()
{
  Database database;

  assert(database.capacity() == 3);

  database.setCapacity(2);

  assert(database.capacity() == 2);

  database.set("A", "1");
  database.set("B", "2");
  database.set("C", "3");

  assert(!database.exists("A"));
  assert(database.exists("B"));
  assert(database.exists("C"));

  std::cout << "PASS: capacity" << std::endl;
}
void testSnapshotPersistence()
{
  const std::string filename =
      "test_database_snapshot.dat";

  {
    Database database;

    database.set(
        "A",
        "Hello");

    database.set(
        "B",
        "World",
        10);

    bool saved =
        database.saveSnapshot(
            filename);

    assert(saved);
  }

  {
    Database database;

    bool loaded =
        database.loadSnapshot(
            filename);

    assert(loaded);

    std::string value;

    bool found =
        database.get(
            "A",
            value);

    assert(found);
    assert(value == "Hello");

    found =
        database.get(
            "B",
            value);

    assert(found);
    assert(value == "World");

    long long ttl =
        database.ttl("B");

    assert(ttl >= 0);
    assert(ttl <= 10);
  }

  std::remove(
      filename.c_str());

  std::cout
      << "PASS: snapshot persistence"
      << std::endl;
}

int main()
{
  testSetAndGet();
  testGetMissingKey();
  testDelete();
  testExists();
  testExpire();
  testExpiredKey();
  testSetWithTTL();
  testPersistentSet();
  testLRUEviction();
  testFlushDB();
  testCapacity();
  testSnapshotPersistence();

  std::cout << std::endl;
  std::cout << "All Database tests passed!" << std::endl;

  return 0;
}
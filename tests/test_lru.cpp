#include "../src/eviction/lru_cache.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

void testInsert()
{
  LRUCache cache(3);

  assert(cache.size() == 0);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");

  assert(cache.size() == 3);

  std::cout << "PASS: insert and size" << std::endl;
}

void testEviction()
{
  LRUCache cache(3);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");

  std::optional<std::string> evicted =
      cache.insert("D");

  assert(evicted.has_value());
  assert(evicted.value() == "A");

  assert(cache.size() == 3);

  std::cout << "PASS: LRU eviction" << std::endl;
}

void testTouch()
{
  LRUCache cache(3);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");

  cache.touch("A");

  std::optional<std::string> evicted =
      cache.insert("D");

  assert(evicted.has_value());
  assert(evicted.value() == "B");

  std::cout << "PASS: touch updates recency" << std::endl;
}

void testRemove()
{
  LRUCache cache(3);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");

  cache.remove("B");

  assert(cache.size() == 2);

  std::optional<std::string> evicted =
      cache.insert("D");

  assert(!evicted.has_value());

  assert(cache.size() == 3);

  std::cout << "PASS: remove" << std::endl;
}

void testClear()
{
  LRUCache cache(3);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");

  cache.clear();

  assert(cache.size() == 0);

  std::cout << "PASS: clear" << std::endl;
}

void testUpdateExistingKey()
{
  LRUCache cache(3);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");

  std::optional<std::string> evicted =
      cache.insert("B");

  assert(!evicted.has_value());
  assert(cache.size() == 3);

  evicted = cache.insert("D");

  assert(evicted.has_value());
  assert(evicted.value() == "A");

  std::cout << "PASS: existing key update" << std::endl;
}

void testSetCapacity()
{
  LRUCache cache(4);

  cache.insert("A");
  cache.insert("B");
  cache.insert("C");
  cache.insert("D");

  std::vector<std::string> evicted =
      cache.setCapacity(2);

  assert(cache.size() == 2);
  assert(evicted.size() == 2);

  assert(evicted[0] == "A");
  assert(evicted[1] == "B");

  std::cout << "PASS: set capacity" << std::endl;
}

int main()
{
  testInsert();
  testEviction();
  testTouch();
  testRemove();
  testClear();
  testUpdateExistingKey();
  testSetCapacity();

  std::cout << std::endl;
  std::cout << "All LRU tests passed!" << std::endl;

  return 0;
}
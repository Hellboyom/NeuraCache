#include "../src/storage/database.h"

#include <cassert>
#include <iostream>
#include <string>

int main()
{
  Database database;

  /*
   * Fill the cache.
   *
   * Database capacity is currently 3.
   */
  database.set("hot", "hot-value");
  database.set("warm", "warm-value");
  database.set("cold", "cold-value");

  /*
   * Build different access patterns.
   *
   * hot  -> heavily accessed
   * warm -> moderately accessed
   * cold -> never accessed
   */
  std::string value;

  for (int i = 0; i < 10; ++i)
  {
    database.get("hot", value);
  }

  for (int i = 0; i < 5; ++i)
  {
    database.get("warm", value);
  }

  /*
   * Insert a new key.
   *
   * The adaptive policy should prefer
   * evicting the least valuable key.
   */
  database.set("new", "new-value");

  /*
   * hot and warm should survive.
   */
  assert(database.exists("hot"));
  assert(database.exists("warm"));

  /*
   * The cache should still contain
   * exactly three entries.
   */
  assert(database.size() == 3);

  std::cout
      << "PASS: hot key survived adaptive eviction\n";

  std::cout
      << "PASS: warm key survived adaptive eviction\n";

  std::cout
      << "PASS: cache capacity maintained\n";

  std::cout
      << "All adaptive integration tests passed!\n";

  return 0;
}
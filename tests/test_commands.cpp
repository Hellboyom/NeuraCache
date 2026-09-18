#include "../src/commands/command_handler.h"
#include "../src/storage/database.h"
#include "../src/metrics/metrics.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

void testPing()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"PING"});

  assert(response == "+PONG\r\n");

  std::cout << "PASS: PING" << std::endl;
}

void testSet()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"SET",
                       "A",
                       "Hello"});

  assert(response == "+OK\r\n");

  std::string value;

  assert(database.get("A", value));
  assert(value == "Hello");

  std::cout << "PASS: SET" << std::endl;
}

void testGet()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  database.set("A", "Hello");

  std::string response =
      handler.execute({"GET",
                       "A"});

  assert(response == "$5\r\nHello\r\n");

  std::cout << "PASS: GET" << std::endl;
}

void testGetMissing()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"GET",
                       "missing"});

  assert(response == "$-1\r\n");

  std::cout << "PASS: GET missing key" << std::endl;
}

void testDelete()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  database.set("A", "Hello");

  std::string response =
      handler.execute({"DEL",
                       "A"});

  assert(response == ":1\r\n");
  assert(!database.exists("A"));

  response =
      handler.execute({"DEL",
                       "A"});

  assert(response == ":0\r\n");

  std::cout << "PASS: DEL" << std::endl;
}

void testExists()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  database.set("A", "Hello");

  std::string response =
      handler.execute({"EXISTS",
                       "A"});

  assert(response == ":1\r\n");

  response =
      handler.execute({"EXISTS",
                       "B"});

  assert(response == ":0\r\n");

  std::cout << "PASS: EXISTS" << std::endl;
}

void testSetWithTTL()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"SET",
                       "A",
                       "Hello",
                       "EX",
                       "10"});

  assert(response == "+OK\r\n");

  long long ttl =
      database.ttl("A");

  assert(ttl >= 0);
  assert(ttl <= 10);

  std::cout << "PASS: SET EX" << std::endl;
}

void testExpire()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  database.set("A", "Hello");

  std::string response =
      handler.execute({"EXPIRE",
                       "A",
                       "10"});

  assert(response == ":1\r\n");

  long long ttl =
      database.ttl("A");

  assert(ttl >= 0);
  assert(ttl <= 10);

  std::cout << "PASS: EXPIRE" << std::endl;
}

void testTTL()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  database.set("A", "Hello");

  std::string response =
      handler.execute({"TTL",
                       "A"});

  assert(response == ":-1\r\n");

  database.set("B", "World", 10);

  response =
      handler.execute({"TTL",
                       "B"});

  assert(
      response == ":10\r\n" ||
      response == ":9\r\n");

  response =
      handler.execute({"TTL",
                       "missing"});

  assert(response == ":-2\r\n");

  std::cout << "PASS: TTL" << std::endl;
}

void testFlushDB()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  database.set("A", "1");
  database.set("B", "2");

  std::string response =
      handler.execute({"FLUSHDB"});

  assert(response == "+OK\r\n");

  assert(!database.exists("A"));
  assert(!database.exists("B"));

  std::cout << "PASS: FLUSHDB" << std::endl;
}

void testUnknownCommand()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"NOPE"});

  assert(response == "-ERR unknown command\r\n");

  std::cout << "PASS: unknown command" << std::endl;
}

void testWrongArguments()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"GET"});

  assert(
      response ==
      "-ERR wrong number of arguments for GET\r\n");

  response =
      handler.execute({"PING",
                       "extra"});

  assert(
      response ==
      "-ERR wrong number of arguments for PING\r\n");

  std::cout << "PASS: wrong arguments" << std::endl;
}

void testCaseInsensitiveCommands()
{
  Database database;
  Metrics metrics;
  CommandHandler handler(
      database,
      metrics);

  std::string response =
      handler.execute({"ping"});

  assert(response == "+PONG\r\n");

  response =
      handler.execute({"set",
                       "A",
                       "Hello"});

  assert(response == "+OK\r\n");

  response =
      handler.execute({"get",
                       "A"});

  assert(response == "$5\r\nHello\r\n");

  std::cout << "PASS: case-insensitive commands" << std::endl;
}

int main()
{
  testPing();
  testSet();
  testGet();
  testGetMissing();
  testDelete();
  testExists();
  testSetWithTTL();
  testExpire();
  testTTL();
  testFlushDB();
  testUnknownCommand();
  testWrongArguments();
  testCaseInsensitiveCommands();

  std::cout << std::endl;
  std::cout << "All Command Handler tests passed!" << std::endl;

  return 0;
}
#include "command_handler.h"

#include <algorithm>
#include <cctype>
#include <sstream>

std::string CommandHandler::upper(
    const std::string &value) const
{
  std::string result = value;

  std::transform(
      result.begin(),
      result.end(),
      result.begin(),
      [](unsigned char character)
      {
        return static_cast<char>(
            std::toupper(character));
      });

  return result;
}

bool CommandHandler::parseLongLong(
    const std::string &value,
    long long &result) const
{
  try
  {
    std::size_t consumed = 0;

    result = std::stoll(
        value,
        &consumed);

    return consumed == value.size();
  }
  catch (...)
  {
    return false;
  }
}

CommandHandler::CommandHandler(
    Database &database,
    Metrics &metrics)
    : database(database),
      metrics(metrics)
{
}

std::string CommandHandler::execute(
    const std::vector<std::string> &command)
{
  if (command.empty())
  {
    return "-ERR empty command\r\n";
  }

  metrics.recordCommand();

  const std::string operation =
      upper(command[0]);

  if (operation == "PING")
  {
    if (command.size() == 1)
    {
      return "+PONG\r\n";
    }

    return "-ERR wrong number of arguments for PING\r\n";
  }

  if (operation == "SET")
  {
    metrics.recordSet();

    if (command.size() == 3)
    {
      database.set(
          command[1],
          command[2]);

      return "+OK\r\n";
    }

    if (command.size() == 5 &&
        upper(command[3]) == "EX")
    {
      long long seconds;

      if (!parseLongLong(
              command[4],
              seconds) ||
          seconds <= 0)
      {
        return "-ERR invalid expiration\r\n";
      }

      database.set(
          command[1],
          command[2],
          seconds);

      return "+OK\r\n";
    }

    return "-ERR syntax error\r\n";
  }

  if (operation == "GET")
  {
    metrics.recordGet();

    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for GET\r\n";
    }

    std::string value;

    if (!database.get(
            command[1],
            value))
    {
      metrics.recordCacheMiss();

      return "$-1\r\n";
    }

    metrics.recordCacheHit();

    return "$" +
           std::to_string(value.size()) +
           "\r\n" +
           value +
           "\r\n";
  }

  if (operation == "DEL")
  {
    metrics.recordDel();

    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for DEL\r\n";
    }

    return database.del(command[1])
               ? ":1\r\n"
               : ":0\r\n";
  }

  if (operation == "EXISTS")
  {
    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for EXISTS\r\n";
    }

    return database.exists(command[1])
               ? ":1\r\n"
               : ":0\r\n";
  }

  if (operation == "EXPIRE")
  {
    if (command.size() != 3)
    {
      return "-ERR wrong number of arguments for EXPIRE\r\n";
    }

    long long seconds;

    if (!parseLongLong(
            command[2],
            seconds))
    {
      return "-ERR invalid expiration\r\n";
    }

    return database.expire(
               command[1],
               seconds)
               ? ":1\r\n"
               : ":0\r\n";
  }

  if (operation == "TTL")
  {
    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for TTL\r\n";
    }

    return ":" +
           std::to_string(
               database.ttl(command[1])) +
           "\r\n";
  }

  if (operation == "DBSIZE")
  {
    if (command.size() != 1)
    {
      return "-ERR wrong number of arguments for DBSIZE\r\n";
    }

    return ":" +
           std::to_string(
               database.size()) +
           "\r\n";
  }

  if (operation == "FLUSHDB")
  {
    if (command.size() != 1)
    {
      return "-ERR wrong number of arguments for FLUSHDB\r\n";
    }

    database.clear();

    return "+OK\r\n";
  }

  if (operation == "INFO")
  {
    if (command.size() != 1)
    {
      return "-ERR wrong number of arguments for INFO\r\n";
    }

    std::string information =
        metrics.info();

    return "$" +
           std::to_string(
               information.size()) +
           "\r\n" +
           information +
           "\r\n";
  }

  return "-ERR unknown command\r\n";
}
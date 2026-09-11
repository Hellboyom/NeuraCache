#include "command_handler.h"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace
{

  std::string upper(
      std::string value)
  {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char character)
        {
          return static_cast<char>(
              std::toupper(character));
        });

    return value;
  }

  bool parseLongLong(
      const std::string &value,
      long long &result)
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

}

CommandHandler::CommandHandler(
    Database &database)
    : database(database)
{
}

std::string CommandHandler::execute(
    const std::vector<std::string> &command)
{
  if (command.empty())
  {
    return "-ERR empty command\r\n";
  }

  const std::string operation =
      upper(command[0]);
  std::cerr << "DEBUG argc=" << command.size()
            << " length=" << operation.size()
            << " operation=[" << operation << "] bytes=";

  for (unsigned char character : operation)
  {
    std::cerr << static_cast<int>(character) << " ";
  }

  std::cerr << std::endl;

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
    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for GET\r\n";
    }

    std::string value;

    if (!database.get(
            command[1],
            value))
    {
      return "$-1\r\n";
    }

    return "$" +
           std::to_string(value.size()) +
           "\r\n" +
           value +
           "\r\n";
  }

  if (operation == "DEL")
  {
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

  return "-ERR unknown command\r\n";
}
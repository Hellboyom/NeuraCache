#include "command_handler.h"

#include <algorithm>

CommandHandler::CommandHandler(Database &database)
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

  std::string operation = command[0];

  std::transform(
      operation.begin(),
      operation.end(),
      operation.begin(),
      ::toupper);

  // PING
  if (operation == "PING")
  {

    if (command.size() == 1)
    {
      return "+PONG\r\n";
    }

    return "-ERR wrong number of arguments\r\n";
  }

  // SET key value
  if (operation == "SET")
  {

    if (command.size() != 3)
    {
      return "-ERR wrong number of arguments for SET\r\n";
    }

    database.set(
        command[1],
        command[2]);

    return "+OK\r\n";
  }

  // GET key
  if (operation == "GET")
  {

    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for GET\r\n";
    }

    std::string value;

    if (!database.get(command[1], value))
    {
      return "$-1\r\n";
    }

    return "$" +
           std::to_string(value.size()) +
           "\r\n" +
           value +
           "\r\n";
  }

  // DEL key
  if (operation == "DEL")
  {

    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for DEL\r\n";
    }

    bool deleted = database.del(command[1]);

    return deleted
               ? ":1\r\n"
               : ":0\r\n";
  }

  // EXISTS key
  if (operation == "EXISTS")
  {

    if (command.size() != 2)
    {
      return "-ERR wrong number of arguments for EXISTS\r\n";
    }

    bool exists = database.exists(command[1]);

    return exists
               ? ":1\r\n"
               : ":0\r\n";
  }

  return "-ERR unknown command\r\n";
}
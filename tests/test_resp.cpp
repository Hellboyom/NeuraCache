#include "../src/protocol/resp.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

void testPing()
{
  std::string input =
      "*1\r\n"
      "$4\r\n"
      "PING\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Complete);

  assert(command.size() == 1);
  assert(command[0] == "PING");

  assert(consumedBytes == input.size());

  std::cout << "PASS: parse PING" << std::endl;
}

void testSet()
{
  std::string input =
      "*3\r\n"
      "$3\r\n"
      "SET\r\n"
      "$1\r\n"
      "A\r\n"
      "$5\r\n"
      "Hello\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Complete);

  assert(command.size() == 3);

  assert(command[0] == "SET");
  assert(command[1] == "A");
  assert(command[2] == "Hello");

  assert(consumedBytes == input.size());

  std::cout << "PASS: parse SET" << std::endl;
}

void testSetWithTTL()
{
  std::string input =
      "*5\r\n"
      "$3\r\n"
      "SET\r\n"
      "$1\r\n"
      "A\r\n"
      "$1\r\n"
      "1\r\n"
      "$2\r\n"
      "EX\r\n"
      "$2\r\n"
      "10\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Complete);

  assert(command.size() == 5);

  assert(command[0] == "SET");
  assert(command[1] == "A");
  assert(command[2] == "1");
  assert(command[3] == "EX");
  assert(command[4] == "10");

  assert(consumedBytes == input.size());

  std::cout << "PASS: parse SET EX" << std::endl;
}

void testMultipleArguments()
{
  std::string input =
      "*4\r\n"
      "$3\r\n"
      "SET\r\n"
      "$5\r\n"
      "hello\r\n"
      "$5\r\n"
      "world\r\n"
      "$1\r\n"
      "1\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Complete);

  assert(command.size() == 4);

  assert(command[0] == "SET");
  assert(command[1] == "hello");
  assert(command[2] == "world");
  assert(command[3] == "1");

  std::cout << "PASS: multiple arguments" << std::endl;
}

void testIncompleteArray()
{
  std::string input =
      "*2\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Incomplete);

  std::cout << "PASS: incomplete array" << std::endl;
}

void testIncompleteBulkString()
{
  std::string input =
      "*1\r\n"
      "$4\r\n"
      "PIN";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Incomplete);

  std::cout << "PASS: incomplete bulk string" << std::endl;
}

void testInvalidRequest()
{
  std::string input =
      "PING\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Invalid);

  std::cout << "PASS: invalid request" << std::endl;
}

void testInvalidArraySize()
{
  std::string input =
      "*0\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Invalid);

  std::cout << "PASS: invalid array size" << std::endl;
}

void testInvalidBulkString()
{
  std::string input =
      "*1\r\n"
      ":4\r\n"
      "PING\r\n";

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Invalid);

  std::cout << "PASS: invalid bulk string" << std::endl;
}

void testPipelinedCommands()
{
  std::string firstCommand =
      "*1\r\n"
      "$4\r\n"
      "PING\r\n";

  std::string secondCommand =
      "*2\r\n"
      "$3\r\n"
      "GET\r\n"
      "$1\r\n"
      "A\r\n";

  std::string input =
      firstCommand +
      secondCommand;

  std::vector<std::string> command;
  std::size_t consumedBytes = 0;

  RESP::ParseResult result =
      RESP::parseCommand(
          input,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Complete);

  assert(command.size() == 1);
  assert(command[0] == "PING");

  assert(
      consumedBytes ==
      firstCommand.size());

  std::string remaining =
      input.substr(consumedBytes);

  command.clear();
  consumedBytes = 0;

  result =
      RESP::parseCommand(
          remaining,
          command,
          consumedBytes);

  assert(result == RESP::ParseResult::Complete);

  assert(command.size() == 2);
  assert(command[0] == "GET");
  assert(command[1] == "A");

  std::cout << "PASS: pipelined commands" << std::endl;
}

void testSimpleStringEncoding()
{
  std::string response =
      RESP::encodeSimpleString("OK");

  assert(response == "+OK\r\n");

  std::cout << "PASS: simple string encoding" << std::endl;
}

void testErrorEncoding()
{
  std::string response =
      RESP::encodeError("something went wrong");

  assert(
      response ==
      "-something went wrong\r\n");

  std::cout << "PASS: error encoding" << std::endl;
}

void testBulkStringEncoding()
{
  std::string response =
      RESP::encodeBulkString("Hello");

  assert(
      response ==
      "$5\r\n"
      "Hello\r\n");

  std::cout << "PASS: bulk string encoding" << std::endl;
}

void testNullEncoding()
{
  std::string response =
      RESP::encodeNull();

  assert(response == "$-1\r\n");

  std::cout << "PASS: null encoding" << std::endl;
}

void testIntegerEncoding()
{
  std::string response =
      RESP::encodeInteger(42);

  assert(response == ":42\r\n");

  std::cout << "PASS: integer encoding" << std::endl;
}

int main()
{
  testPing();
  testSet();
  testSetWithTTL();
  testMultipleArguments();

  testIncompleteArray();
  testIncompleteBulkString();

  testInvalidRequest();
  testInvalidArraySize();
  testInvalidBulkString();

  testPipelinedCommands();

  testSimpleStringEncoding();
  testErrorEncoding();
  testBulkStringEncoding();
  testNullEncoding();
  testIntegerEncoding();

  std::cout << std::endl;
  std::cout << "All RESP tests passed!" << std::endl;

  return 0;
}
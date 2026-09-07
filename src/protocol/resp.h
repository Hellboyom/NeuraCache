#pragma once

#include <string>
#include <vector>

namespace RESP
{

  enum class ParseResult
  {
    Complete,
    Incomplete,
    Error
  };

  ParseResult parseCommand(
      const std::string &input,
      std::vector<std::string> &command,
      size_t &consumed);

  std::string encodeSimpleString(
      const std::string &value);

  std::string encodeError(
      const std::string &value);

  std::string encodeBulkString(
      const std::string &value);

  std::string encodeNull();

  std::string encodeInteger(
      long long value);

}
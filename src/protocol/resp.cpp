#include "resp.h"

#include <cstddef>
#include <string>

namespace RESP
{

  namespace
  {

    // Find either CRLF or LF.
    //
    // Returns:
    //   position of the line ending
    //
    // lineEndingLength:
    //   2 for CRLF
    //   1 for LF
    //
    bool findLineEnd(
        const std::string &input,
        std::size_t start,
        std::size_t &lineEnd,
        std::size_t &lineEndingLength)
    {
      std::size_t crlf = input.find("\r\n", start);

      if (crlf != std::string::npos)
      {
        lineEnd = crlf;
        lineEndingLength = 2;
        return true;
      }

      std::size_t lf = input.find('\n', start);

      if (lf != std::string::npos)
      {
        lineEnd = lf;
        lineEndingLength = 1;
        return true;
      }

      return false;
    }

    bool parseInteger(
        const std::string &input,
        std::size_t start,
        std::size_t end,
        long long &value)
    {
      if (start >= end)
      {
        return false;
      }

      try
      {
        std::size_t charactersRead = 0;

        value = std::stoll(
            input.substr(
                start,
                end - start),
            &charactersRead);

        return charactersRead == end - start;
      }
      catch (...)
      {
        return false;
      }
    }

  }

  bool parseCommand(
      const std::string &input,
      std::vector<std::string> &command,
      std::size_t &consumedBytes)
  {
    command.clear();
    consumedBytes = 0;

    if (input.empty())
    {
      return false;
    }

    // RESP command must begin with an array.
    if (input[0] != '*')
    {
      return false;
    }

    std::size_t position = 1;

    // --------------------------------------------------
    // Read array length
    // --------------------------------------------------

    std::size_t lineEnd;
    std::size_t lineEndingLength;

    if (!findLineEnd(
            input,
            position,
            lineEnd,
            lineEndingLength))
    {
      // We don't have the complete line yet.
      return false;
    }

    long long argumentCount;

    if (!parseInteger(
            input,
            position,
            lineEnd,
            argumentCount))
    {
      return false;
    }

    if (argumentCount <= 0)
    {
      return false;
    }

    position = lineEnd + lineEndingLength;

    // --------------------------------------------------
    // Read each bulk string
    // --------------------------------------------------

    for (
        long long i = 0;
        i < argumentCount;
        ++i)
    {
      // Need at least '$'.
      if (position >= input.size())
      {
        return false;
      }

      if (input[position] != '$')
      {
        return false;
      }

      ++position;

      // ----------------------------------------------
      // Read bulk string length
      // ----------------------------------------------

      if (!findLineEnd(
              input,
              position,
              lineEnd,
              lineEndingLength))
      {
        return false;
      }

      long long valueLength;

      if (!parseInteger(
              input,
              position,
              lineEnd,
              valueLength))
      {
        return false;
      }

      if (valueLength < 0)
      {
        return false;
      }

      position = lineEnd + lineEndingLength;

      // ----------------------------------------------
      // Check whether complete value has arrived
      // ----------------------------------------------

      std::size_t length =
          static_cast<std::size_t>(valueLength);

      if (
          input.size() - position < length)
      {
        // TCP packet was incomplete.
        return false;
      }

      // ----------------------------------------------
      // Extract value
      // ----------------------------------------------

      command.push_back(
          input.substr(
              position,
              length));

      position += length;

      // ----------------------------------------------
      // Consume value's line ending
      // ----------------------------------------------

      if (
          position + 1 < input.size() &&
          input[position] == '\r' &&
          input[position + 1] == '\n')
      {
        position += 2;
      }
      else if (
          position < input.size() &&
          input[position] == '\n')
      {
        position += 1;
      }
      else
      {
        // The value arrived, but its terminating
        // newline has not arrived yet.
        return false;
      }
    }

    // Tell the server exactly how many bytes
    // belong to this command.
    consumedBytes = position;

    return !command.empty();
  }

  std::string encodeSimpleString(
      const std::string &value)
  {
    return "+" + value + "\r\n";
  }

  std::string encodeError(
      const std::string &value)
  {
    return "-" + value + "\r\n";
  }

  std::string encodeBulkString(
      const std::string &value)
  {
    return "$" +
           std::to_string(value.size()) +
           "\r\n" +
           value +
           "\r\n";
  }

  std::string encodeNull()
  {
    return "$-1\r\n";
  }

  std::string encodeInteger(
      long long value)
  {
    return ":" +
           std::to_string(value) +
           "\r\n";
  }

}
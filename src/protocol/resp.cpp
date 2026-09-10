#include "resp.h"

#include <cstddef>
#include <string>

namespace RESP
{

namespace
{

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
            input.substr(start, end - start),
            &charactersRead);

        return charactersRead == end - start;
    }
    catch (...)
    {
        return false;
    }
}

}

ParseResult parseCommand(
    const std::string &input,
    std::vector<std::string> &command,
    std::size_t &consumedBytes)
{
    command.clear();
    consumedBytes = 0;

    if (input.empty())
    {
        return ParseResult::Incomplete;
    }

    if (input[0] != '*')
    {
        return ParseResult::Invalid;
    }

    std::size_t position = 1;
    std::size_t lineEnd;
    std::size_t lineEndingLength;

    if (!findLineEnd(
            input,
            position,
            lineEnd,
            lineEndingLength))
    {
        return ParseResult::Incomplete;
    }

    long long argumentCount;

    if (!parseInteger(
            input,
            position,
            lineEnd,
            argumentCount))
    {
        return ParseResult::Invalid;
    }

    if (argumentCount <= 0 ||
        argumentCount > 1024)
    {
        return ParseResult::Invalid;
    }

    position = lineEnd + lineEndingLength;

    for (long long i = 0; i < argumentCount; ++i)
    {
        if (position >= input.size())
        {
            return ParseResult::Incomplete;
        }

        if (input[position] != '$')
        {
            return ParseResult::Invalid;
        }

        ++position;

        if (!findLineEnd(
                input,
                position,
                lineEnd,
                lineEndingLength))
        {
            return ParseResult::Incomplete;
        }

        long long valueLength;

        if (!parseInteger(
                input,
                position,
                lineEnd,
                valueLength))
        {
            return ParseResult::Invalid;
        }

        if (valueLength < 0)
        {
            return ParseResult::Invalid;
        }

        position = lineEnd + lineEndingLength;

        std::size_t length =
            static_cast<std::size_t>(valueLength);

        if (input.size() - position < length)
        {
            return ParseResult::Incomplete;
        }

        command.push_back(
            input.substr(position, length));

        position += length;

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
            return ParseResult::Incomplete;
        }
    }

    consumedBytes = position;

    return ParseResult::Complete;
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

#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace RESP
{

enum class ParseResult
{
    Complete,
    Incomplete,
    Invalid
};

ParseResult parseCommand(
    const std::string &input,
    std::vector<std::string> &command,
    std::size_t &consumedBytes);

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

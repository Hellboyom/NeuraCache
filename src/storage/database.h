#pragma once

#include <string>
#include <unordered_map>

class Database
{
private:
  std::unordered_map<std::string, std::string> data;

public:
  void set(const std::string &key, const std::string &value);

  bool get(
      const std::string &key,
      std::string &value) const;

  bool del(const std::string &key);

  bool exists(const std::string &key) const;

  size_t size() const;
};
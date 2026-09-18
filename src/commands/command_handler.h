#pragma once

#include "../storage/database.h"
#include "../metrics/metrics.h"

#include <string>
#include <vector>

class CommandHandler
{
private:
  Database &database;
  Metrics &metrics;

  std::string upper(
      const std::string &value) const;

  bool parseLongLong(
      const std::string &value,
      long long &result) const;

public:
  CommandHandler(
      Database &database,
      Metrics &metrics);

  std::string execute(
      const std::vector<std::string> &command);
};
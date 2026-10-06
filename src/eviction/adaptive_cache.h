#pragma once

#include "../ai/predictor.h"
#include "lru_cache.h"

#include <cstddef>
#include <string>
#include <vector>

class AdaptiveCache
{
public:
  AdaptiveCache(
      std::size_t capacity,
      Predictor &predictor);

  void touch(
      const std::string &key);

  std::string insert(
      const std::string &key);

  void remove(
      const std::string &key);

  void clear();

  std::size_t size() const;
  bool contains(
      const std::string &key) const;

  std::size_t capacity() const;

  std::vector<std::string> setCapacity(
      std::size_t capacity);

private:
  LRUCache lru;
  Predictor &predictor;

  std::string chooseVictim() const;
};
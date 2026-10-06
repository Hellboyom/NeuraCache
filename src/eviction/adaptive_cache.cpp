#include "adaptive_cache.h"

#include <limits>

AdaptiveCache::AdaptiveCache(
    std::size_t capacity,
    Predictor &predictor)
    : lru(capacity),
      predictor(predictor)
{
}

void AdaptiveCache::touch(
    const std::string &key)
{
  lru.touch(key);
}
bool AdaptiveCache::contains(
    const std::string &key) const
{
  return lru.contains(key);
}

std::string AdaptiveCache::insert(
    const std::string &key)
{
  if (lru.contains(key))
  {
    lru.touch(key);
    return "";
  }

  if (lru.size() < lru.getCapacity())
  {
    lru.insert(key);
    return "";
  }

  std::string victim =
      chooseVictim();

  if (!victim.empty())
  {
    lru.remove(victim);
  }

  lru.insert(key);

  return victim;
}

void AdaptiveCache::remove(
    const std::string &key)
{
  lru.remove(key);
}

void AdaptiveCache::clear()
{
  lru.clear();
}

std::size_t AdaptiveCache::size() const
{
  return lru.size();
}

std::size_t AdaptiveCache::capacity() const
{
  return lru.getCapacity();
}

std::vector<std::string>
AdaptiveCache::setCapacity(
    std::size_t capacity)
{
  std::vector<std::string> evicted;

  while (lru.size() > capacity)
  {
    std::string victim =
        chooseVictim();

    if (victim.empty())
    {
      break;
    }

    lru.remove(victim);
    evicted.push_back(victim);
  }

  return evicted;
}

std::string AdaptiveCache::chooseVictim() const
{
  std::vector<std::string> keys =
      lru.keys();

  if (keys.empty())
  {
    return "";
  }

  std::string victim;
  double worstScore =
      std::numeric_limits<double>::max();

  for (const auto &key : keys)
  {
    Predictor::Prediction prediction =
        predictor.predict(key);

    double score =
        prediction.score;

    if (score < worstScore)
    {
      worstScore = score;
      victim = key;
    }
  }

  return victim;
}
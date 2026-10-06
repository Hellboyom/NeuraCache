#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class Predictor
{
public:
  struct Prediction
  {
    std::string key;
    double score;
    std::size_t estimatedAccesses;
  };
  struct AccessRecord
  {
    std::string key;
    std::size_t totalAccesses;
    std::size_t recentAccesses;
  };

  Predictor();

  void recordAccess(
      const std::string &key);

  Prediction predict(
      const std::string &key) const;

  std::vector<Prediction> topPredictions(
      std::size_t limit) const;

  std::size_t totalAccesses() const;

  std::size_t trackedKeys() const;
  std::vector<AccessRecord> accessRecords() const;

  void clear();

private:
  struct AccessPattern
  {
    std::size_t accesses;
    std::size_t recentAccesses;
  };

  std::unordered_map<
      std::string,
      AccessPattern>
      patterns;

  std::size_t totalAccessCount;
};
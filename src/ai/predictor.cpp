#include "predictor.h"

#include <algorithm>
#include <cmath>

Predictor::Predictor()
    : totalAccessCount(0)
{
}

void Predictor::recordAccess(
    const std::string &key)
{
    for (auto &pair : patterns)
    {
        pair.second.recentAccesses =
            pair.second.recentAccesses * 9 / 10;
    }

    AccessPattern &pattern =
        patterns[key];

    pattern.accesses++;

    pattern.recentAccesses += 10;

    totalAccessCount++;
}

Predictor::Prediction Predictor::predict(
    const std::string &key) const
{
    auto iterator =
        patterns.find(key);

    if (iterator == patterns.end())
    {
        return {
            key,
            0.0,
            0};
    }

    const AccessPattern &pattern =
        iterator->second;

    double frequencyScore = 0.0;

    if (totalAccessCount > 0)
    {
        frequencyScore =
            static_cast<double>(
                pattern.accesses) /
            static_cast<double>(
                totalAccessCount);
    }

    double totalRecentAccesses = 0.0;

    for (const auto &pair : patterns)
    {
        totalRecentAccesses +=
            static_cast<double>(
                pair.second.recentAccesses);
    }

    double recencyScore = 0.0;

    if (totalRecentAccesses > 0.0)
    {
        recencyScore =
            static_cast<double>(
                pattern.recentAccesses) /
            totalRecentAccesses;
    }

    double score =
        (frequencyScore * 0.6) +
        (recencyScore * 0.4);

    return {
        key,
        score,
        pattern.accesses};
}
std::vector<Predictor::Prediction>
Predictor::topPredictions(
    std::size_t limit) const
{
    std::vector<Prediction> predictions;

    for (const auto &pair : patterns)
    {
        predictions.push_back(
            predict(pair.first));
    }

    std::sort(
        predictions.begin(),
        predictions.end(),
        [](const Prediction &a,
           const Prediction &b)
        {
            return a.score > b.score;
        });

    if (predictions.size() > limit)
    {
        predictions.resize(limit);
    }

    return predictions;
}

std::size_t Predictor::totalAccesses() const
{
    return totalAccessCount;
}
std::size_t Predictor::trackedKeys() const
{
    return patterns.size();
}
std::vector<Predictor::AccessRecord>
Predictor::accessRecords() const
{
    std::vector<AccessRecord> records;

    for (const auto &pair : patterns)
    {
        records.push_back({pair.first,
                           pair.second.accesses,
                           pair.second.recentAccesses});
    }

    return records;
}

void Predictor::clear()
{
    patterns.clear();

    totalAccessCount = 0;
}
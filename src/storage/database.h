#pragma once

#include <chrono>
#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

class Database
{
public:

    Database();

    void set(
        const std::string &key,
        const std::string &value);

    void set(
        const std::string &key,
        const std::string &value,
        long long ttlSeconds);

    bool get(
        const std::string &key,
        std::string &value);

    bool del(
        const std::string &key);

    bool exists(
        const std::string &key);

    bool expire(
        const std::string &key,
        long long ttlSeconds);

    long long ttl(
        const std::string &key);

    std::size_t size() const;

    void clear();

private:

    struct Entry
    {
        std::string value;

        std::optional<
            std::chrono::steady_clock::time_point>
            expiresAt;
    };

    bool isExpired(
        const Entry &entry) const;

    void removeExpired(
        const std::string &key);

    std::unordered_map<
        std::string,
        Entry>
        data;

    mutable std::mutex mutex;
};

#include "database.h"

Database::Database()
{
}

bool Database::isExpired(
    const Entry &entry) const
{
    if (!entry.expiresAt.has_value())
    {
        return false;
    }

    return std::chrono::steady_clock::now() >=
           entry.expiresAt.value();
}

void Database::removeExpired(
    const std::string &key)
{
    auto iterator = data.find(key);

    if (iterator != data.end() &&
        isExpired(iterator->second))
    {
        data.erase(iterator);
    }
}

void Database::set(
    const std::string &key,
    const std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex);

    Entry entry;

    entry.value = value;
    entry.expiresAt.reset();

    data[key] = std::move(entry);
}

void Database::set(
    const std::string &key,
    const std::string &value,
    long long ttlSeconds)
{
    std::lock_guard<std::mutex> lock(mutex);

    Entry entry;

    entry.value = value;

    entry.expiresAt =
        std::chrono::steady_clock::now() +
        std::chrono::seconds(ttlSeconds);

    data[key] = std::move(entry);
}

bool Database::get(
    const std::string &key,
    std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    auto iterator = data.find(key);

    if (iterator == data.end())
    {
        return false;
    }

    value = iterator->second.value;

    return true;
}

bool Database::del(
    const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    return data.erase(key) > 0;
}

bool Database::exists(
    const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    return data.find(key) != data.end();
}

bool Database::expire(
    const std::string &key,
    long long ttlSeconds)
{
    if (ttlSeconds <= 0)
    {
        return del(key);
    }

    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    auto iterator = data.find(key);

    if (iterator == data.end())
    {
        return false;
    }

    iterator->second.expiresAt =
        std::chrono::steady_clock::now() +
        std::chrono::seconds(ttlSeconds);

    return true;
}

long long Database::ttl(
    const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    auto iterator = data.find(key);

    if (iterator == data.end())
    {
        return -2;
    }

    if (!iterator->second.expiresAt.has_value())
    {
        return -1;
    }

    auto remaining =
        std::chrono::duration_cast<
            std::chrono::seconds>(
            iterator->second.expiresAt.value() -
            std::chrono::steady_clock::now());

    if (remaining.count() < 0)
    {
        data.erase(iterator);

        return -2;
    }

    return remaining.count();
}

std::size_t Database::size() const
{
    std::lock_guard<std::mutex> lock(mutex);

    return data.size();
}

void Database::clear()
{
    std::lock_guard<std::mutex> lock(mutex);

    data.clear();
}

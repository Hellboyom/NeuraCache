#include "database.h"

Database::Database()
    : lru(3)
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

        lru.remove(key);
    }
}

void Database::removeEvictedKeys(
    const std::optional<std::string> &evictedKey)
{
    if (!evictedKey.has_value())
    {
        return;
    }

    data.erase(evictedKey.value());
}

void Database::set(
    const std::string &key,
    const std::string &value)
{

    std::lock_guard<std::mutex> lock(mutex);

    Entry entry;

    entry.value = value;
    entry.expiresAt.reset();

    bool keyAlreadyExists =
        data.find(key) != data.end();

    data[key] = std::move(entry);

    if (keyAlreadyExists)
    {
        lru.touch(key);
        return;
    }

    std::optional<std::string> evictedKey =
        lru.insert(key);

    removeEvictedKeys(evictedKey);
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

    bool keyAlreadyExists =
        data.find(key) != data.end();

    data[key] = std::move(entry);

    if (keyAlreadyExists)
    {
        lru.touch(key);
        return;
    }

    std::optional<std::string> evictedKey =
        lru.insert(key);

    removeEvictedKeys(evictedKey);
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

    lru.touch(key);

    return true;
}

bool Database::del(
    const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    auto iterator = data.find(key);

    if (iterator == data.end())
    {
        return false;
    }

    data.erase(iterator);

    lru.remove(key);

    return true;
}

bool Database::exists(
    const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex);

    removeExpired(key);

    auto iterator = data.find(key);

    if (iterator == data.end())
    {
        return false;
    }

    lru.touch(key);

    return true;
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

    lru.touch(key);

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
        lru.touch(key);

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

        lru.remove(key);

        return -2;
    }

    lru.touch(key);

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

    lru.clear();
}

void Database::setCapacity(
    std::size_t newCapacity)
{
    std::lock_guard<std::mutex> lock(mutex);

    std::vector<std::string> evictedKeys =
        lru.setCapacity(newCapacity);

    for (const std::string &key : evictedKeys)
    {
        data.erase(key);
    }
}

std::size_t Database::capacity() const
{
    std::lock_guard<std::mutex> lock(mutex);

    return lru.getCapacity();
}
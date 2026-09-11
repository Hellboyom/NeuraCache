#include "lru_cache.h"

LRUCache::LRUCache(
    std::size_t capacity)
    : capacity(capacity)
{
}

void LRUCache::touch(
    const std::string &key)
{
    auto iterator = positions.find(key);

    if (iterator == positions.end())
    {
        return;
    }

    order.erase(iterator->second);

    order.push_front(key);

    iterator->second = order.begin();
}

std::optional<std::string> LRUCache::insert(
    const std::string &key)
{
    auto iterator = positions.find(key);

    if (iterator != positions.end())
    {
        touch(key);

        return std::nullopt;
    }

    order.push_front(key);

    positions[key] = order.begin();

    if (order.size() <= capacity)
    {
        return std::nullopt;
    }

    std::string evictedKey = order.back();

    order.pop_back();

    positions.erase(evictedKey);

    return evictedKey;
}

void LRUCache::remove(
    const std::string &key)
{
    auto iterator = positions.find(key);

    if (iterator == positions.end())
    {
        return;
    }

    order.erase(iterator->second);

    positions.erase(iterator);
}

void LRUCache::clear()
{
    order.clear();

    positions.clear();
}

std::size_t LRUCache::size() const
{
    return order.size();
}

std::size_t LRUCache::getCapacity() const
{
    return capacity;
}

std::vector<std::string> LRUCache::setCapacity(
    std::size_t newCapacity)
{
    capacity = newCapacity;

    std::vector<std::string> evictedKeys;

    while (order.size() > capacity)
    {
        std::string evictedKey = order.back();

        order.pop_back();

        positions.erase(evictedKey);

        evictedKeys.push_back(evictedKey);
    }

    return evictedKeys;
}
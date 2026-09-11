#pragma once

#include <cstddef>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class LRUCache
{
public:
    explicit LRUCache(
        std::size_t capacity);

    void touch(
        const std::string &key);

    std::optional<std::string> insert(
        const std::string &key);

    void remove(
        const std::string &key);

    void clear();

    std::size_t size() const;

    std::size_t getCapacity() const;

    std::vector<std::string> setCapacity(
        std::size_t capacity);

private:
    std::size_t capacity;

    std::list<std::string> order;

    std::unordered_map<
        std::string,
        std::list<std::string>::iterator>
        positions;
};
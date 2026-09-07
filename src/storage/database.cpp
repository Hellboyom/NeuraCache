#include "database.h"

void Database::set(
    const std::string &key,
    const std::string &value)
{
    data[key] = value;
}

bool Database::get(
    const std::string &key,
    std::string &value) const
{

    auto it = data.find(key);

    if (it == data.end())
    {
        return false;
    }

    value = it->second;

    return true;
}

bool Database::del(const std::string &key)
{

    return data.erase(key) > 0;
}

bool Database::exists(const std::string &key) const
{

    return data.find(key) != data.end();
}

size_t Database::size() const
{

    return data.size();
}
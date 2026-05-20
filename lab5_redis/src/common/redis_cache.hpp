#pragma once

#include <optional>
#include <string>
#include <chrono>

#include <userver/storages/redis/client.hpp>

namespace delivery_service {

class RedisCache {
public:
    explicit RedisCache(userver::storages::redis::ClientPtr redis);

    std::optional<std::string> Get(const std::string& key) const;

    void Set(const std::string& key,
             const std::string& value,
             std::chrono::seconds ttl) const;

    void Delete(const std::string& key) const;

    bool Exists(const std::string& key) const;

    int64_t Increment(const std::string& key) const;

    void Expire(const std::string& key,
                std::chrono::seconds ttl) const;

private:
    userver::storages::redis::ClientPtr redis_;
};

}
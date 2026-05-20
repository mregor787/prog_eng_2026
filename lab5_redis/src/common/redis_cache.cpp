#include "redis_cache.hpp"  

namespace delivery_service {

RedisCache::RedisCache(userver::storages::redis::ClientPtr redis)
    : redis_(std::move(redis)) {}

std::optional<std::string> RedisCache::Get(const std::string& key) const {
    auto result = redis_->Get(key, {}).Get();
    if (!result.has_value()) {
        return std::nullopt;
    }
    return result.value();
}

void RedisCache::Set(const std::string& key, const std::string& value, std::chrono::seconds ttl) const {
    redis_->Set(key, value, ttl, {}).Get();
}

void RedisCache::Delete(const std::string& key) const {
    redis_->Del(key, {}).Get();
}

bool RedisCache::Exists(const std::string& key) const {
    return redis_->Exists(key, {}).Get() > 0;
}

int64_t RedisCache::Increment(const std::string& key) const {
    return redis_->Incr(key, {}).Get();
}

void RedisCache::Expire(const std::string& key,
                        std::chrono::seconds ttl) const {
    redis_->Expire(key, ttl, {}).Get();
}

}
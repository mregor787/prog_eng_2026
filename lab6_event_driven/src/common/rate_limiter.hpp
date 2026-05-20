#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

#include <userver/storages/redis/client.hpp>

namespace delivery_service {

struct RateLimitResult {
    bool allowed;
    int64_t limit;
    int64_t remaining;
    int64_t reset_after_seconds;
};

class RateLimiter {
public:
    explicit RateLimiter(userver::storages::redis::ClientPtr redis);

    RateLimitResult CheckLimit(const std::string& key, int64_t limit,
                               std::chrono::seconds window) const;

private:
    userver::storages::redis::ClientPtr redis_;
};

}
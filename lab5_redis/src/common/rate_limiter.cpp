#include "rate_limiter.hpp"

namespace delivery_service {

RateLimiter::RateLimiter(userver::storages::redis::ClientPtr redis)
    : redis_(std::move(redis)) {}

RateLimitResult RateLimiter::CheckLimit(const std::string& key, int64_t limit,
                                        std::chrono::seconds window) const {
    const int64_t now_sec = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    const int64_t window_sec = window.count();
    const int64_t window_ts = now_sec / window_sec;

    const std::string redis_key = "rate_limit:" + key + ":" + std::to_string(window_ts);
    const int64_t count = redis_->Incr(redis_key, {}).Get();

    if (count == 1) {
        redis_->Expire(redis_key, window, {}).Get();
    }

    const bool allowed = count <= limit;
    const int64_t remaining = std::max<int64_t>(0, limit - count);
    const int64_t reset_after = window_sec - (now_sec % window_sec);

    return RateLimitResult{
        .allowed = allowed,
        .limit = limit,
        .remaining = remaining,
        .reset_after_seconds = reset_after
    };
}

}
#include "login_user_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/formats/json.hpp>
#include <jwt-cpp/jwt.h>

namespace delivery_service::user {

std::string GetClientIp(const userver::server::http::HttpRequest& request) {
    auto ip = request.GetHeader("X-Real-IP");
    if (!ip.empty()) 
        return ip;
    ip = request.GetHeader("X-Forwarded-For");
    if (!ip.empty()) {
        const auto comma = ip.find(',');
        return (comma != std::string::npos) ? ip.substr(0, comma) : ip;
    }
    return "unknown";
}
    
bool ApplyRateLimit(const userver::server::http::HttpRequest& request, RateLimiter& limiter, 
                    const std::string& key, int64_t limit, std::chrono::seconds window) {
    auto rl = limiter.CheckLimit(key, limit, window);
    request.GetHttpResponse().SetHeader(
        std::string{"X-RateLimit-Limit"},
        std::to_string(rl.limit)
    );
    request.GetHttpResponse().SetHeader(
        std::string{"X-RateLimit-Remaining"},
        std::to_string(rl.remaining)
    );
    request.GetHttpResponse().SetHeader(
        std::string{"X-RateLimit-Reset"},
        std::to_string(rl.reset_after_seconds)
    );
    return !rl.allowed;
}

LoginHandler::LoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& mongo_component = context.FindComponent<userver::components::Mongo>("mongo-delivery-db");
    db_ = std::make_shared<MongoDatabase>(mongo_component.GetPool());
    auto& redis_component = context.FindComponent<userver::components::Redis>("redis-delivery");
    rate_limiter_ = std::make_shared<RateLimiter>(redis_component.GetClient("delivery-db"));
}

std::string LoginHandler::GenerateToken(int64_t user_id) const {
    auto token = ::jwt::create<::jwt::traits::kazuho_picojson>()
        .set_issuer("delivery-service")
        .set_type("JWT")
        .set_payload_claim("user_id", ::jwt::claim(std::to_string(user_id)))
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
        .sign(::jwt::algorithm::hs256{"jwt-secret-key"});
    
    return token;
}

std::string LoginHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    auto client_ip = GetClientIp(request);
    if (ApplyRateLimit(request, *rate_limiter_, "login:" + client_ip, 10, std::chrono::minutes(1))) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kTooManyRequests);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject(
                "error",
                "Too many login attempts"
            )
        );
    }

    auto request_body = userver::formats::json::FromString(request.RequestBody());
    
    if (!request_body.HasMember("login")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Login is required")
        );
    }
    if (!request_body.HasMember("password")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Password is required")
        );
    }
    
    auto login_request = LoginRequest::FromJson(request_body);

    if (login_request.login.empty() || login_request.password.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing required fields")
        );
    }

    auto user = db_->AuthenticateUser(login_request.login, login_request.password);
    if (!user) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Invalid credentials")
        );
    }

    AuthToken token;
    token.token = GenerateToken(*user->id);
    token.user_id = *user->id;
    token.expires_at = std::chrono::system_clock::now() + std::chrono::hours(24);

    return userver::formats::json::ToString(token.ToJson());
}

}
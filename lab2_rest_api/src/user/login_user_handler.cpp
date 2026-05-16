#include "login_user_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <jwt-cpp/jwt.h>

namespace delivery_service::user {

LoginHandler::LoginHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/delivery.db");
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
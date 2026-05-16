#include "create_user_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::user {

CreateUserHandler::CreateUserHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/delivery.db");
}

std::string CreateUserHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    auto request_body = userver::formats::json::FromString(request.RequestBody());
    
    if (!request_body.HasMember("login") || !request_body.HasMember("email") ||
        !request_body.HasMember("first_name") || !request_body.HasMember("last_name") ||
        !request_body.HasMember("password")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing required fields")
        );
    }
    
    auto create_request = CreateUserRequest::FromJson(request_body);

    if (create_request.login.empty() || create_request.email.empty() ||
        create_request.first_name.empty() || create_request.last_name.empty() ||
        create_request.password.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing required fields")
        );
    }
    
    if (create_request.email.find('@') == std::string::npos) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Invalid email format")
        );
    }

    auto user = db_->CreateUser(create_request);
    if (!user) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "User already exists")
        );
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(user->ToJson());
}

}
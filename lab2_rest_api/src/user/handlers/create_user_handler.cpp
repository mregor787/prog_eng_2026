#include "user/handlers/create_user_handler.hpp"
#include <userver/formats/json.hpp>
#include <userver/components/component_context.hpp>

CreateUserHandler::CreateUserHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<UserStorage>()) {}

std::string CreateUserHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto json = userver::formats::json::FromString(request.RequestBody());

    User user;
    user.id = std::to_string(rand());
    user.login = json["login"].As<std::string>();
    user.first_name = json["first_name"].As<std::string>();
    user.last_name = json["last_name"].As<std::string>();

    if (!storage_.CreateUser(user)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return R"({"error":"User already exists"})";
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);

    userver::formats::json::ValueBuilder res;
    res["id"] = user.id;
    res["login"] = user.login;
    res["first_name"] = user.first_name;
    res["last_name"] = user.last_name;

    return userver::formats::json::ToString(res.ExtractValue());
}
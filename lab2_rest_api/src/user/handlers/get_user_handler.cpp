#include "user/handlers/get_user_handler.hpp"
#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>

using namespace userver;

GetUserHandler::GetUserHandler(
    const components::ComponentConfig& config,
    const components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<UserStorage>()) {}

std::string GetUserHandler::HandleRequestThrow(
    const server::http::HttpRequest& request,
    server::request::RequestContext&) const {

    const auto& login = request.GetPathArg("login");

    auto user = storage_.GetUserByLogin(login);

    if (!user) {
        request.SetResponseStatus(server::http::HttpStatus::kNotFound);
        return R"({"error":"User not found"})";
    }

    formats::json::ValueBuilder response;
    response["id"] = user->id;
    response["login"] = user->login;
    response["first_name"] = user->first_name;
    response["last_name"] = user->last_name;

    return formats::json::ToString(response.ExtractValue());
}
#include "user/handlers/search_users_handler.hpp"
#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>

SearchUsersHandler::SearchUsersHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<UserStorage>()) {}

std::string SearchUsersHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    const auto& mask = request.GetArg("name");

    if (mask.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error":"Query parameter 'name' is required"})";
    }

    auto users = storage_.FindUsersByNameMask(mask);

    userver::formats::json::ValueBuilder response(userver::formats::json::Type::kArray);

    for (const auto& user : users) {
        userver::formats::json::ValueBuilder u;
        u["id"] = user.id;
        u["login"] = user.login;
        u["first_name"] = user.first_name;
        u["last_name"] = user.last_name;

        response.PushBack(u.ExtractValue());
    }

    return userver::formats::json::ToString(response.ExtractValue());
}
#include "get_user_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::user {

GetUserHandler::GetUserHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/delivery.db");
}

std::string GetUserHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    auto login = request.GetArg("login");
    if (login.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing 'login' parameter")
        );
    }

    auto user = db_->GetUserByLogin(login);
    if (!user) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "User not found")
        );
    }

    return userver::formats::json::ToString(user->ToJson());
}

}
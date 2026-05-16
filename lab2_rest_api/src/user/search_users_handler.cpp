#include "search_users_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::user {

SearchUsersHandler::SearchUsersHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/delivery.db");
}

std::string SearchUsersHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {
    
    auto query = request.GetArg("query");
    if (query.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Missing 'query' parameter")
        );
    }

    auto users = db_->SearchUsersByQuery(query);
    
    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& user : users) {
        builder.PushBack(user.ToJson());
    }

    return userver::formats::json::ToString(builder.ExtractValue());
}

}
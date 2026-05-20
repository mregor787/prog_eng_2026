#include "get_user_parcels_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::parcel {

int64_t ExtractUserIdFromContext(
    const userver::server::request::RequestContext& context
);

GetUserParcelsHandler::GetUserParcelsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-delivery-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
    auto& redis_component = context.FindComponent<userver::components::Redis>("redis-delivery");
    redis_ = std::make_shared<RedisCache>(redis_component.GetClient("delivery-db"));
}

std::string GetUserParcelsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto user_id = ExtractUserIdFromContext(context);

    auto cache_key = "parcels:user:" + std::to_string(user_id);
    auto cached = redis_->Get(cache_key);
    if (cached) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
        return *cached;
    }

    auto parcels = db_->GetUserParcels(user_id);
    userver::formats::json::ValueBuilder builder(
        userver::formats::json::Type::kArray
    );
    for (const auto& parcel : parcels) {
        builder.PushBack(parcel.ToJson());
    }
    auto response = userver::formats::json::ToString(builder.ExtractValue());

    redis_->Set(cache_key, response, std::chrono::seconds{300});
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);

    return response;
}

}
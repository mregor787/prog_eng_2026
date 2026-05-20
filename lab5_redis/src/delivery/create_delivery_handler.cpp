#include "create_delivery_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::delivery {

int64_t ExtractUserIdFromContext(const userver::server::request::RequestContext& context) {
    return context.GetData<int64_t>("user_id");
}

CreateDeliveryHandler::CreateDeliveryHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-delivery-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
    auto& redis_component = context.FindComponent<userver::components::Redis>("redis-delivery");
    redis_ = std::make_shared<RedisCache>(redis_component.GetClient("delivery-db"));
}

std::string CreateDeliveryHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto sender_id = ExtractUserIdFromContext(context);

    auto request_body = userver::formats::json::FromString(request.RequestBody());

    if (!request_body.HasMember("receiver_id") || !request_body.HasMember("parcel_id")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "receiver_id and parcel_id are required")
        );
    }

    auto create_request = CreateDeliveryRequest::FromJson(request_body);

    auto delivery = db_->CreateDelivery(sender_id, create_request);
    if (!delivery) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Failed to create a delivery")
        );
    }

    redis_->Delete("deliveries:sender:" + std::to_string(sender_id));
    redis_->Delete("deliveries:receiver:" + std::to_string(create_request.receiver_id));

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(delivery->ToJson());
}

}
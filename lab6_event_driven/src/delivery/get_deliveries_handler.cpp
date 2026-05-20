#include "get_deliveries_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/redis/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace delivery_service::delivery {

GetDeliveriesHandler::GetDeliveriesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-delivery-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
    auto& redis_component = context.FindComponent<userver::components::Redis>("redis-delivery");
    redis_ = std::make_shared<RedisCache>(redis_component.GetClient("delivery-db"));
}

std::string GetDeliveriesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto sender_id_arg = request.GetArg("sender_id");
    auto receiver_id_arg = request.GetArg("receiver_id");
    if ((sender_id_arg.empty() && receiver_id_arg.empty()) ||
        (!sender_id_arg.empty() && !receiver_id_arg.empty())) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject(
                "error",
                "Specify exactly one query parameter: sender_id or receiver_id"
            )
        );
    }

    std::vector<Delivery> deliveries;
    std::string cache_key;
    try {
        if (!sender_id_arg.empty()) {
            auto sender_id = std::stoll(sender_id_arg);
            cache_key = "deliveries:sender:" + std::to_string(sender_id);
            auto cached = redis_->Get(cache_key);
            if (cached) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
                return *cached;
            }
            deliveries = db_->GetDeliveriesBySender(sender_id);
        } else {
            auto receiver_id = std::stoll(receiver_id_arg);
            cache_key = "deliveries:receiver:" + std::to_string(receiver_id);
            auto cached = redis_->Get(cache_key);
            if (cached) {
                request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
                return *cached;
            }
            deliveries = db_->GetDeliveriesByReceiver(receiver_id);
        }
    } catch (const std::exception&) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject(
                "error",
                "sender_id and receiver_id must be integers"
            )
        );
    }

    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& delivery : deliveries) {
        builder.PushBack(delivery.ToJson());
    }
    auto response = userver::formats::json::ToString(builder.ExtractValue());
    redis_->Set(cache_key, response, std::chrono::seconds{300});
    request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
    return response;
}

}
#include "get_deliveries_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace delivery_service::delivery {

GetDeliveriesHandler::GetDeliveriesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    db_ = std::make_shared<Database>("/app/data/delivery.db");
}

std::string GetDeliveriesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto sender_id_arg = request.GetArg("sender_id");
    auto receiver_id_arg = request.GetArg("receiver_id");

    if ((sender_id_arg.empty() && receiver_id_arg.empty()) || (!sender_id_arg.empty() && !receiver_id_arg.empty())) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject(
                "error",
                "Specify exactly one query parameter: sender_id or receiver_id"
            )
        );
    }

    std::vector<Delivery> deliveries;

    try {
        if (!sender_id_arg.empty()) {
            auto sender_id = std::stoll(sender_id_arg);
            deliveries = db_->GetDeliveriesBySender(sender_id);
        } else {
            auto receiver_id = std::stoll(receiver_id_arg);
            deliveries = db_->GetDeliveriesByReceiver(receiver_id);
        }
    } catch (const std::exception&) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "sender_id and receiver_id must be integers")
        );
    }

    userver::formats::json::ValueBuilder builder(userver::formats::json::Type::kArray);
    for (const auto& delivery : deliveries) {
        builder.PushBack(delivery.ToJson());
    }
    return userver::formats::json::ToString(builder.ExtractValue());
}

}
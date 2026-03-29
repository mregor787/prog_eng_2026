#include "delivery/handlers/get_deliveries_handler.hpp"
#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>

GetDeliveriesHandler::GetDeliveriesHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<DeliveryStorage>()) {}

std::string GetDeliveriesHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    const auto& sender_id = request.GetArg("sender_id");
    const auto& receiver_id = request.GetArg("receiver_id");

    std::vector<Delivery> deliveries;

    if (!sender_id.empty()) {
        deliveries = storage_.GetBySender(sender_id);
    } else if (!receiver_id.empty()) {
        deliveries = storage_.GetByReceiver(receiver_id);
    } else {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return R"({"error":"sender_id or receiver_id required"})";
    }

    userver::formats::json::ValueBuilder response(userver::formats::json::Type::kArray);

    for (const auto& d : deliveries) {
        userver::formats::json::ValueBuilder item;
        item["id"] = d.id;
        item["sender_id"] = d.sender_id;
        item["receiver_id"] = d.receiver_id;
        item["parcel_id"] = d.parcel_id;
        item["status"] = d.status;

        response.PushBack(item.ExtractValue());
    }

    return userver::formats::json::ToString(response.ExtractValue());
}
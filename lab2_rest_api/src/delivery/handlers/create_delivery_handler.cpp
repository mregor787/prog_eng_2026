#include "delivery/handlers/create_delivery_handler.hpp"
#include <userver/formats/json.hpp>
#include <userver/components/component_context.hpp>

CreateDeliveryHandler::CreateDeliveryHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<DeliveryStorage>()),
      parcel_storage_(context.FindComponent<ParcelStorage>()) {}

std::string CreateDeliveryHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto json = userver::formats::json::FromString(request.RequestBody());

    const auto parcel_id = json["parcel_id"].As<std::string>();
    const auto receiver_id = json["receiver_id"].As<std::string>();

    auto parcel = parcel_storage_.GetParcelById(parcel_id);
    if (!parcel) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kNotFound);
        return R"({"error":"Parcel not found"})";
    }

    Delivery delivery;
    delivery.id = std::to_string(rand());
    delivery.parcel_id = parcel_id;
    delivery.receiver_id = receiver_id;
    delivery.sender_id = parcel->sender_id;
    delivery.status = "created";

    if (!storage_.CreateDelivery(delivery)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return R"({"error":"Delivery already exists"})";
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);

    userver::formats::json::ValueBuilder res;
    res["id"] = delivery.id;
    res["sender_id"] = delivery.sender_id;
    res["receiver_id"] = delivery.receiver_id;
    res["parcel_id"] = delivery.parcel_id;
    res["status"] = delivery.status;

    return userver::formats::json::ToString(res.ExtractValue());
}
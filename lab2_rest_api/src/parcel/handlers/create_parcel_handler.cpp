#include "parcel/handlers/create_parcel_handler.hpp"
#include <userver/formats/json.hpp>
#include <userver/components/component_context.hpp>

CreateParcelHandler::CreateParcelHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<ParcelStorage>()) {}

std::string CreateParcelHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    auto json = userver::formats::json::FromString(request.RequestBody());

    Parcel parcel;
    parcel.id = std::to_string(rand());
    parcel.sender_id = json["sender_id"].As<std::string>();
    parcel.description = json["description"].As<std::string>();

    if (!storage_.CreateParcel(parcel)) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return R"({"error":"Parcel already exists"})";
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);

    userver::formats::json::ValueBuilder res;
    res["id"] = parcel.id;
    res["sender_id"] = parcel.sender_id;
    res["description"] = parcel.description;

    return userver::formats::json::ToString(res.ExtractValue());
}
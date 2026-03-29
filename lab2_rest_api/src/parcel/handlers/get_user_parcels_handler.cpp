#include "parcel/handlers/get_user_parcels_handler.hpp"
#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>

GetUserParcelsHandler::GetUserParcelsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context),
      storage_(context.FindComponent<ParcelStorage>()) {}

std::string GetUserParcelsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext&) const {

    const auto& user_id = request.GetPathArg("user_id");

    auto parcels = storage_.GetParcelsByUser(user_id);

    userver::formats::json::ValueBuilder response(userver::formats::json::Type::kArray);

    for (const auto& parcel : parcels) {
        userver::formats::json::ValueBuilder p;
        p["id"] = parcel.id;
        p["sender_id"] = parcel.sender_id;
        p["description"] = parcel.description;

        response.PushBack(p.ExtractValue());
    }

    return userver::formats::json::ToString(response.ExtractValue());
}
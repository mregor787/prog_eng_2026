#include "get_user_parcels_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::parcel {

int64_t ExtractUserIdFromContext(
    const userver::server::request::RequestContext& context
);

GetUserParcelsHandler::GetUserParcelsHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {

    db_ = std::make_shared<Database>("/app/data/delivery.db");
}

std::string GetUserParcelsHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {

    auto user_id = ExtractUserIdFromContext(context);

    auto parcels = db_->GetUserParcels(user_id);

    userver::formats::json::ValueBuilder builder(
        userver::formats::json::Type::kArray
    );

    for (const auto& parcel : parcels) {
        builder.PushBack(parcel.ToJson());
    }

    request.SetResponseStatus(
        userver::server::http::HttpStatus::kOk
    );

    return userver::formats::json::ToString(
        builder.ExtractValue()
    );
}

}
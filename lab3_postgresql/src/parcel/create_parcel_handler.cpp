#include "create_parcel_handler.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/formats/json.hpp>

namespace delivery_service::parcel {

int64_t ExtractUserIdFromContext(const userver::server::request::RequestContext& context) {
    return context.GetData<int64_t>("user_id");
}

CreateParcelHandler::CreateParcelHandler(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context) {
    auto& pg_component = context.FindComponent<userver::components::Postgres>("postgres-delivery-db");
    db_ = std::make_shared<Database>(pg_component.GetCluster());
}

std::string CreateParcelHandler::HandleRequestThrow(
    const userver::server::http::HttpRequest& request,
    userver::server::request::RequestContext& context) const {
    
    auto user_id = ExtractUserIdFromContext(context);
    
    auto request_body = userver::formats::json::FromString(request.RequestBody());
    
    if (!request_body.HasMember("description") || !request_body.HasMember("weight")) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Description and weight are required")
        );
    }
    
    auto create_request = CreateParcelRequest::FromJson(request_body);

    auto parcel = db_->CreateParcel(user_id, create_request);
    if (!parcel) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kConflict);
        return userver::formats::json::ToString(
            userver::formats::json::MakeObject("error", "Failed to create a parcel")
        );
    }

    request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
    return userver::formats::json::ToString(parcel->ToJson());
}

}
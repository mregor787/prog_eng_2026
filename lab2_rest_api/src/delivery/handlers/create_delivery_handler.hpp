#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include "delivery/storage/delivery_storage.hpp"
#include "parcel/storage/parcel_storage.hpp" 

class CreateDeliveryHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-create-delivery";

    CreateDeliveryHandler(const userver::components::ComponentConfig&,
                          const userver::components::ComponentContext&);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest&,
        userver::server::request::RequestContext&) const override;

private:
    DeliveryStorage& storage_;
    ParcelStorage& parcel_storage_;
};
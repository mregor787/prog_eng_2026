#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include "parcel/storage/parcel_storage.hpp"

class CreateParcelHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-create-parcel";

    CreateParcelHandler(const userver::components::ComponentConfig&,
                        const userver::components::ComponentContext&);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest&,
        userver::server::request::RequestContext&) const override;

private:
    ParcelStorage& storage_;
};
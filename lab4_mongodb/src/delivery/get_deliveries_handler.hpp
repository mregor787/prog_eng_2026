#pragma once

#include <memory>

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/auth/auth_checker_base.hpp>

#include "../common/db.hpp"
#include "../common/models.hpp"

namespace delivery_service::delivery {

// GET /api/v1/deliveries?sender_id=... - Get deliveries by sender ID
// GET /api/v1/deliveries?receiver_id=... - Get deliveries by receiver ID
class GetDeliveriesHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-deliveries";

    GetDeliveriesHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
};

}
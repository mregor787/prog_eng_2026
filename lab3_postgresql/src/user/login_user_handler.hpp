#pragma once

#include <memory>

#include <userver/server/handlers/http_handler_base.hpp>

#include "../common/db.hpp"
#include "../common/models.hpp"

namespace delivery_service::user {

// POST /api/v1/auth/login - Login user
class LoginHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-login";

    LoginHandler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const override;

private:
    std::shared_ptr<Database> db_;
    std::string GenerateToken(int64_t user_id) const;
};

}
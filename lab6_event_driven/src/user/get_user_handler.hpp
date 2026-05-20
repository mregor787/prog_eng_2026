#pragma once

#include <memory>

#include <userver/server/handlers/http_handler_base.hpp>

#include "../common/mongo_db.hpp"
#include "../common/models.hpp"

namespace delivery_service::user {

// GET /api/v1/users/{login} - Get user by login
class GetUserHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-get-user";

    GetUserHandler(const userver::components::ComponentConfig& config,
                          const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&) const override;

private:
    std::shared_ptr<MongoDatabase> db_;
};

}
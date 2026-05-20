#include "handlers.hpp"
#include "create_user_handler.hpp"
#include "get_user_handler.hpp"
#include "login_user_handler.hpp"
#include "search_users_handler.hpp"

namespace delivery_service::user {

void AppendUserHandlers(userver::components::ComponentList& component_list) {
    component_list.Append<CreateUserHandler>();
    component_list.Append<GetUserHandler>();
    component_list.Append<SearchUsersHandler>();
    component_list.Append<LoginHandler>();
}

}
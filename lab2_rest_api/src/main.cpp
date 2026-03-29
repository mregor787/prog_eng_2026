#include <userver/components/minimal_server_component_list.hpp>
#include <userver/utils/daemon_run.hpp>

#include "user/storage/user_storage.hpp"
#include "user/handlers/search_users_handler.hpp"
#include "user/handlers/create_user_handler.hpp"
#include "user/handlers/get_user_handler.hpp"
#include "parcel/storage/parcel_storage.hpp"
#include "parcel/handlers/create_parcel_handler.hpp"
#include "parcel/handlers/get_user_parcels_handler.hpp"
#include "delivery/storage/delivery_storage.hpp"
#include "delivery/handlers/create_delivery_handler.hpp"
#include "delivery/handlers/get_deliveries_handler.hpp"


int main(int argc, char* argv[]) {
    return userver::utils::DaemonMain(
        argc, argv,
        userver::components::MinimalServerComponentList()
            .Append<UserStorage>()
            .Append<SearchUsersHandler>()
            .Append<CreateUserHandler>()
            .Append<GetUserHandler>()
            .Append<ParcelStorage>()
            .Append<CreateParcelHandler>()
            .Append<GetUserParcelsHandler>()
            .Append<DeliveryStorage>()
            .Append<CreateDeliveryHandler>()
            .Append<GetDeliveriesHandler>()
    );
}
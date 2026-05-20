#include "handlers.hpp"
#include "create_parcel_handler.hpp"
#include "get_user_parcels_handler.hpp"

namespace delivery_service::parcel {

void AppendParcelHandlers(userver::components::ComponentList& component_list) {
    component_list.Append<CreateParcelHandler>();
    component_list.Append<GetUserParcelsHandler>();
}

} 
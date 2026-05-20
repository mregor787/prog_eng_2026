#include "handlers.hpp"
#include "create_delivery_handler.hpp"
#include "get_deliveries_handler.hpp"

namespace delivery_service::delivery {

void AppendDeliveryHandlers(userver::components::ComponentList& component_list) {
    component_list.Append<CreateDeliveryHandler>();
    component_list.Append<GetDeliveriesHandler>();
}

} 
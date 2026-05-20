#pragma once

#include <userver/components/component_list.hpp>

namespace delivery_service::delivery {

void AppendDeliveryHandlers(userver::components::ComponentList& component_list);

}
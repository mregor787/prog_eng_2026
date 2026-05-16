#pragma once

#include <userver/components/component_list.hpp>

namespace delivery_service::parcel {

void AppendParcelHandlers(userver::components::ComponentList& component_list);

} 
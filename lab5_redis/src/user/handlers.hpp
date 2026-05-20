#pragma once

#include <userver/components/component_list.hpp>

namespace delivery_service::user {

void AppendUserHandlers(userver::components::ComponentList& component_list);

}
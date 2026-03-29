#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include <userver/components/component_base.hpp>
#include "delivery/model/delivery.hpp"

class DeliveryStorage final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "delivery-storage";

    DeliveryStorage(const userver::components::ComponentConfig&,
                    const userver::components::ComponentContext&);

    bool CreateDelivery(const Delivery& delivery);

    std::vector<Delivery> GetBySender(const std::string& sender_id) const;
    std::vector<Delivery> GetByReceiver(const std::string& receiver_id) const;

private:
    std::unordered_map<std::string, Delivery> deliveries_;
};
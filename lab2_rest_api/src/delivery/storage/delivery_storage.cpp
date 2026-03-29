#include "delivery/storage/delivery_storage.hpp"

DeliveryStorage::DeliveryStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {}

bool DeliveryStorage::CreateDelivery(const Delivery& delivery) {
    if (deliveries_.count(delivery.id)) {
        return false;
    }

    deliveries_[delivery.id] = delivery;
    return true;
}

std::vector<Delivery> DeliveryStorage::GetBySender(const std::string& sender_id) const {
    std::vector<Delivery> result;

    for (const auto& [id, d] : deliveries_) {
        if (d.sender_id == sender_id) {
            result.push_back(d);
        }
    }

    return result;
}

std::vector<Delivery> DeliveryStorage::GetByReceiver(const std::string& receiver_id) const {
    std::vector<Delivery> result;

    for (const auto& [id, d] : deliveries_) {
        if (d.receiver_id == receiver_id) {
            result.push_back(d);
        }
    }

    return result;
}
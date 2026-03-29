#include "parcel/storage/parcel_storage.hpp"

ParcelStorage::ParcelStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {}

bool ParcelStorage::CreateParcel(const Parcel& parcel) {
    if (parcels_.count(parcel.id)) {
        return false;
    }

    parcels_[parcel.id] = parcel;
    return true;
}

std::vector<Parcel> ParcelStorage::GetParcelsByUser(const std::string& user_id) const {
    std::vector<Parcel> result;

    for (const auto& [id, parcel] : parcels_) {
        if (parcel.sender_id == user_id) {
            result.push_back(parcel);
        }
    }

    return result;
}

std::optional<Parcel> ParcelStorage::GetParcelById(const std::string& id) const {
    auto it = parcels_.find(id);
    if (it == parcels_.end()) {
        return std::nullopt;
    }
    return it->second;
}
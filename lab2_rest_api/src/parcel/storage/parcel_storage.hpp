#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <optional>

#include <userver/components/component_base.hpp>
#include "parcel/model/parcel.hpp"

class ParcelStorage final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "parcel-storage";

    ParcelStorage(const userver::components::ComponentConfig&,
                  const userver::components::ComponentContext&);

    bool CreateParcel(const Parcel& parcel);

    std::vector<Parcel> GetParcelsByUser(const std::string& user_id) const;

    std::optional<Parcel> GetParcelById(const std::string& id) const;

private:
    std::unordered_map<std::string, Parcel> parcels_;
};
#pragma once

#include <string>
#include <vector>
#include <optional>

#include <userver/storages/postgres/cluster.hpp>

#include "models.hpp"

namespace delivery_service {

class Database {
public:
    explicit Database(userver::storages::postgres::ClusterPtr pg_cluster);
    ~Database();

    std::optional<User> CreateUser(const CreateUserRequest& request);
    std::optional<User> GetUserByLogin(const std::string& login);
    std::optional<User> GetUserById(int64_t id);
    std::vector<User> SearchUsersByQuery(const std::string& query);
    std::optional<User> AuthenticateUser(const std::string& login, const std::string& password);

    std::optional<Parcel> CreateParcel(int64_t sender_id, const CreateParcelRequest& request);
    std::optional<Parcel> GetParcelById(int64_t parcel_id);
    std::vector<Parcel> GetUserParcels(int64_t user_id);

    std::optional<Delivery> CreateDelivery(int64_t sender_id, const CreateDeliveryRequest& request);
    std::optional<Delivery> GetDeliveryById(int64_t delivery_id);
    std::vector<Delivery> GetDeliveriesBySender(int64_t sender_id);
    std::vector<Delivery> GetDeliveriesByReceiver(int64_t receiver_id);

private:
    userver::storages::postgres::ClusterPtr pg_;
};

}
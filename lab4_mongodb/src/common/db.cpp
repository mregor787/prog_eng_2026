#include "db.hpp"

#include <userver/storages/postgres/cluster.hpp>
#include <cstdlib>

namespace delivery_service {

Database::Database(userver::storages::postgres::ClusterPtr pg_cluster) 
    : pg_(std::move(pg_cluster)) {}

Database::~Database() = default;

std::optional<Parcel> Database::CreateParcel(int64_t sender_id, const CreateParcelRequest& request) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO parcels (sender_id, description, weight) "
        "VALUES ($1, $2, $3) RETURNING id, sender_id, description, weight, created_at",
        sender_id, request.description, request.weight);

    if (res.IsEmpty()) return std::nullopt;

    auto row = res[0];
    Parcel p;
    p.id = row["id"].As<int64_t>();
    p.sender_id = row["sender_id"].As<int64_t>();
    p.description = row["description"].As<std::string>();
    p.weight = row["weight"].As<double>();
    p.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return p;
}

std::optional<Parcel> Database::GetParcelById(int64_t parcel_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, sender_id, description, weight, created_at FROM parcels WHERE id = $1",
        parcel_id);

    if (res.IsEmpty()) return std::nullopt;

    auto row = res[0];
    Parcel p;
    p.id = row["id"].As<int64_t>();
    p.sender_id = row["sender_id"].As<int64_t>();
    p.description = row["description"].As<std::string>();
    p.weight = row["weight"].As<double>();
    p.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return p;
}

std::vector<Parcel> Database::GetUserParcels(int64_t user_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, sender_id, full_name, description, weight, created_at FROM parcels WHERE sender_id = $1 ORDER BY created_at DESC",
        user_id);

    std::vector<Parcel> results;
    for (auto row : res) {
        Parcel p;
        p.id = row["id"].As<int64_t>();
        p.sender_id = row["sender_id"].As<int64_t>();
        p.description = row["description"].As<std::string>();
        p.weight = row["weight"].As<double>();
        p.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(p);
    }
    return results;
}

std::optional<Delivery> Database::CreateDelivery(int64_t sender_id, const CreateDeliveryRequest& request) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO deliveries (sender_id, receiver_id, parcel_id, status, cost) "
        "VALUES ($1, $2, $3, $4, $5) RETURNING id, sender_id, receiver_id, parcel_id, status, cost, created_at",
        sender_id, request.receiver_id, request.receiver_id, 10.0 + (rand() % 100));

    if (res.IsEmpty()) return std::nullopt;

    auto row = res[0];
    Delivery d;
    d.id = row["id"].As<int64_t>();
    d.sender_id = row["sender_id"].As<int64_t>();
    d.receiver_id = row["receiver_id"].As<int64_t>();
    d.parcel_id = row["parcel_id"].As<int64_t>();
    d.status = row["status"].As<std::string>();
    d.cost = row["cost"].As<std::optional<double>>();
    d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return d;
}

std::optional<Delivery> Database::GetDeliveryById(int64_t delivery_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at "
        "FROM deliveries WHERE id = $1",
        delivery_id);

    if (res.IsEmpty()) return std::nullopt;

    auto row = res[0];
    Delivery d;
    d.id = row["id"].As<int64_t>();
    d.sender_id = row["sender_id"].As<int64_t>();
    d.receiver_id = row["receiver_id"].As<int64_t>();
    d.parcel_id = row["parcel_id"].As<int64_t>();
    d.status = row["status"].As<std::string>();
    d.cost = row["cost"].As<std::optional<double>>();
    d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return d;
}

std::vector<Delivery> Database::GetDeliveriesBySender(int64_t sender_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at "
        "FROM deliveries WHERE sender_id = $1 ORDER BY created_at DESC",
        sender_id);

    std::vector<Delivery> results;
    for (auto row : res) {
        Delivery d;
        d.id = row["id"].As<int64_t>();
        d.sender_id = row["sender_id"].As<int64_t>();
        d.receiver_id = row["receiver_id"].As<int64_t>();
        d.parcel_id = row["parcel_id"].As<int64_t>();
        d.status = row["status"].As<std::string>();
        d.cost = row["cost"].As<std::optional<double>>();
        d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(d);
    }
    return results;
}

std::vector<Delivery> Database::GetDeliveriesByReceiver(int64_t receiver_id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at "
        "FROM deliveries WHERE receiver_id = $1 ORDER BY created_at DESC",
        receiver_id);

    std::vector<Delivery> results;
    for (auto row : res) {
        Delivery d;
        d.id = row["id"].As<int64_t>();
        d.sender_id = row["sender_id"].As<int64_t>();
        d.receiver_id = row["receiver_id"].As<int64_t>();
        d.parcel_id = row["parcel_id"].As<int64_t>();
        d.status = row["status"].As<std::string>();
        d.cost = row["cost"].As<std::optional<double>>();
        d.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(d);
    }
    return results;
}

}
#include "db.hpp"

#include <userver/crypto/hash.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <cstdlib>

namespace delivery_service {

namespace {
    std::string HashPassword(const std::string& password) {
        return userver::crypto::hash::Sha256(password);
    }

    bool VerifyPassword(const std::string& password, const std::string& hash) {
        return HashPassword(password) == hash;
    }
}

Database::Database(userver::storages::postgres::ClusterPtr pg_cluster) 
    : pg_(std::move(pg_cluster)) {}

Database::~Database() = default;

std::optional<User> Database::CreateUser(const CreateUserRequest& request) {
    auto hash = HashPassword(request.password);
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO users (login, email, first_name, last_name, password_hash) "
        "VALUES ($1, $2, $3, $4, $5) RETURNING id, login, email, first_name, last_name, password_hash, created_at",
        request.login, request.email, request.first_name, request.last_name, hash);

    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    User u;
    u.id = row["id"].As<int64_t>();
    u.login = row["login"].As<std::string>();
    u.email = row["email"].As<std::string>();
    u.first_name = row["first_name"].As<std::string>();
    u.last_name = row["last_name"].As<std::string>();
    u.password_hash = row["password_hash"].As<std::string>();
    u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::optional<User> Database::GetUserByLogin(const std::string& login) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE login = $1",
        login);

    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    User u;
    u.id = row["id"].As<int64_t>();
    u.login = row["login"].As<std::string>();
    u.email = row["email"].As<std::string>();
    u.first_name = row["first_name"].As<std::string>();
    u.last_name = row["last_name"].As<std::string>();
    u.password_hash = row["password_hash"].As<std::string>();
    u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::optional<User> Database::GetUserById(int64_t id) {
    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE id = $1",
        id);

    if (res.IsEmpty()) return std::nullopt;
    
    auto row = res[0];
    User u;
    u.id = row["id"].As<int64_t>();
    u.login = row["login"].As<std::string>();
    u.email = row["email"].As<std::string>();
    u.first_name = row["first_name"].As<std::string>();
    u.last_name = row["last_name"].As<std::string>();
    u.password_hash = row["password_hash"].As<std::string>();
    u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
    return u;
}

std::vector<User> Database::SearchUsersByQuery(const std::string& query) {
    std::string search = "%" + query + "%";

    auto res = pg_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE "
        "first_name LIKE $1 OR last_name LIKE $2 OR first_name || ' ' || last_name LIKE $3",
        search, search, search);

    std::vector<User> results;
    for (auto row : res) {
        User u;
        u.id = row["id"].As<int64_t>();
        u.login = row["login"].As<std::string>();
        u.email = row["email"].As<std::string>();
        u.first_name = row["first_name"].As<std::string>();
        u.last_name = row["last_name"].As<std::string>();
        u.password_hash = row["password_hash"].As<std::string>();
        u.created_at = row["created_at"].As<std::chrono::system_clock::time_point>();
        results.push_back(u);
    }
    return results;
}

std::optional<User> Database::AuthenticateUser(const std::string& login, const std::string& password) {
    auto u = GetUserByLogin(login);
    if (!u || !VerifyPassword(password, u->password_hash)) return std::nullopt;
    return u;
}

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
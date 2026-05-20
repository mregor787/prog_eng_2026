#pragma once

#include <string>
#include <vector>
#include <optional>
#include <userver/storages/mongo/pool.hpp>

#include "models.hpp"

namespace delivery_service {

class MongoDatabase {
public:
    explicit MongoDatabase(userver::storages::mongo::PoolPtr mongo_pool);
    ~MongoDatabase();

    std::optional<User> CreateUser(const CreateUserRequest& request);
    std::optional<User> GetUserByLogin(const std::string& login);
    std::optional<User> GetUserById(int64_t id);
    std::vector<User> SearchUsersByQuery(const std::string& query);
    std::optional<User> AuthenticateUser(const std::string& login, const std::string& password);

private:
    userver::storages::mongo::PoolPtr mongo_;
};

}
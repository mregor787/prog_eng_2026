#pragma once

#include <userver/components/component_base.hpp>
#include "user/model/user.hpp"

#include <unordered_map>
#include <optional>
#include <mutex>

class UserStorage final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "user-storage";

    UserStorage(const userver::components::ComponentConfig&,
                const userver::components::ComponentContext&);

    bool CreateUser(const User& user);
    std::optional<User> GetUserByLogin(const std::string& login);

    std::vector<User> FindUsersByNameMask(const std::string& mask) const;

private:
    std::unordered_map<std::string, User> users_;
    std::mutex mutex_;
};
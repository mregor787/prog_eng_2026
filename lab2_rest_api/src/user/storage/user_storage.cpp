#include <algorithm>

#include "user/storage/user_storage.hpp"

UserStorage::UserStorage(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context)
    : ComponentBase(config, context) {}

bool UserStorage::CreateUser(const User& user) {
    std::lock_guard lock(mutex_);

    if (users_.count(user.login)) {
        return false;
    }

    users_[user.login] = user;
    return true;
}

std::optional<User> UserStorage::GetUserByLogin(const std::string& login) {
    std::lock_guard lock(mutex_);

    if (!users_.count(login)) {
        return std::nullopt;
    }

    return users_[login];
}

std::vector<User> UserStorage::FindUsersByNameMask(const std::string& mask) const {
    std::vector<User> result;

    auto to_lower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };

    const auto mask_lower = to_lower(mask);

    for (const auto& [login, user] : users_) {
        auto first = to_lower(user.first_name);
        auto last = to_lower(user.last_name);

        if (first.find(mask_lower) != std::string::npos ||
            last.find(mask_lower) != std::string::npos) {
            result.push_back(user);
        }
    }

    return result;
}
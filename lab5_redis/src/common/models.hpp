#pragma once

#include <string>
#include <vector>
#include <optional>
#include <chrono>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace delivery_service {

struct User {
    std::optional<int64_t> id;
    std::string login;
    std::string email;
    std::string first_name;
    std::string last_name;
    std::string password_hash;
    std::chrono::system_clock::time_point created_at;

    userver::formats::json::Value ToJson() const;
    static User FromJson(const userver::formats::json::Value& json);
};

struct Parcel {
    std::optional<int64_t> id;
    int64_t sender_id;
    std::string description;
    double weight;
    std::chrono::system_clock::time_point created_at;

    userver::formats::json::Value ToJson() const;
    static Parcel FromJson(const userver::formats::json::Value& json);
};

struct Delivery {
    std::optional<int64_t> id;
    int64_t sender_id;
    int64_t receiver_id;
    int64_t parcel_id;
    std::string status;
    std::optional<double> cost;
    std::chrono::system_clock::time_point created_at;

    userver::formats::json::Value ToJson() const;
    static Delivery FromJson(const userver::formats::json::Value& json);
};

struct AuthToken {
    std::string token;
    int64_t user_id;
    std::chrono::system_clock::time_point expires_at;

    userver::formats::json::Value ToJson() const;
};

struct LoginRequest {
    std::string login;
    std::string password;

    static LoginRequest FromJson(const userver::formats::json::Value& json);
};

struct CreateUserRequest {
    std::string login;
    std::string email;
    std::string first_name;
    std::string last_name;
    std::string password;

    static CreateUserRequest FromJson(const userver::formats::json::Value& json);
};

struct CreateParcelRequest {
    std::string description;
    double weight;

    static CreateParcelRequest FromJson(const userver::formats::json::Value& json);
};

struct CreateDeliveryRequest {
    int64_t receiver_id;
    int64_t parcel_id;

    static CreateDeliveryRequest FromJson(const userver::formats::json::Value& json);
};

}
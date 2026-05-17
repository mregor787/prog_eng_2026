#include "models.hpp"

#include <userver/formats/json/serialize.hpp>
#include <userver/utils/datetime.hpp>

namespace delivery_service {

userver::formats::json::Value User::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    if (id) {
        builder["id"] = *id;
    }
    builder["login"] = login;
    builder["email"] = email;
    builder["first_name"] = first_name;
    builder["last_name"] = last_name;
    builder["created_at"] = userver::utils::datetime::Timestring(created_at);
    return builder.ExtractValue();
}

User User::FromJson(const userver::formats::json::Value& json) {
    User user;
    if (json.HasMember("id")) {
        user.id = json["id"].As<int64_t>();
    }
    user.login = json["login"].As<std::string>();
    user.email = json["email"].As<std::string>();
    user.first_name = json["first_name"].As<std::string>();
    user.last_name = json["last_name"].As<std::string>();
    if (json.HasMember("password")) {
        user.password_hash = json["password"].As<std::string>();
    }
    return user;
}

userver::formats::json::Value Parcel::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    if (id) {
        builder["id"] = *id;
    }
    builder["sender_id"] = sender_id;
    builder["description"] = description;
    builder["weight"] = weight;
    builder["created_at"] = userver::utils::datetime::Timestring(created_at);
    return builder.ExtractValue();
}

Parcel Parcel::FromJson(const userver::formats::json::Value& json) {
    Parcel parcel;
    if (json.HasMember("id")) {
        parcel.id = json["id"].As<int64_t>();
    }
    if (json.HasMember("sender_id")) {
        parcel.sender_id = json["sender_id"].As<int64_t>();
    }
    parcel.description = json["description"].As<std::string>();
    parcel.weight = json["weight"].As<double>();
    return parcel;
}

userver::formats::json::Value Delivery::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    if (id) {
        builder["id"] = *id;
    }
    builder["sender_id"] = sender_id;
    builder["receiver_id"] = receiver_id;
    builder["parcel_id"] = parcel_id;
    builder["status"] = status;
    if (cost) {
        builder["cost"] = *cost;
    }
    builder["created_at"] = userver::utils::datetime::Timestring(created_at);
    return builder.ExtractValue();
}

Delivery Delivery::FromJson(const userver::formats::json::Value& json) {
    Delivery delivery;
    if (json.HasMember("id")) {
        delivery.id = json["id"].As<int64_t>();
    }
    if (json.HasMember("sender_id")) {
        delivery.sender_id = json["sender_id"].As<int64_t>();
    }
    delivery.receiver_id = json["receiver_id"].As<int64_t>();
    delivery.parcel_id = json["parcel_id"].As<int64_t>();
    if (json.HasMember("status")) {
        delivery.status = json["status"].As<std::string>();
    }
    if (json.HasMember("cost")) {
        delivery.cost = json["cost"].As<double>();
    }
    return delivery;
}

userver::formats::json::Value AuthToken::ToJson() const {
    userver::formats::json::ValueBuilder builder;
    builder["token"] = token;
    builder["user_id"] = user_id;
    builder["expires_at"] = userver::utils::datetime::Timestring(expires_at);
    return builder.ExtractValue();
}

LoginRequest LoginRequest::FromJson(const userver::formats::json::Value& json) {
    LoginRequest request;
    request.login = json["login"].As<std::string>();
    request.password = json["password"].As<std::string>();
    return request;
}

CreateUserRequest CreateUserRequest::FromJson(const userver::formats::json::Value& json) {
    CreateUserRequest request;
    request.login = json["login"].As<std::string>();
    request.email = json["email"].As<std::string>();
    request.first_name = json["first_name"].As<std::string>();
    request.last_name = json["last_name"].As<std::string>();
    request.password = json["password"].As<std::string>();
    return request;
}

CreateParcelRequest CreateParcelRequest::FromJson(const userver::formats::json::Value& json) {
    CreateParcelRequest request;
    request.description = json["description"].As<std::string>();
    request.weight = json["weight"].As<double>();
    return request;
}

CreateDeliveryRequest CreateDeliveryRequest::FromJson(const userver::formats::json::Value& json) {
    CreateDeliveryRequest request;
    request.receiver_id = json["receiver_id"].As<int64_t>();
    request.parcel_id = json["parcel_id"].As<int64_t>();
    return request;
}

}
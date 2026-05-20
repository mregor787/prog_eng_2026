#include "producer.hpp"

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/json/serialize.hpp>

#include <iostream>

namespace delivery_service::parcel {

ParcelEventProducer::ParcelEventProducer() {
    connection_ = amqp_new_connection();

    auto* socket =
        amqp_tcp_socket_new(
            static_cast<amqp_connection_state_t>(connection_)
        );

    amqp_socket_open(socket, "rabbitmq", 5672);

    amqp_login(
        static_cast<amqp_connection_state_t>(connection_),
        "/",
        0,
        131072,
        0,
        AMQP_SASL_METHOD_PLAIN,
        "guest",
        "guest"
    );

    amqp_channel_open(
        static_cast<amqp_connection_state_t>(connection_),
        1
    );

    channel_ = reinterpret_cast<void*>(1);

    amqp_exchange_declare(
        static_cast<amqp_connection_state_t>(connection_),
        1,
        amqp_cstring_bytes("delivery.events"),
        amqp_cstring_bytes("topic"),
        0,
        1,
        0,
        0,
        amqp_empty_table
    );
}

void ParcelEventProducer::PublishParcelCreated(
    int64_t parcel_id,
    int64_t sender_id,
    const std::string& description,
    double weight
) {
    userver::formats::json::ValueBuilder payload;

    payload["event_type"] = "ParcelCreated";

    payload["payload"]["parcel_id"] = parcel_id;
    payload["payload"]["sender_id"] = sender_id;
    payload["payload"]["description"] = description;
    payload["payload"]["weight"] = weight;

    auto json =
        userver::formats::json::ToString(
            payload.ExtractValue()
        );

    amqp_basic_publish(
        static_cast<amqp_connection_state_t>(connection_),
        1,
        amqp_cstring_bytes("delivery.events"),
        amqp_cstring_bytes("parcel.created"),
        0,
        0,
        nullptr,
        amqp_cstring_bytes(json.c_str())
    );

    std::cout << "Published ParcelCreated event\n";
}

}
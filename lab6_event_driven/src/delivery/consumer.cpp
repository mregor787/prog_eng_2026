#include "consumer.hpp"

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <iostream>

namespace delivery_service::delivery {

ParcelEventConsumer::ParcelEventConsumer() {}

void ParcelEventConsumer::StartConsuming() {
    auto connection = amqp_new_connection();

    auto* socket = amqp_tcp_socket_new(connection);

    amqp_socket_open(socket, "rabbitmq", 5672);

    amqp_login(
        connection,
        "/",
        0,
        131072,
        0,
        AMQP_SASL_METHOD_PLAIN,
        "guest",
        "guest"
    );

    amqp_channel_open(connection, 1);

    amqp_queue_declare(
        connection,
        1,
        amqp_cstring_bytes("parcel.queue"),
        0,
        0,
        0,
        1,
        amqp_empty_table
    );

    amqp_queue_bind(
        connection,
        1,
        amqp_cstring_bytes("parcel.queue"),
        amqp_cstring_bytes("delivery.events"),
        amqp_cstring_bytes("parcel.created"),
        amqp_empty_table
    );

    amqp_basic_consume(
        connection,
        1,
        amqp_cstring_bytes("parcel.queue"),
        amqp_empty_bytes,
        0,
        1,
        0,
        amqp_empty_table
    );

    std::cout << "Waiting for ParcelCreated events...\n";

    while (true) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(connection);

        res = amqp_consume_message(
            connection,
            &envelope,
            nullptr,
            0
        );

        if (res.reply_type != AMQP_RESPONSE_NORMAL) {
            continue;
        }

        std::string body(
            static_cast<char*>(envelope.message.body.bytes),
            envelope.message.body.len
        );

        std::cout << "Received event: " << body << std::endl;

        amqp_destroy_envelope(&envelope);
    }
}

}
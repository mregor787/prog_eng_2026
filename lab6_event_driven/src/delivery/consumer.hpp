#pragma once

namespace delivery_service::delivery {

class ParcelEventConsumer {
public:
    ParcelEventConsumer();

    void StartConsuming();
};

}
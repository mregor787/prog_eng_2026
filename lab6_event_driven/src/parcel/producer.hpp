#pragma once

#include <string>

namespace delivery_service::parcel {

class ParcelEventProducer {
public:
    ParcelEventProducer();

    void PublishParcelCreated(
        int64_t parcel_id,
        int64_t sender_id,
        const std::string& description,
        double weight
    );

private:
    void* connection_;
    void* channel_;
};

}
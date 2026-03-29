#pragma once

#include <string>

struct Delivery {
    std::string id;
    std::string sender_id;
    std::string receiver_id;
    std::string parcel_id;
    std::string status;
};
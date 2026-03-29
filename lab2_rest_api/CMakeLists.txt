cmake_minimum_required(VERSION 3.15)
project(delivery_service CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(userver REQUIRED COMPONENTS core)

add_executable(delivery_service
    src/main.cpp
    src/user/storage/user_storage.cpp
    src/user/handlers/search_users_handler.cpp
    src/user/handlers/create_user_handler.cpp
    src/user/handlers/get_user_handler.cpp
    src/parcel/storage/parcel_storage.cpp
    src/parcel/handlers/create_parcel_handler.cpp
    src/parcel/handlers/get_user_parcels_handler.cpp
    src/delivery/storage/delivery_storage.cpp
    src/delivery/handlers/create_delivery_handler.cpp
    src/delivery/handlers/get_deliveries_handler.cpp
)

target_include_directories(delivery_service PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(delivery_service userver::core)
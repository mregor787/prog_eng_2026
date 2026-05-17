cmake_minimum_required(VERSION 3.12)
project(delivery-service CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(userver REQUIRED COMPONENTS core postgresql)

add_executable(${PROJECT_NAME}-user
    src/user/main.cpp
    src/user/handlers.cpp
    src/user/create_user_handler.cpp
    src/user/get_user_handler.cpp
    src/user/login_user_handler.cpp
    src/user/search_users_handler.cpp
    src/common/db.cpp
    src/common/models.cpp
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
)

target_include_directories(${PROJECT_NAME}-user PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}-user PRIVATE
    userver::core
    userver::postgresql
)

add_executable(${PROJECT_NAME}-parcel
    src/parcel/main.cpp
    src/parcel/handlers.cpp
    src/parcel/create_parcel_handler.cpp
    src/parcel/get_user_parcels_handler.cpp
    src/common/db.cpp
    src/common/models.cpp
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
)

target_include_directories(${PROJECT_NAME}-parcel PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}-parcel PRIVATE
    userver::core
    userver::postgresql
)

add_executable(${PROJECT_NAME}-delivery
    src/delivery/main.cpp
    src/delivery/handlers.cpp
    src/delivery/create_delivery_handler.cpp
    src/delivery/get_deliveries_handler.cpp
    src/common/db.cpp
    src/common/models.cpp
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
)

target_include_directories(${PROJECT_NAME}-delivery PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}-delivery PRIVATE
    userver::core
    userver::postgresql
)

install(TARGETS ${PROJECT_NAME}-user ${PROJECT_NAME}-parcel ${PROJECT_NAME}-delivery
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)

install(DIRECTORY configs/
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)
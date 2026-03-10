workspace "Delivery Service" {
    
    model {
        user = person "User" "Customer who sends and receives parcels"
        admin = person "Admin" "Service operator who monitors deliveries"
        emailService = softwareSystem "Email Service" "External system used to send email notifications"
        paymentSystem = softwareSystem "Payment System" "External payment processing service"

        deliverySystem = softwareSystem "Delivery Service" "System for managing parcel deliveries between users" {
            api = container "API Gateway" "Main entry point for clients. Handles HTTP requests and routes them to services." "C++ (userver)"
            userService = container "User Service" "Handles user registration and user search." "C++ (userver)"
            parcelService = container "Parcel Service" "Handles parcel creation and retrieval." "C++ (userver)"
            deliveryMgmt = container "Delivery Management Service" "Handles creation and management of deliveries." "C++ (userver)"
            postgres = container "Relational Database" "Stores users, parcels and deliveries." "PostgreSQL"
            mongo = container "Tracking Database" "Stores delivery tracking events and logs." "MongoDB"
            redis = container "Cache" "Caches frequently accessed data." "Redis"

            api -> userService "Calls user operations" "HTTPS/REST"
            api -> parcelService "Calls parcel operations" "HTTPS/REST"
            api -> deliveryMgmt "Calls delivery operations" "HTTPS/REST"

            userService -> postgres "Reads/Writes users" "JDBC"
            parcelService -> postgres "Reads/Writes parcels" "JDBC"
            deliveryMgmt -> postgres "Reads/Writes deliveries" "JDBC"

            //userService -> redis "Caches users" "Redis Protocol"
            deliveryMgmt -> redis "Caches deliveries" "Redis Protocol"

            deliveryMgmt -> mongo "Stores tracking events" "MongoDB Driver"
            deliveryMgmt -> emailService "Sends delivery notifications" "SMTP/API"
            deliveryMgmt -> paymentSystem "Processes delivery payment" "HTTPS API"
        }

        user -> api "Uses the system via web/mobile client" "HTTPS"
        admin -> api "Monitors deliveries and users" "HTTPS"
    }

    views {
        themes default

        systemContext deliverySystem {
            include *
            autolayout lr
            title "System Context Diagram"
        }

        container deliverySystem {
            include *
            autolayout lr
            title "Container Diagram"
        }

        dynamic deliverySystem create_delivery_scenario "Create delivery scenario" {
            autolayout lr
            title "Dynamic Diagram - Creating a Delivery"

            user -> api "POST /deliveries"
            api -> userService "Validate sender"
            api -> userService "Validate receiver"
            api -> parcelService "Create parcel"
            api -> deliveryMgmt "Create delivery"
            deliveryMgmt -> postgres "Save delivery"
            deliveryMgmt -> mongo "Write tracking event"
            deliveryMgmt -> emailService "Send delivery notification"
        }
    }
}
db.createCollection("users", {
    validator: {
        $jsonSchema: {
            bsonType: "object",
            required: [
                "_id",
                "login",
                "email",
                "password_hash",
                "first_name",
                "last_name",
                "created_at",
                "preferences",
                "statistics"
            ],
            properties: {
                _id: {
                    bsonType: "long",
                    description: "User ID must be Int64 and is required"
                },

                login: {
                    bsonType: "string",
                    minLength: 3,
                    maxLength: 64,
                    description: "Login must be a string"
                },

                email: {
                    bsonType: "string",
                    pattern: "^.+@.+\\..+$",
                    description: "Email must be valid"
                },

                password_hash: {
                    bsonType: "string",
                    minLength: 32,
                    description: "Password hash must be a string"
                },

                first_name: {
                    bsonType: "string",
                    minLength: 1,
                    maxLength: 64,
                    description: "First name must be a string"
                },

                last_name: {
                    bsonType: "string",
                    minLength: 1,
                    maxLength: 64,
                    description: "Last name must be a string"
                },

                created_at: {
                    bsonType: "date",
                    description: "Creation timestamp is required"
                },

                preferences: {
                    bsonType: "object",
                    required: [
                        "language",
                        "notifications_enabled"
                    ],
                    properties: {
                        language: {
                            bsonType: "string",
                            enum: ["en", "ru", "de", "fr", "es"],
                            description: "Preferred language"
                        },

                        notifications_enabled: {
                            bsonType: "bool",
                            description: "Notification flag"
                        }
                    }
                },

                statistics: {
                    bsonType: "object",
                    required: [
                        "sent_parcels",
                        "received_parcels"
                    ],
                    properties: {
                        sent_parcels: {
                            bsonType: "int",
                            minimum: 0,
                            description: "Number of sent parcels"
                        },

                        received_parcels: {
                            bsonType: "int",
                            minimum: 0,
                            description: "Number of received parcels"
                        }
                    }
                }
            }
        }
    },

    validationLevel: "strict",
    validationAction: "error"
});


// valid document example
db.users.insertOne({
    _id: NumberLong(100),
    login: "valid_user",
    email: "valid@example.com",
    password_hash: "2bb80d537b1da3e38bd30361aa855686",
    first_name: "Valid",
    last_name: "User",
    created_at: new Date(),

    preferences: {
        language: "en",
        notifications_enabled: true
    },

    statistics: {
        sent_parcels: 12,
        received_parcels: 7
    }
});

// invalid document examples

// Missing required field "email"
db.users.insertOne({
    _id: NumberLong(101),
    login: "broken_user",
    password_hash: "hash",
    first_name: "Broken",
    last_name: "User",
    created_at: new Date(),

    preferences: {
        language: "en",
        notifications_enabled: true
    },

    statistics: {
        sent_parcels: 1,
        received_parcels: 2
    }
});

// Invalid email format
db.users.insertOne({
    _id: NumberLong(102),
    login: "bad_email",
    email: "not-an-email",
    password_hash: "hash",
    first_name: "Bad",
    last_name: "Email",
    created_at: new Date(),

    preferences: {
        language: "en",
        notifications_enabled: true
    },

    statistics: {
        sent_parcels: 3,
        received_parcels: 1
    }
});

// Invalid language value
db.users.insertOne({
    _id: NumberLong(103),
    login: "bad_language",
    email: "badlang@example.com",
    password_hash: "hash",
    first_name: "Bad",
    last_name: "Language",
    created_at: new Date(),

    preferences: {
        language: "jp",
        notifications_enabled: true
    },

    statistics: {
        sent_parcels: 5,
        received_parcels: 2
    }
});

// Wrong type for statistics field
db.users.insertOne({
    _id: NumberLong(104),
    login: "wrong_type",
    email: "wrong@example.com",
    password_hash: "hash",
    first_name: "Wrong",
    last_name: "Type",
    created_at: new Date(),

    preferences: {
        language: "en",
        notifications_enabled: true
    },

    statistics: {
        sent_parcels: "many",
        received_parcels: 2
    }
});

// Show collection validation rules
db.getCollectionInfos({
    name: "users"
});
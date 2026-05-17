// Create user
var new_user = db.users.insertOne({
    _id: NumberLong(11),
    login: "new_user",
    email: "new_user@example.com",
    password_hash: "generated_hash",
    first_name: "New",
    last_name: "User",
    created_at: new Date(),
    preferences: {
      language: "en",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 2,
      received_parcels: 11
    }
});


// Get user by login
var user_by_login = db.users.findOne({ login: "new_user"});


// Get user by id
var user_by_id = db.users.findOne({ _id: NumberLong(11)});


// Search users by name/surname mask
var users_by_mask = db.users.find({
    $or: [
        { first_name: { $regex: "new", $options: "i" } },
        { last_name: { $regex: "user", $options: "i" } },
        {
            $expr: {
                $regexMatch: {
                    input: { $concat: ["$first_name", " ", "$last_name"] },
                    regex: "new",
                    options: "i"
                }
            }
        }
    ]
});

// Update user preferences
db.users.updateOne(
    { _id: NumberLong(1) },
    {
        $set: {
            "preferences.language": "de",
            "preferences.notifications_enabled": false
        }
    }
);

// Count users
db.users.countDocuments();

// Find users with notifications enabled
db.users.find({"preferences.notifications_enabled": true});

// Sort users by creation date
db.users.find().sort({created_at: -1});

// Delete user by id
db.users.deleteOne({ _id: NumberLong(11)});

db.users.drop();
db.users.insertMany([
  {
    _id: NumberLong(1),
    login: "egor",
    email: "egor@example.com",
    password_hash: "8f0e2f76e22b43b4b1e9c7c6f44a2d3a9e5d7b2f8a6c1d4e9f0a1b2c3d4e5f6",
    first_name: "Egor",
    last_name: "Belousov",
    created_at: new Date("2026-05-01T10:00:00Z"),
    preferences: {
      language: "ru",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 5,
      received_parcels: 2
    }
  },
  {
    _id: NumberLong(2),
    login: "john_doe",
    email: "john@example.com",
    password_hash: "7c9d3a5e1b2f4d6c8a0f9e7d5c3b1a2f6e8d0c4b9a7e5d3c1f2a4b6d8e0f1a3",
    first_name: "John",
    last_name: "Doe",
    created_at: new Date("2026-05-02T11:15:00Z"),
    preferences: {
      language: "en",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 3,
      received_parcels: 7
    }
  },
  {
    _id: NumberLong(3),
    login: "alice",
    email: "alice@example.com",
    password_hash: "1a3c5e7f9b2d4f6a8c0e1d3b5f7a9c2e4d6b8f0a1c3e5d7f9a2b4c6d8e0f1a2",
    first_name: "Alice",
    last_name: "Smith",
    created_at: new Date("2026-05-03T09:30:00Z"),
    preferences: {
      language: "en",
      notifications_enabled: false
    },
    statistics: {
      sent_parcels: 8,
      received_parcels: 1
    }
  },
  {
    _id: NumberLong(4),
    login: "maria",
    email: "maria@example.com",
    password_hash: "4d6f8a0c2e4b6d8f1a3c5e7b9d0f2a4c6e8b1d3f5a7c9e0b2d4f6a8c1e3b5d7",
    first_name: "Maria",
    last_name: "Ivanova",
    created_at: new Date("2026-05-04T14:20:00Z"),
    preferences: {
      language: "ru",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 12,
      received_parcels: 4
    }
  },
  {
    _id: NumberLong(5),
    login: "peter",
    email: "peter@example.com",
    password_hash: "9f1d3b5a7c0e2f4d6b8a1c3e5f7d9a2b4c6e8f0a1d3b5c7e9f2a4d6b8c0e1f3",
    first_name: "Peter",
    last_name: "Brown",
    created_at: new Date("2026-05-05T08:45:00Z"),
    preferences: {
      language: "de",
      notifications_enabled: false
    },
    statistics: {
      sent_parcels: 1,
      received_parcels: 9
    }
  },
  {
    _id: NumberLong(6),
    login: "anna",
    email: "anna@example.com",
    password_hash: "2b4d6f8a0c1e3f5a7d9b2c4e6f8a1d3b5c7e9f0a2d4b6c8e1f3a5d7b9c0e2f4",
    first_name: "Anna",
    last_name: "Klein",
    created_at: new Date("2026-05-06T16:10:00Z"),
    preferences: {
      language: "de",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 6,
      received_parcels: 6
    }
  },
  {
    _id: NumberLong(7),
    login: "ivan",
    email: "ivan@example.com",
    password_hash: "5c7e9f1a3d5b7c9e0f2a4d6b8c1e3f5a7d9b2c4e6f8a0d1b3c5e7f9a2d4b6c8",
    first_name: "Ivan",
    last_name: "Petrov",
    created_at: new Date("2026-05-07T12:00:00Z"),
    preferences: {
      language: "ru",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 15,
      received_parcels: 3
    }
  },
  {
    _id: NumberLong(8),
    login: "lucy",
    email: "lucy@example.com",
    password_hash: "8a0c2e4f6b1d3f5a7c9e0b2d4f6a8c1e3b5d7f9a2c4e6b8d0f1a3c5e7b9d2f4",
    first_name: "Lucy",
    last_name: "Wilson",
    created_at: new Date("2026-05-08T18:25:00Z"),
    preferences: {
      language: "en",
      notifications_enabled: false
    },
    statistics: {
      sent_parcels: 2,
      received_parcels: 11
    }
  },
  {
    _id: NumberLong(9),
    login: "max",
    email: "max@example.com",
    password_hash: "1d3f5a7c9e0b2d4f6a8c1e3b5d7f9a2c4e6b8d0f1a3c5e7b9d2f4a6c8e0b1d3",
    first_name: "Max",
    last_name: "Muller",
    created_at: new Date("2026-05-09T07:50:00Z"),
    preferences: {
      language: "de",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 9,
      received_parcels: 5
    }
  },
  {
    _id: NumberLong(10),
    login: "sophia",
    email: "sophia@example.com",
    password_hash: "6e8f0a2c4d6b8e1f3a5d7b9c0e2f4a6c8e0b1d3f5a7c9e2b4d6f8a1c3e5b7d9",
    first_name: "Sophia",
    last_name: "Taylor",
    created_at: new Date("2026-05-10T13:40:00Z"),
    preferences: {
      language: "en",
      notifications_enabled: true
    },
    statistics: {
      sent_parcels: 4,
      received_parcels: 8
    }
  }
]);
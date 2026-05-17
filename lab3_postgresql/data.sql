INSERT INTO users (login, email, first_name, last_name, password_hash)
VALUES
    ('mregor', 'mregor@example.com', 'Egor', 'Belousov', '8f0e2f76e22b43e285518f5c7e5a0120c5a8d1f8d26ab9dd98ec6053b4577692'),
    ('ivan', 'ivan@example.com', 'Ivan', 'Petrov', '6d5779b9b85bd4f11e44c9772e0f1b7f9c2c3df1b8f0f8d1d9c7c7d8b1e6f3a2'),
    ('anna', 'anna@example.com', 'Anna', 'Smirnova', 'ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f'),
    ('maria', 'maria@example.com', 'Maria', 'Ivanova', '4e738ca5563c06cf58a7b1fdb4b6a6b3fefc1c6b2d9d3c9b8e7f5a4c2b1d0e9f'),
    ('alex', 'alex@example.com', 'Alex', 'Sidorov', '1c8bfe8f801d79745c4631d09fff36c82d0a0d9f5c6f8b7e4d3c2b1a0f9e8d7c'),
    ('john', 'john@example.com', 'John', 'Doe', '60303ae22b9988617223a3e8e1b2e4c7d6f5a4b3c2d1e0f9a8b7c6d5e4f3a2b1'),
    ('kate', 'kate@example.com', 'Kate', 'Wilson', '5e884898da28047151d0e56f8dc6292773603d0d6aabbddccffee11223344556'),
    ('nikita', 'nikita@example.com', 'Nikita', 'Volkov', '2bb80d537b1da3e38bd30361aa855686bde0baef1f5f6d4c3b2a190817161514'),
    ('olga', 'olga@example.com', 'Olga', 'Morozova', '925d6b8f8c7a6e5d4c3b2a19081716151413121110ffeeddccbbaa9988776655'),
    ('max', 'max@example.com', 'Max', 'Kuznetsov', 'd8578edf8458ce06fbc5bb76a58c5ca4a5b6c7d8e9f00112233445566778899a');

INSERT INTO parcels (sender_id, description, weight)
VALUES
    (1, 'Books collection', 2.50),
    (2, 'Gaming keyboard', 1.20),
    (3, 'Winter clothes', 4.80),
    (4, 'Smartphone', 0.45),
    (5, 'Coffee machine', 6.70),
    (6, 'Headphones', 0.30),
    (7, 'Documents', 0.10),
    (8, 'Laptop', 2.10),
    (9, 'Shoes', 1.90),
    (10, 'Board games', 3.40);

INSERT INTO deliveries (sender_id, receiver_id, parcel_id, status, cost)
VALUES
    (1, 2, 1, 'CREATED', 15.50),
    (2, 3, 2, 'IN_TRANSIT', 22.00),
    (3, 4, 3, 'DELIVERED', 35.90),
    (4, 5, 4, 'CREATED', 12.40),
    (5, 6, 5, 'CANCELLED', 40.00),
    (6, 7, 6, 'IN_TRANSIT', 18.75),
    (7, 8, 7, 'DELIVERED', 9.99),
    (8, 9, 8, 'CREATED', 27.30),
    (9, 10, 9, 'IN_TRANSIT', 19.50),
    (10, 1, 10, 'DELIVERED', 31.80);
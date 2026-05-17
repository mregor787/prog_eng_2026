CREATE TABLE IF NOT EXISTS users (
    id BIGSERIAL PRIMARY KEY,
    login VARCHAR(64) NOT NULL UNIQUE,
    email VARCHAR(255) NOT NULL UNIQUE,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    password_hash TEXT NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS parcels (
    id BIGSERIAL PRIMARY KEY,
    sender_id BIGINT NOT NULL,
    description TEXT NOT NULL,
    weight NUMERIC(10, 2) NOT NULL CHECK (weight > 0),
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT fk_parcels_sender
        FOREIGN KEY (sender_id)
        REFERENCES users(id)
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS deliveries (
    id BIGSERIAL PRIMARY KEY,
    sender_id BIGINT NOT NULL,
    receiver_id BIGINT NOT NULL,
    parcel_id BIGINT NOT NULL UNIQUE,
    status VARCHAR(32) NOT NULL DEFAULT 'CREATED',
    cost NUMERIC(10, 2),
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),

    CONSTRAINT fk_deliveries_sender
        FOREIGN KEY (sender_id)
        REFERENCES users(id)
        ON DELETE CASCADE,

    CONSTRAINT fk_deliveries_receiver
        FOREIGN KEY (receiver_id)
        REFERENCES users(id)
        ON DELETE CASCADE,

    CONSTRAINT fk_deliveries_parcel
        FOREIGN KEY (parcel_id)
        REFERENCES parcels(id)
        ON DELETE CASCADE
);

-- SELECT * FROM users WHERE login = ?
CREATE INDEX IF NOT EXISTS idx_users_login
    ON users(login);

-- SELECT * FROM users WHERE first_name LIKE '%abc%'...
CREATE INDEX IF NOT EXISTS idx_users_name
    ON users(first_name, last_name);

-- SELECT * FROM parcels WHERE sender_id = ?
CREATE INDEX IF NOT EXISTS idx_parcels_sender_id
    ON parcels(sender_id);

-- SELECT * FROM parcels ORDER BY created_at DESC
CREATE INDEX IF NOT EXISTS idx_parcels_created_at
    ON parcels(created_at);

-- SELECT * FROM deliveries WHERE sender_id = ?
CREATE INDEX IF NOT EXISTS idx_deliveries_sender_id
    ON deliveries(sender_id);

-- SELECT * FROM deliveries WHERE receiver_id = ?
CREATE INDEX IF NOT EXISTS idx_deliveries_receiver_id
    ON deliveries(receiver_id);

-- SELECT * FROM deliveries WHERE status = ?
CREATE INDEX IF NOT EXISTS idx_deliveries_status
    ON deliveries(status);

-- SELECT * FROM deliveries ORDER BY created_at DESC
CREATE INDEX IF NOT EXISTS idx_deliveries_created_at
    ON deliveries(created_at);
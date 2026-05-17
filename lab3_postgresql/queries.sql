-- Create a user
INSERT INTO users (login, email, first_name, last_name, password_hash)
VALUES ($1, $2, $3, $4, $5) RETURNING id, login, email, first_name, last_name, password_hash, created_at

-- Get user by login
SELECT id, login, email, first_name, last_name, password_hash, created_at 
FROM users WHERE login = $1

-- Get user by id
SELECT id, login, email, first_name, last_name, password_hash, created_at 
FROM users WHERE id = $1

-- Search users by name/surname mask
SELECT id, login, email, first_name, last_name, password_hash, created_at 
FROM users WHERE first_name LIKE $1 OR last_name LIKE $2 OR first_name || ' ' || last_name LIKE $3

-- Create a parcel
INSERT INTO parcels (sender_id, description, weight)
VALUES ($1, $2, $3) RETURNING id, sender_id, description, weight, created_at

-- Get parcel by id
SELECT id, sender_id, description, weight, created_at 
FROM parcels WHERE id = $1

-- Get user parcels
SELECT id, sender_id, full_name, description, weight, created_at 
FROM parcels WHERE sender_id = $1 ORDER BY created_at DESC

-- Create a delivery
INSERT INTO deliveries (sender_id, receiver_id, parcel_id, status, cost)
VALUES ($1, $2, $3, $4, $5) RETURNING id, sender_id, receiver_id, parcel_id, status, cost, created_at

-- Get delivery by id
SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at
FROM deliveries WHERE id = $1

-- Get deliveries by sender
SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at 
FROM deliveries WHERE sender_id = $1 ORDER BY created_at DESC

-- Get deliveries by receiver
SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at
FROM deliveries WHERE receiver_id = $1 ORDER BY created_at DESC
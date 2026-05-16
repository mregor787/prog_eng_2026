#include "db.hpp"

#include <userver/crypto/hash.hpp>
#include <sstream>
#include <ctime>
#include <iostream>
#include <iomanip>

namespace delivery_service {

std::chrono::system_clock::time_point ParseDateTime(const std::string& datetime_str) {
    if (datetime_str.empty()) {
        return std::chrono::system_clock::now();
    }
    
    std::tm tm = {};
    std::istringstream ss(datetime_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    
    if (ss.fail()) {
        return std::chrono::system_clock::now();
    }
    
    std::time_t time_t_value = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t_value);
}

Database::Database(const std::string& db_path) : db_(nullptr) {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Cannot open database: " + std::string(sqlite3_errmsg(db_)));
    }
    
    char* err_msg = nullptr;
    rc = sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string error = err_msg ? err_msg : "Unknown error";
        sqlite3_free(err_msg);
        std::cerr << "Failed to enable WAL mode: " << error << std::endl;
    }
    
    sqlite3_busy_timeout(db_, 5000);
    
    InitSchema();
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

void Database::InitSchema() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            login TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            password_hash TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS parcels (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sender_id INTEGER NOT NULL,
            description TEXT NOT NULL,
            weight REAL NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,

            FOREIGN KEY (sender_id) 
                REFERENCES users(id) 
                ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS deliveries (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sender_id INTEGER NOT NULL,
            receiver_id INTEGER NOT NULL,
            parcel_id INTEGER NOT NULL,
            status TEXT DEFAULT 'CREATED',
            cost REAL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (sender_id) 
                REFERENCES users(id),
            FOREIGN KEY (received_id) 
                REFERENCES users(id),
            FOREIGN KEY (parcel_id)
                REFERENCES parcels(id)
        );

        CREATE INDEX IF NOT EXISTS idx_users_login ON users(login);
        CREATE INDEX IF NOT EXISTS idx_parcels_sender_id ON parcels(sender_id);
        CREATE INDEX IF NOT EXISTS idx_deliveries_sender_id ON deliveries(sender_id);
        CREATE INDEX IF NOT EXISTS idx_deliveries_receiver_id ON deliveries(receiver_id);
    )";
    
    ExecuteSQL(sql);
}

bool Database::ExecuteSQL(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::string error = err_msg ? err_msg : "Unknown error";
        sqlite3_free(err_msg);
        std::cerr << "SQL Error: " << error << std::endl;
        return false;
    }
    return true;
}

std::optional<int64_t> Database::GetLastInsertId() {
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, "SELECT last_insert_rowid()", -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    std::optional<int64_t> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int64(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::string Database::HashPassword(const std::string& password) const {
    return userver::crypto::hash::Sha256(password);
}

bool Database::VerifyPassword(const std::string& password, const std::string& hash) const {
    return HashPassword(password) == hash;
}

std::optional<User> Database::CreateUser(const CreateUserRequest& request) {
    const char* sql = "INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    
    std::string password_hash = HashPassword(request.password);
    sqlite3_bind_text(stmt, 1, request.login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, request.email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, request.first_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, request.last_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, password_hash.c_str(), -1, SQLITE_STATIC);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return std::nullopt;
    
    auto id = GetLastInsertId();
    return id ? GetUserById(*id) : std::nullopt;
}

std::optional<User> Database::GetUserByLogin(const std::string& login) {
    const char* sql = "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE login = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    
    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<User> Database::GetUserById(int64_t id) {
    const char* sql = "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, id);
    
    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<User> Database::SearchUsersByQuery(const std::string& query) {
    std::string search = "%" + query + "%";
    const char* sql = 
        "SELECT id, login, email, first_name, last_name, password_hash, created_at FROM users WHERE "
        "first_name LIKE ? OR last_name LIKE ? OR first_name || ' ' || last_name LIKE ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    sqlite3_bind_text(stmt, 1, search.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, search.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, search.c_str(), -1, SQLITE_STATIC);
    
    std::vector<User> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.first_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        u.last_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        u.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        results.push_back(u);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::optional<User> Database::AuthenticateUser(const std::string& login, const std::string& password) {
    auto u = GetUserByLogin(login);
    if (!u || !VerifyPassword(password, u->password_hash)) return {};
    return u;
}

std::optional<Parcel> Database::CreateParcel(int64_t sender_id, const CreateParcelRequest& request) {
    const char* sql = "INSERT INTO parcels (sender_id, description, weight) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, sender_id);
    sqlite3_bind_text(stmt, 2, request.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, request.weight);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return std::nullopt;
    
    auto id = GetLastInsertId();
    return id ? GetParcelById(*id) : std::nullopt;
}

std::optional<Parcel> Database::GetParcelById(int64_t parcel_id) {
    const char* sql = "SELECT id, sender_id, description, weight, created_at FROM parcels WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, parcel_id);
    std::optional<Parcel> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Parcel p;
        p.id = sqlite3_column_int64(stmt, 0);
        p.sender_id = sqlite3_column_int64(stmt, 1);
        p.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        p.weight = sqlite3_column_double(stmt, 3);
        p.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        result = p;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Parcel> Database::GetUserParcels(int64_t user_id) {
    const char* sql = "SELECT id, sender_id, full_name, description, weight, created_at FROM parcels WHERE sender_id = ? ORDER BY created_at DESC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    sqlite3_bind_int64(stmt, 1, user_id);
    std::vector<Parcel> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Parcel p;
        p.id = sqlite3_column_int64(stmt, 0);
        p.sender_id = sqlite3_column_int64(stmt, 1);
        p.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        p.weight = sqlite3_column_double(stmt, 3);
        p.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        results.push_back(p);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::optional<Delivery> Database::CreateDelivery(int64_t sender_id, const CreateDeliveryRequest& request) {
    const char* sql = "INSERT INTO deliveries (sender_id, receiver_id, parcel_id, status, cost) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, sender_id);
    sqlite3_bind_int64(stmt, 2, request.receiver_id);
    sqlite3_bind_int64(stmt, 3, request.parcel_id);
    sqlite3_bind_text(stmt, 4, "CREATED", -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, 10.0 + (rand() % 90));
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return std::nullopt;
    
    auto id = GetLastInsertId();
    return id ? GetDeliveryById(*id) : std::nullopt;
}

std::optional<Delivery> Database::GetDeliveryById(int64_t delivery_id) {
    const char* sql = "SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at FROM deliveries WHERE id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, delivery_id);
    std::optional<Delivery> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Delivery d;
        d.id = sqlite3_column_int64(stmt, 0);
        d.sender_id = sqlite3_column_int64(stmt, 1);
        d.receiver_id = sqlite3_column_int64(stmt, 2);
        d.parcel_id = sqlite3_column_int64(stmt, 3);
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL) {
            d.cost = sqlite3_column_double(stmt, 5);
        }
        d.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        result = d;
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Delivery> Database::GetDeliveriesBySender(int64_t sender_id) {
    const char* sql = "SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at FROM deliveries WHERE sender_id = ? ORDER BY created_at DESC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    sqlite3_bind_int64(stmt, 1, sender_id);
    std::vector<Delivery> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Delivery d;
        d.id = sqlite3_column_int64(stmt, 0);
        d.sender_id = sqlite3_column_int64(stmt, 1);
        d.receiver_id = sqlite3_column_int64(stmt, 2);
        d.parcel_id = sqlite3_column_int64(stmt, 3);
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL) {
            d.cost = sqlite3_column_double(stmt, 5);
        }
        d.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        results.push_back(d);
    }
    sqlite3_finalize(stmt);
    return results;
}

std::vector<Delivery> Database::GetDeliveriesByReceiver(int64_t receiver_id) {
    const char* sql = "SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at FROM deliveries WHERE receiver_id = ? ORDER BY created_at DESC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return {};
    sqlite3_bind_int64(stmt, 1, receiver_id);
    std::vector<Delivery> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Delivery d;
        d.id = sqlite3_column_int64(stmt, 0);
        d.sender_id = sqlite3_column_int64(stmt, 1);
        d.receiver_id = sqlite3_column_int64(stmt, 2);
        d.parcel_id = sqlite3_column_int64(stmt, 3);
        d.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        if (sqlite3_column_type(stmt, 5) != SQLITE_NULL) {
            d.cost = sqlite3_column_double(stmt, 5);
        }
        d.created_at = ParseDateTime(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        results.push_back(d);
    }
    sqlite3_finalize(stmt);
    return results;
}

}
# Оптимизация запросов в PostgreSQL

В данном документе представлен анализ производительности SQL-запросов сервиса доставки, а также возможные способы их оптимизации с использованием индексов, EXPLAIN ANALYZE и партиционирования таблиц.

## Стратегия индексирования

Для оптимизации поиска и сортировки были созданы такие индексы:

```sql
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
```

### Назначение индексов

| Индекс | Назначение |
|----------|----------|
| `idx_users_login` | Поиск пользователя по логину |
| `idx_users_name` | Поиск пользователя по имени/фамилии |
| `idx_parcels_sender_id` | Получение посылок пользователя |
| `idx_parcels_created_at` | Сортировка посылок по времени создания |
| `idx_deliveries_sender_id` | Получение доставок отправителя |
| `idx_deliveries_receiver_id` | Получение доставок получателя |
| `idx_deliveries_status` | Получение доставок с заданным статусом |
| `idx_deliveries_created_at` | Сортировка доставок по времени создания |

## Анализ планов выполнения запросов

### Поиск пользователя по логину

Запрос:

```sql
EXPLAIN ANALYZE
SELECT id, login, email, first_name, last_name, password_hash, created_at 
FROM users WHERE login = 'mregor';
```

План выполнения:

```
Index Scan using idx_users_login on users
  (cost=0.15..8.17 rows=1 width=256)
  (actual time=0.012..0.013 rows=1 loops=1)
  Index Cond: ((login)::text = 'mregor'::text)

Planning Time: 0.072 ms
Execution Time: 0.028 ms
```

Вывод: используется индекс `idx_users_login`. PostgreSQL выполняет Index Scan, что обеспечивает быстрый поиск пользователя по логину за `O(log N)`. Данный запрос является критически важным для аутентификации и выполняется эффективно даже при большом количестве пользователей.

### Получение посылок пользователя

Запрос:

```sql
EXPLAIN ANALYZE
SELECT id, sender_id, description, weight, created_at
FROM parcels WHERE sender_id = 1 ORDER BY created_at DESC;
```

План выполнения:

```
Index Scan Backward using idx_parcels_created_at on parcels
  (cost=0.14..8.24 rows=5 width=128)
  (actual time=0.019..0.024 rows=5 loops=1)
  Filter: (sender_id = 1)

Planning Time: 0.083 ms
Execution Time: 0.041 ms
```

Вывод: Оптимизатор PostgreSQL использует индекс `idx_parcels_created_at` для обхода записей в обратном порядке (Index Scan Backward) с целью сразу получать данные в нужной сортировке по времени создания. Фильтрация по `sender_id` выполняется во время сканирования. На больших объемах данных возможно использование индекса `idx_parcels_sender_id` либо составного индекса (`sender_id, created_at DESC`).

### Получение доставок по получателю

Запрос:

```sql
EXPLAIN ANALYZE
SELECT id, sender_id, receiver_id, parcel_id, status, cost, created_at
FROM deliveries WHERE receiver_id = 2 ORDER BY created_at DESC;
```

План выполнения:

```
Index Scan using idx_deliveries_receiver_id on deliveries
  (cost=0.15..12.32 rows=7 width=144)
  (actual time=0.016..0.022 rows=7 loops=1)
  Index Cond: (receiver_id = 2)

Planning Time: 0.091 ms
Execution Time: 0.039 ms
```

Вывод: Используется индекс `idx_deliveries_receiver_id`, что позволяет быстро находить все доставки конкретного получателя без полного сканирования таблицы. Однако сортировка по `created_at DESC` может потребовать дополнительной обработки. Для дальнейшей оптимизации рекомендуется использовать составной индекс (`receiver_id, created_at DESC`).

## Партиционирование

Таблица `deliveries` является потенциально самой быстрорастущей, так как каждая доставка создает новую запись, история доставок обычно хранится долго, количество записей о доставках увеличивается крайне быстро.

### Выбранная стратегия

Партиционирование по времени (`created_at`), например, по месяцам.

Пример:

```sql
CREATE TABLE deliveries (
    id BIGSERIAL,
    sender_id BIGINT NOT NULL,
    receiver_id BIGINT NOT NULL,
    parcel_id BIGINT NOT NULL,
    status TEXT NOT NULL,
    cost DOUBLE PRECISION,
    created_at TIMESTAMP NOT NULL
) PARTITION BY RANGE (created_at);
```

Разделы:

```sql
CREATE TABLE deliveries_2026_01 PARTITION OF deliveries
FOR VALUES FROM ('2026-01-01') TO ('2026-02-01');

CREATE TABLE deliveries_2026_02 PARTITION OF deliveries
FOR VALUES FROM ('2026-02-01') TO ('2026-03-01');
```

### Преимущества стратегии

- Быстрая очистка старых данных (вместо `DELETE FROM` используется `DROP TABLE`);
- Partition Pruning (PostgreSQL читает только нужные разделы таблицы);
- Масштабируемость (разделы можно хранить на разных дисках, tablespace, с разными параметрами хранения).

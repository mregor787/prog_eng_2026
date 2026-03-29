# Лабораторная работа №2 "Разработка REST API сервиса"

**Выполнил:** Белоусов Е.Л., группа М8О-107СВ-25

## Описание проекта

В ходе данной ЛР был реализован REST API сервис для системы доставки.

Сервис позволяет:

* управлять пользователями
* создавать и получать посылки
* создавать и отслеживать доставки между пользователями

Реализация выполнена на языке **C++** с использованием фреймворка **userver**.

## Архитектура

На текущем этапе реализован **монолитный сервис**, включающий:

* User API
* Parcel API
* Delivery API

Код проекта разделен по модулям:

```
src/
  user/
    handlers/
    storage/
    model/
  parcel/
    handlers/
    storage/
    model/
  delivery/
    handlers/
    storage/
    model/
```

## Запуск проекта

### Требования

* Docker
* Docker Compose

### Сборка и запуск

```bash
docker compose build --no-cache
docker compose up
```

Сервис будет доступен по адресу:

```
http://localhost:8080
```

## Реализованные API endpoints

### User API

#### Создание пользователя

```http
POST /users
```

Request:

```json
{
  "login": "john",
  "first_name": "John",
  "last_name": "Doe"
}
```

Response:

```json
{
  "id": "123",
  "login": "john",
  "first_name": "John",
  "last_name": "Doe"
}
```

#### Получение пользователя по логину

```http
GET /users/{login}
```

Response:

```json
{
  "id": "123",
  "login": "john",
  "first_name": "John",
  "last_name": "Doe"
}
```

#### Поиск пользователей (по маске имени/фамилии)

```http
GET /users?name=John
```

Response:

```json
[
  {
    "id": "123",
    "login": "john",
    "first_name": "John",
    "last_name": "Doe"
  }
]
```

### Parcel API

#### Создание посылки

```http
POST /parcels
```

Request:

```json
{
  "sender_id": "1",
  "description": "Books"
}
```

Response:

```json
{
  "id": "10",
  "sender_id": "1",
  "description": "Books"
}
```

#### Получение посылок пользователя

```http
GET /parcels?user_id=1
```

Response:

```json
[
  {
    "id": "10",
    "sender_id": "1",
    "description": "Books"
  }
]
```

### Delivery API

#### Создание доставки

```http
POST /deliveries
```

Request:

```json
{
  "receiver_id": "2",
  "parcel_id": "10"
}
```

Response:

```json
{
  "id": "100",
  "sender_id": "1",
  "receiver_id": "2",
  "parcel_id": "10",
  "status": "created"
}
```

#### Получение доставок по отправителю

```http
GET /deliveries?sender_id=1
```

Response:
```json
[
  {
    "id": "100",
    "sender_id": "1",
    "receiver_id": "2",
    "parcel_id": "10",
    "status": "created"
  }
]
```

#### Получение доставок по получателю

```http
GET /deliveries?receiver_id=2
```

Response:
```json
[
  {
    "id": "100",
    "sender_id": "1",
    "receiver_id": "2",
    "parcel_id": "10",
    "status": "created"
  }
]
```

## Особенности реализации

* Используется **in-memory хранилище** (std::unordered_map / std::vector)
* Архитектура построена на **userver components**
* Каждый **storage** реализован как отдельный компонент
* Используются DTO (структуры моделей)

## Обработка ошибок

* 404 — ресурс не найден
* 409 — конфликт (например, пользователь уже существует)
* 400 — некорректный запрос (частично)

## Тестирование

Пока используется ручное тестирование через curl.

Пример:

```bash
curl -X POST http://localhost:8080/users \
  -H "Content-Type: application/json" \
  -d '{"login":"john","first_name":"John","last_name":"Doe"}'
```

## OpenAPI

Файл спецификации: [openapi.yaml](openapi.yaml)


## Docker

Проект запускается в контейнере:

* используется prebuilt образ userver
* сборка через CMake внутри контейнера

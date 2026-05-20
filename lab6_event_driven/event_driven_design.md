# Проектирование Event-Driven архитектуры для Delivery Service

## Обзор
В данном документе описывается событийно‑ориентированная архитектура (EDA) для сервиса доставки. Система разбита на микросервисы `user-service`, `parcel-service` и `delivery-service`, а взаимодействие между ними осуществляется через RabbitMQ для обеспечения слабой связанности сервисов, масштабируемости и асинхронной обработки событий.

На текущем этапе реализован обмен событиями между `parcel-service` и `delivery-service`.

## Компоненты

### Event Producers
- `parcel-service`: Публикует событие `ParcelCreated`.
- `delivery-service`: Потенциально может публиковать `DeliveryCreated`, `DeliveryCompleted`, `DeliveryCancelled`.
- `user-service`: Потенциально может публиковать `UserRegistered`.

### Event Consumers
- `delivery-service`: Потребляет `ParcelCreated` для логирования и последующей синхронизации информации о новых посылках.
- `parcel-service`: В будущем может потреблять события о статусах доставки.
- `user-service`: В будущем может потреблять события о завершении доставки для обновления статистики пользователя.

## Поток событий

### Создание посылки

1. Пользователь отправляет запрос `POST /api/v1/parcels`;
2. `parcel-service`:

- сохраняет посылку в PostgreSQL;
- инвалидирует Redis-кэш;
- публикует событие `ParcelCreated` в RabbitMQ.

3. `delivery-service`:

- получает событие из очереди;
- обрабатывает payload;
- логирует информацию о новой посылке.

## CQRS

В системе возможно применение паттерна CQRS (Command Query Responsibility Segregation).

### Write Model

Write-модель отвечает за обработку команд:

- `CreateParcel`
- `CreateDelivery`
- `RegisterUser`
- `LoginUser`

После успешного изменения состояния сервис публикует event в RabbitMQ.

### Read Model

Read-модель отвечает за запросы:

- `GetUserParcels`
- `GetDeliveriesBySender`
- `GetDeliveriesByReceiver`
- `SearchUsers`

Для ускорения чтения используются:

- PostgreSQL read-запросы;
- Redis-кэш.

В дальнейшем read-модель может быть полностью денормализована и синхронизироваться только через events.

## Message Broker (RabbitMQ)
### Broker

В качестве брокера сообщений используется RabbitMQ.

### Exchange

Используется topic exchange `delivery_events`

### Routing Keys
| Routing Key | Назначение |
|---|---|
`parcel.created` | Создание новой посылки |
`delivery.created` | Создание новой доставки |
`delivery.completed` | Завершение доставки |

### Queue

Для обработки событий о посылках используется очередь `parcel_created_queue`

## Формат сообщений

Сообщения передаются в формате JSON.

Пример события `ParcelCreated`:

```json
{
  "event_type": "ParcelCreated",
  "parcel_id": 15,
  "sender_id": 3,
  "description": "Books",
  "weight": 2.5
}
```

## Гарантии доставки

Используется стратегия доставки At-least-once.

Гарантии обеспечиваются за счет:

- подтверждений сообщений (ACK);
- сохранения сообщений брокером RabbitMQ;
- повторной доставки при отсутствии ACK.

Повторная обработка сообщений допускается и должна быть идемпотентной.

## Масштабируемость

Event-Driven архитектура позволяет:

- независимо масштабировать сервисы;
- добавлять новых consumers без изменения producers;
- выполнять асинхронную обработку;
- уменьшать связанность между сервисами.

RabbitMQ позволяет горизонтально масштабировать consumers путем запуска нескольких экземпляров обработчиков очередей.

## Отказоустойчивость

При временной недоступности consumer:

- сообщения сохраняются в очереди RabbitMQ;
- обработка продолжается после восстановления сервиса.

При недоступности RabbitMQ:

- REST API продолжает работать;
- публикация событий может быть повторена позже.
# Event Catalog

| Event Name | Producer | Consumers | Description |
|---|---|---|---|
| `ParcelCreated` | `parcel-service` | `delivery-service` | Была создана новая посылка |
|  `DeliveryCreated` | `delivery-service` | `analytics-service` | Была создана новая доставка |
| `DeliveryStatusUpdated` | `delivery-service` | `analytics-service` | Статус доставки был изменен |
| `UserLoggedIn` | `user-service` | `analytics-service` | Пользователь успешно выполнил вход в систему |

## Event Structure (Payload Examples)

### ParcelCreated

```json
{
  "event_id": "uuid",
  "event_type": "ParcelCreated",
  "timestamp": "2026-05-21T12:00:00Z",
  "payload": {
    "parcel_id": 15,
    "sender_id": 3,
    "description": "Books",
    "weight": 2.5
  }
}
```

- Producer: `parcel-service`
- Consumers: `delivery-service`
- Routing key: `parcel.created`
- Delivery guarantee: `at least once`

### DeliveryCreated

```json
{
  "event_id": "uuid",
  "event_type": "DeliveryCreated",
  "timestamp": "2026-05-21T12:10:00Z",
  "payload": {
    "delivery_id": 42,
    "sender_id": 3,
    "receiver_id": 7,
    "parcel_id": 15,
    "status": "CREATED",
    "cost": 12.5
  }
}
```

- Producer: `delivery-service`
- Consumers: `analytics-service`
- Routing key: `delivery.created`
- Delivery guarantee: `at-least-once`

### DeliveryStatusUpdated

```json
{
  "event_id": "uuid",
  "event_type": "DeliveryStatusUpdated",
  "timestamp": "2026-05-21T13:00:00Z",
  "payload": {
    "delivery_id": 42,
    "old_status": "CREATED",
    "new_status": "DELIVERED"
  }
}
```

- Producer: `delivery-service`
- Consumers: `analytics-service`
- Routing key: `delivery.updated`
- Delivery guarantee: `at-least-once`

### UserLoggedIn

```json
{
  "event_id": "uuid",
  "event_type": "UserLoggedIn",
  "timestamp": "2026-05-21T13:15:00Z",
  "payload": {
    "user_id": 3,
    "login": "john_doe"
  }
}
```

- Producer: `user-service`
- Consumers: `analytics-service`
- Routing key: `user.logged_in`
- Delivery guarantee: `at-least-once`
# Skell's GreenHouse V3.0

Aplicacion movil para monitoreo y configuracion de un invernadero autonomo con IA mediante la API HTTP del ESP32.

## API esperada

- `GET /api/datos`: devuelve sensores, estado del ESP32 y salidas de IA.
- `GET /api/foto`: devuelve una fotografia `image/jpeg`.
- `POST /api/perfil`: recibe un numero plano `1`, `2` o `3`.
- `POST /api/wifi`: recibe `{ "red": "NombreRed", "psw": "Contrasena" }`.

## Desarrollo con Bun

```bash
bun install
bun run dev
```

## Build web

```bash
bun run build
```

## Android con Capacitor

```bash
bun run cap:add:android
bun run cap:sync
bun run cap:open
```

La configuracion de Capacitor permite trafico HTTP local (`cleartext`) para acceder al ESP32 por IP, por ejemplo `http://192.168.1.50`.

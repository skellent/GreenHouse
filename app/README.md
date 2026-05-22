# GreenHouse V3.0

Aplicacion movil para monitoreo y configuracion de un invernadero autonomo con IA mediante la API HTTP del ESP32.

## Manual de uso

1. Abre la app y entra en `Configurar`.
2. En `Direccion API`, escribe la IP del ESP32, por ejemplo `http://192.168.1.50`, y pulsa `Guardar`.
3. Activa `Auto` para que la app consulte `/api/datos` automaticamente. Puedes elegir un intervalo de `1 segundo` o `2 segundos`.
4. En `Monitoreo`, revisa temperatura, humedad, luz, ultrasonido, estado del ESP32 y decisiones de IA.
5. Activa `Registrar historial` cuando quieras que cada lectura automatica se guarde localmente. Tambien puedes pulsar `Guardar lectura` para almacenar una muestra puntual.
6. En `Historial`, revisa las lecturas recientes, exportalas como CSV o limpia el registro.
7. En `Camara`, pulsa capturar para consultar `/api/foto`; luego puedes descargar la imagen.
8. En `Configurar`, cambia el perfil de planta:
   - `Desertico`: condiciones secas y alta tolerancia a calor.
   - `Tropical`: mayor humedad y temperatura estable.
   - `Alpino`: perfil frio para plantas adaptadas a montana.
9. En `WiFi`, envia una nueva red y contrasena al ESP32 mediante `/api/wifi`. La placa responde antes de reconectarse; espera unos 15 segundos.
10. Puedes cambiar tema (`Sistema`, `Claro`, `Oscuro`) e idioma (`Español`, `English`, `Русский`, `Deutsch`, `中文`) desde la app.

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

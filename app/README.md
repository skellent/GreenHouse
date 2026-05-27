# GreenHouse V3.0

Aplicacion movil vanilla HTML/CSS/JS empaquetada con Capacitor para monitorear y configurar el invernadero autonomo Skell's GreenHouse desde Android o navegador local.

## Resumen

GreenHouse se conecta a la API HTTP del ESP32-S3, consulta sensores ambientales, muestra resultados de IA, captura fotografias de la camara, permite cambiar perfiles de planta, configura WiFi y guarda historial local exportable en CSV.

La app web vive en `www/` y no depende de TypeScript ni de frameworks de UI. Capacitor copia esos archivos directamente al WebView y expone sus plugins mediante `window.Capacitor`.

## Manual de usuario

### 1. Primer inicio

1. Instala o abre la aplicacion `GreenHouse`.
2. Entra a `Configurar`.
3. En `Direccion API`, escribe la direccion del ESP32-S3. Puedes usar `http://192.168.1.50` o solo `192.168.1.50`.
4. Pulsa `Guardar`.
5. Vuelve a `Monitoreo` y espera la primera lectura.

Si la app muestra `Sin conexion con la API`, confirma que el telefono este en la misma red que el ESP32-S3 y que la IP sea correcta.

### 2. Monitoreo

La pantalla `Monitoreo` consulta `GET /api/datos` y muestra:

- Temperatura ambiente.
- Temperatura del agua.
- Humedad ambiente.
- Humedad del suelo.
- Intensidad de luz.
- Altura, tomada desde el campo `sensores.ultrasonido`.
- Temperatura del ESP32.
- Estado de la camara.
- Intensidad UV calculada por IA.
- Estado de riego.
- Estado de ventilacion.
- Perfil de planta activo.

El boton de actualizar fuerza una lectura manual. La animacion de actualizacion solo afecta a los valores, no a los paneles completos.

### 3. Actualizacion automatica

En `Configurar`, activa `Auto` para que la app consulte el invernadero de forma continua. Puedes elegir:

- `1 segundo`: mas inmediato, ideal para pruebas cercanas al router.
- `2 segundos`: recomendado para uso diario, mas amable con la red y el ESP32-S3.

### 4. Historial

La opcion `Registrar historial` decide si cada lectura correcta se guarda localmente.

- Activada: cada respuesta valida de `/api/datos` se agrega al historial.
- Desactivada: la app sigue monitoreando, pero no guarda muestras automaticamente.
- `Guardar lectura`: almacena manualmente la lectura actual.
- `Limpiar`: borra el historial local.
- `Exportar CSV`: guarda un archivo `.csv`.

El CSV incluye todos los datos que entrega la API actualmente:

- Todos los sensores.
- Estado del ESP32.
- Todos los resultados de IA.
- Perfil numerico y nombre del perfil.
- Una columna `raw_json` con la respuesta completa para conservar cualquier campo adicional.

### 5. Camara

La pantalla `Camara` consulta `GET /api/foto`.

Por un bug observado en la API del ESP32-S3, la app hace dos solicitudes seguidas al capturar una foto. La primera descarta un posible frame cacheado y la segunda se usa como imagen visible. Esto evita que el usuario tenga que tocar dos veces el boton para ver una foto en vivo.

El boton de descarga guarda la imagen:

- En Android: en Documentos mediante el plugin Filesystem de Capacitor.
- En navegador: como descarga normal del navegador.

### 6. Perfiles de planta

En `Configurar`, el selector de perfil envia un numero plano a `POST /api/perfil`:

- `Desertico`: envia `1`.
- `Tropical`: envia `2`.
- `Alpino`: envia `3`, pensado para plantas de climas frios o de montana.

### 7. WiFi

En `Configurar`, la seccion WiFi envia a `POST /api/wifi`:

```json
{ "red": "NombreRed", "psw": "Contrasena" }
```

El ESP32-S3 responde antes de intentar reconectarse para no cortar la respuesta HTTP. Luego espera unos 15 segundos y consulta la nueva IP desde tu router o pantalla del dispositivo.

### 8. Tema e idioma

La app permite tema:

- `Sistema`
- `Claro`
- `Oscuro`

Tambien permite cambiar el idioma de la interfaz entre Espanol e Ingles en esta version vanilla.

### 9. Creditos y ayuda dentro de la app

La pestana `Creditos` contiene:

- Integrantes del proyecto.
- Una descripcion corta del objetivo del invernadero.
- Un manual de uso resumido para consulta rapida sin salir de la app.

## API esperada

- `GET /api/datos`: devuelve sensores, estado del ESP32 y resultados de IA.
- `GET /api/foto`: devuelve una fotografia `image/jpeg`.
- `POST /api/perfil`: recibe un numero plano `1`, `2` o `3`.
- `POST /api/wifi`: recibe `{ "red": "NombreRed", "psw": "Contrasena" }`.

## Desarrollo con Bun

```bash
bun install
bun run dev
```

El servidor local sirve `www/` en `http://127.0.0.1:5173`.

## Verificacion

```bash
bun run build
```

El comando valida que existan los archivos estaticos requeridos.

## Android con Capacitor

```bash
bun run cap:sync
bun run cap:open
```

El paquete Android es `dev.skellent.greenhouse` y el nombre visible de la aplicacion es `GreenHouse`.

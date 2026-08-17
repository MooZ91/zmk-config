# Configuración ZMK para MB9i

Este repositorio contiene la configuración de firmware ZMK para el **MB9i**, un teclado numérico compacto de 3x3.

## 🛠 Características

- **Matriz 3x3**: Diseño compacto optimizado para entrada numérica.
- **Soporte para ZMK Studio**: Integración con [ZMK Studio](https://zmk.dev/docs/studio), que permite ajustar el keymap en tiempo real sin necesidad de reflashear.
- **Conectividad Bluetooth**: Compilado para el controlador `nice_nano_v2`.
- **Soporte multi-capa**:
  - **Capa por defecto**: Distribución estándar de teclado numérico.
  - **Capa BT**: Gestión de perfiles Bluetooth y controles del sistema (Bootloader, desbloqueo de Studio).
- **LEDs de estado**: Dos LEDs ARGB que indican el nivel de batería y la conexión activa.
- **Confirmación visual**: Cada pulsación dispara un destello, con un color distinto según la capa activa.

## 🚀 Especificaciones de hardware

- **Controlador**: `nice_nano_v2`
- **Distribución**: Matriz 3x3 (9 teclas)
- **Dirección de diodos**: Columna a fila
- **Iluminación**: 2 LEDs ARGB (WS2812 / SK6812) en cadena

## 🔌 Asignación de pines

La matriz de teclado usa `diode-direction = col2row` (los diodos van de columna a fila) y se conecta al `nice_nano_v2` de la siguiente manera:

### Filas

| Fila | Pin GPIO |
|------|----------|
| Fila 0 | P0.10 |
| Fila 1 | P1.11 |
| Fila 2 | P0.09 |

### Columnas

| Columna | Pin GPIO |
|---------|----------|
| Columna 0 | P0.29 |
| Columna 1 | P0.11 |
| Columna 2 | P1.04 |

### LEDs ARGB

La cadena de LEDs direccionables se controla por SPI (`spi3`), que es la forma soportada de generar el timing del protocolo WS2812 en el nRF52840. Solo se declara la línea MOSI, porque el LED no usa reloj:

| Señal | Pin GPIO |
|-------|----------|
| Datos (MOSI) | P0.17 |

- **Cantidad**: 2 LEDs en cadena
- **Orden de color**: GRB
- **Nota**: P0.17 es el pad `SDA` de la `nice!nano`, así que ese pin queda ocupado para I2C.

### Mapeo de teclas físicas

| Tecla | Posición (fila, columna) |
|-------|---------------------------|
| 1 | (0, 0) |
| 2 | (0, 1) |
| 3 | (0, 2) |
| 4 | (1, 0) |
| 5 | (1, 1) |
| 6 | (1, 2) |
| 7 | (2, 0) |
| 8 | (2, 1) |
| 9 | (2, 2) |

## ⌨️ Resumen del keymap

### Capa por defecto
Una configuración estándar de teclado numérico para una entrada de datos eficiente.

### Capa de Bluetooth y sistema
Se usa para gestionar las conexiones inalámbricas y acceder a utilidades del firmware:
- Selección y borrado de perfiles Bluetooth.
- Acceso al bootloader.
- Desbloqueo de ZMK Studio.

## 💡 LEDs de estado

Los dos LEDs ARGB funcionan como indicadores y no como iluminación decorativa. La lógica vive en un módulo Zephyr propio, dentro de `module/`.

### LED 1 — Nivel de batería

| Carga | Color |
|-------|-------|
| Más del 50% | Verde |
| Entre 20% y 50% | Amarillo |
| Entre 10% y 20% | Rojo |
| Menos del 10% | Rojo parpadeante |

### LED 2 — Conexión activa

| Estado | Color |
|--------|-------|
| USB | Blanco |
| Bluetooth conectado | Azul |
| Emparejando | Azul parpadeante |
| Bluetooth desconectado | Magenta |

### Destello de confirmación

Cada pulsación dispara un destello doble sobre el LED 2, como confirmación de que el comando salió. El LED de batería no se interrumpe, así que la carga sigue siendo legible por rápido que escribas:

| Capa activa | Color del destello |
|-------------|--------------------|
| Capa por defecto | Cian |
| Capa BT | Violeta |

Ambos LEDs se apagan cuando el teclado entra en reposo, para no drenar la batería.

### Ajustes

Los valores se cambian desde `config/boards/shields/mb9i/mb9i.conf`:

| Opción | Por defecto | Qué controla |
|--------|-------------|--------------|
| `CONFIG_MB9I_STATUS_LEDS_BRIGHTNESS` | 50 | Brillo de los LEDs, en porcentaje |
| `CONFIG_MB9I_STATUS_LEDS_BATT_HIGH` | 50 | Umbral por encima del cual la batería se muestra en verde |
| `CONFIG_MB9I_STATUS_LEDS_BATT_LOW` | 20 | Umbral por encima del cual se muestra en amarillo |
| `CONFIG_MB9I_STATUS_LEDS_BATT_CRITICAL` | 10 | Umbral por debajo del cual el rojo parpadea |
| `CONFIG_MB9I_STATUS_LEDS_FLASH_MS` | 60 | Duración de cada paso del destello |
| `CONFIG_MB9I_STATUS_LEDS_BLINK_MS` | 1000 | Período del parpadeo |

## 📦 Compilación e instalación

Este proyecto está configurado para compilarse mediante GitHub Actions. Puedes usar la configuración por defecto o personalizarla según tus necesidades.

El repositorio incluye un módulo Zephyr propio en `module/`, que `build.yaml` inyecta con `ZMK_EXTRA_MODULES`. No hace falta ningún paso extra: la compilación lo toma automáticamente.

### Opción 1: Usar el firmware por defecto
Si no necesitas un keymap personalizado, puedes descargar el firmware ya compilado directamente desde este repositorio:
1. Ve a la pestaña **Actions** en la parte superior de este repositorio.
2. Selecciona la ejecución exitosa más reciente (normalmente llamada "Build").
3. Baja hasta la sección **Artifacts**.
4. Descarga el artefacto `firmware`, extrae el archivo `.uf2` y flashéalo a tu `nice_nano_v2`.

### Opción 2: Personalizar y compilar tu propia versión
Si quieres cambiar el keymap o la configuración:
1. Haz un fork de este repositorio a tu propia cuenta de GitHub.
2. Personaliza tu archivo `mb9i.keymap`.
3. Haz commit y push de tus cambios para disparar una nueva compilación.
4. Descarga tu firmware `.uf2` personalizado desde la pestaña **Actions** de *tu* repositorio forkeado:
    - Selecciona la ejecución exitosa más reciente.
    - Baja hasta la sección **Artifacts**.
    - Descarga el artefacto `firmware` y extrae el archivo `.uf2`.
5. Flashea el firmware a tu controlador `nice_nano_v2`.

## 📄 Licencia

Este proyecto está licenciado bajo la Licencia MIT.

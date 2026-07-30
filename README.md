# Configuración ZMK para MB9i

Este repositorio contiene la configuración de firmware ZMK para el **MB9i**, un teclado numérico compacto de 3x3.

## 🛠 Características

- **Matriz 3x3**: Diseño compacto optimizado para entrada numérica.
- **Soporte para ZMK Studio**: Integración con [ZMK Studio](https://zmk.dev/docs/studio), que permite ajustar el keymap en tiempo real sin necesidad de reflashear.
- **Conectividad Bluetooth**: Compilado para el controlador `nice_nano_v2`.
- **Soporte multi-capa**:
  - **Capa por defecto**: Distribución estándar de teclado numérico.
  - **Capa BT**: Gestión de perfiles Bluetooth y controles del sistema (Bootloader, desbloqueo de Studio).

## 🚀 Especificaciones de hardware

- **Controlador**: `nice_nano_v2`
- **Distribución**: Matriz 3x3 (9 teclas)
- **Dirección de diodos**: Columna a fila

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

## 📦 Compilación e instalación

Este proyecto está configurado para compilarse mediante GitHub Actions. Puedes usar la configuración por defecto o personalizarla según tus necesidades.

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

/*
 * LEDs de estado del MB9i.
 *
 * LED 0 - nivel de batería:  verde >50%, amarillo 20-50%, rojo 10-20%,
 *                            rojo parpadeante <10%.
 * LED 1 - conexión activa:   blanco USB, azul BLE conectado,
 *                            azul parpadeante emparejando, magenta BLE caído.
 *
 * Ambos se apagan cuando el teclado sale del estado activo (idle/sleep).
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/activity.h>
#include <zmk/battery.h>
#include <zmk/endpoints.h>
#include <zmk/endpoints_types.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/endpoint_changed.h>

#if IS_ENABLED(CONFIG_ZMK_BLE)
#include <zmk/ble.h>
#include <zmk/events/ble_active_profile_changed.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_USB)
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>
#endif

#include <mb9i_status_leds.h>

#if IS_ENABLED(CONFIG_ZMK_RGB_UNDERGLOW)
#error "MB9I_STATUS_LEDS requiere ZMK_RGB_UNDERGLOW=n: el underglow reescribe todos los pixeles"
#endif

LOG_MODULE_REGISTER(mb9i_status_leds, CONFIG_ZMK_LOG_LEVEL);

#define STRIP_NODE DT_CHOSEN(zmk_underglow)
#define STRIP_LEN DT_PROP(STRIP_NODE, chain_length)

BUILD_ASSERT(STRIP_LEN >= 2, "Los LEDs de estado necesitan una cadena de al menos 2 LEDs");

static const struct device *const strip = DEVICE_DT_GET(STRIP_NODE);

/* Escala un canal 0-255 al brillo configurado. */
#define SCALE(v) ((uint8_t)((v) * CONFIG_MB9I_STATUS_LEDS_BRIGHTNESS / 100))
#define RGB(_r, _g, _b) ((struct led_rgb){.r = SCALE(_r), .g = SCALE(_g), .b = SCALE(_b)})

#define COLOR_OFF RGB(0, 0, 0)
#define COLOR_GREEN RGB(0, 255, 0)
#define COLOR_YELLOW RGB(255, 200, 0)
#define COLOR_RED RGB(255, 0, 0)
#define COLOR_BLUE RGB(0, 0, 255)
#define COLOR_MAGENTA RGB(255, 0, 255)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_CYAN RGB(0, 255, 255)

static struct led_rgb pixels[STRIP_LEN];

/* Fase compartida del parpadeo: false = apagado durante medio período. */
static bool blink_phase = true;

static void leds_update(void);

static void blink_tick(struct k_work *work) {
    blink_phase = !blink_phase;
    leds_update();
}

static K_WORK_DELAYABLE_DEFINE(blink_work, blink_tick);

/* Destello de confirmación: mientras está activo tapa el estado normal. */
static bool flashing;

static void flash_end(struct k_work *work) {
    flashing = false;
    leds_update();
}

static K_WORK_DELAYABLE_DEFINE(flash_work, flash_end);

void mb9i_status_leds_flash(void) {
    flashing = true;
    leds_update();
    k_work_reschedule(&flash_work, K_MSEC(CONFIG_MB9I_STATUS_LEDS_FLASH_MS));
}

static void first_update(struct k_work *work) { leds_update(); }

static K_WORK_DELAYABLE_DEFINE(first_update_work, first_update);

static struct led_rgb battery_color(bool *blink) {
    uint8_t soc = zmk_battery_state_of_charge();

    if (soc > CONFIG_MB9I_STATUS_LEDS_BATT_HIGH) {
        return COLOR_GREEN;
    }
    if (soc >= CONFIG_MB9I_STATUS_LEDS_BATT_LOW) {
        return COLOR_YELLOW;
    }
    if (soc >= CONFIG_MB9I_STATUS_LEDS_BATT_CRITICAL) {
        return COLOR_RED;
    }

    *blink = true;
    return COLOR_RED;
}

static struct led_rgb connection_color(bool *blink) {
#if IS_ENABLED(CONFIG_ZMK_USB)
    if (zmk_endpoints_selected().transport == ZMK_TRANSPORT_USB) {
        return COLOR_WHITE;
    }
#endif

#if IS_ENABLED(CONFIG_ZMK_BLE)
    if (zmk_ble_active_profile_is_connected()) {
        return COLOR_BLUE;
    }
    if (zmk_ble_active_profile_is_open()) {
        *blink = true;
        return COLOR_BLUE;
    }
    return COLOR_MAGENTA;
#else
    return COLOR_OFF;
#endif
}

static void leds_update(void) {
    struct led_rgb battery = COLOR_OFF;
    struct led_rgb connection = COLOR_OFF;
    bool blink_battery = false;
    bool blink_connection = false;

    if (flashing) {
        battery = COLOR_CYAN;
        connection = COLOR_CYAN;
    } else if (zmk_activity_get_state() == ZMK_ACTIVITY_ACTIVE) {
        /* Fuera del estado activo apagamos todo para no drenar la batería. */
        battery = battery_color(&blink_battery);
        connection = connection_color(&blink_connection);
    }

    if (blink_battery && !blink_phase) {
        battery = COLOR_OFF;
    }
    if (blink_connection && !blink_phase) {
        connection = COLOR_OFF;
    }

    pixels[0] = battery;
    pixels[1] = connection;

    int err = led_strip_update_rgb(strip, pixels, STRIP_LEN);
    if (err < 0) {
        LOG_ERR("No se pudo actualizar la tira de LEDs: %d", err);
        return;
    }

    if (blink_battery || blink_connection) {
        k_work_reschedule(&blink_work, K_MSEC(CONFIG_MB9I_STATUS_LEDS_BLINK_MS / 2));
    } else {
        k_work_cancel_delayable(&blink_work);
        blink_phase = true;
    }
}

static int status_leds_listener(const zmk_event_t *eh) {
    leds_update();
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(mb9i_status_leds, status_leds_listener);
ZMK_SUBSCRIPTION(mb9i_status_leds, zmk_battery_state_changed);
ZMK_SUBSCRIPTION(mb9i_status_leds, zmk_activity_state_changed);
ZMK_SUBSCRIPTION(mb9i_status_leds, zmk_endpoint_changed);
#if IS_ENABLED(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(mb9i_status_leds, zmk_ble_active_profile_changed);
#endif
#if IS_ENABLED(CONFIG_ZMK_USB)
ZMK_SUBSCRIPTION(mb9i_status_leds, zmk_usb_conn_state_changed);
#endif

static int status_leds_init(void) {
    if (!device_is_ready(strip)) {
        LOG_ERR("La tira de LEDs no está lista");
        return -ENODEV;
    }

    /* El primer reporte de batería llega después del arranque: si pintáramos
     * ahora veríamos un rojo parpadeante espurio con el 0% inicial. */
    k_work_reschedule(&first_update_work, K_SECONDS(2));

    return 0;
}

SYS_INIT(status_leds_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

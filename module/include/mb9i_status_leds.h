/*
 * API interna de los LEDs de estado del MB9i.
 */

#pragma once

/*
 * Dispara un destello doble en cian (destello, hueco oscuro, destello) como
 * confirmación visual; al terminar vuelve a mostrar batería y conexión.
 * Cada paso dura CONFIG_MB9I_STATUS_LEDS_FLASH_MS.
 * Llamarla de nuevo mientras la secuencia corre la reinicia desde cero.
 */
void mb9i_status_leds_flash(void);

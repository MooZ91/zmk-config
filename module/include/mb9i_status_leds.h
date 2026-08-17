/*
 * API interna de los LEDs de estado del MB9i.
 */

#pragma once

/*
 * Destella ambos LEDs en cian como confirmación visual y, pasados
 * CONFIG_MB9I_STATUS_LEDS_FLASH_MS, vuelve a mostrar batería y conexión.
 * Llamarla de nuevo mientras el destello está activo lo extiende.
 */
void mb9i_status_leds_flash(void);

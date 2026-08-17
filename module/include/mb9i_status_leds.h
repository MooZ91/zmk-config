/*
 * API interna de los LEDs de estado del MB9i.
 */

#pragma once

/*
 * Dispara un destello doble (destello, hueco oscuro, destello) sobre
 * el LED de conexión, como confirmación visual de "comando enviado"; al
 * terminar ese LED vuelve a mostrar el endpoint activo. El LED de batería no
 * se toca. El color depende de la capa activa: cian en la capa por defecto,
 * violeta en la capa 1. Cada paso dura CONFIG_MB9I_STATUS_LEDS_FLASH_MS.
 * Llamarla de nuevo mientras la secuencia corre la reinicia desde cero.
 */
void mb9i_status_leds_flash(void);

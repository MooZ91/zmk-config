# Se compila dentro del target `app` para heredar los includes de ZMK
# (zmk/app/include), que no se exportan a las zephyr_library de los módulos.
target_sources_ifdef(CONFIG_MB9I_STATUS_LEDS app PRIVATE src/status_leds.c)

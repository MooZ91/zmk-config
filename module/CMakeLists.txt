if(CONFIG_MB9I_STATUS_LEDS)
  zephyr_library()
  zephyr_library_sources(src/status_leds.c)
endif()

idf_component_register(SRCS "main.c" "isotp_bridge.c" "isotp.c" "ble_server.c" "ws2812_control.c" "twai.c" "persist.c" "isotp_link_containers.c" "led.c" "eeprom.c" "uart.c" "connection_handler.c"
                    INCLUDE_DIRS ".")

# Disable warnings as errors for ESP-IDF v5.x compatibility
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=format -Wno-error=maybe-uninitialized -Wno-error=implicit-function-declaration -Wno-error)

# Allow multiple definitions (legacy code has globals defined in headers)
target_link_options(${COMPONENT_LIB} INTERFACE "-Wl,--allow-multiple-definition")

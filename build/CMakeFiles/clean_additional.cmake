# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "0.p3.S"
  "1.p3.S"
  "2.p3.S"
  "3.p3.S"
  "4.p3.S"
  "5.p3.S"
  "6.p3.S"
  "7.p3.S"
  "8.p3.S"
  "9.p3.S"
  "ai_esp32s3.bin"
  "ai_esp32s3.map"
  "bootloader/bootloader.bin"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.map"
  "config/sdkconfig.cmake"
  "config/sdkconfig.h"
  "enter_ble_wifi.p3.S"
  "esp-idf/esptool_py/flasher_args.json.in"
  "esp-idf/mbedtls/x509_crt_bundle"
  "flash_app_args"
  "flash_bootloader_args"
  "flash_project_args"
  "flasher_args.json"
  "key_wake_up.p3.S"
  "ldgen_libraries"
  "ldgen_libraries.in"
  "project_elf_src_esp32s3.c"
  "success.p3.S"
  "wifi_config_ok.p3.S"
  "x509_crt_bundle.S"
  )
endif()

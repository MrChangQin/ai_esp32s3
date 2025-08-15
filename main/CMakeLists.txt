
SET(SRCS "main.cpp"
    "app.cpp"
    "work_task.cpp"
    "drivers/storage/sd_card.cpp"
    "drivers/audio/wav_recorder.cpp"
    "drivers/audio/audio_es8311_es7210.cpp"
    "drivers/audio/audio_hal.cpp"
    "drivers/audio/input_audio_process.cpp"
    "drivers/ble/ble_lib.c"
    "drivers/setting/setting.cpp"
    "drivers/audio/wake_word/wake_word_detect.cpp"
    "drivers/audio/wake_word/wake_word_hal.cpp"
    "drivers/audio/wake_word/cmd_word_detect.cpp"
    "drivers/led/one_color_led.cpp"
    "board/board.cpp"
    "board/common/wifi_board.cpp"
    "protocols/protocol_adapter.cpp"
    "protocols/websocket_protocol.cpp"

)

# 新增开发板需要在这里写一个配置BOARD_TYPE的宏为板子类型文件夹名称
if(CONFIG_BOARD_TYPE_LC_SZP_DEV)
    set(BOARD_TYPE "lc_szp_dev")
elseif(CONFIG_BOARD_TYPE_LC_SZP_C3_DEV)
    set(BOARD_TYPE "lc_szp_c3_dev")
# 新增开发板配置
endif()

file(GLOB BOARD_TYPE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/board/${BOARD_TYPE}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/board/${BOARD_TYPE}/*.c
)
list(APPEND SRCS ${BOARD_TYPE_SOURCES})

file(GLOB P3_SOUNDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.p3)


SET(INCLUDE_DIRS 
    "."
    "drivers"
    "drivers/storage"
    "drivers/audio"
    "drivers/ble"
    "drivers/setting"
    "drivers/audio/wake_word"
    "drivers/led"
    "board"
    "board/common"
    "protocols"
    "resources"
)

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS ${INCLUDE_DIRS}
    
    EMBED_FILES ${P3_SOUNDS}  #添加音频文件
)

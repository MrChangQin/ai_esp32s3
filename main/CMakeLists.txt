
SET(SRCS "main.cpp"
   "app.cpp"
   "work_task.cpp"
   "drivers/storage/sd_card.cpp"
   "drivers/audio/audio_es7210.cpp"
   "drivers/audio/wav_recorder.cpp"
   "drivers/audio/audio_es8311.cpp"
   "drivers/audio/audio_es8311_es7210.cpp"
   "drivers/audio/audio_hal.cpp"
   "board/board.cpp"
)

# 新增开发板需要在这里写一个配置BOARD_TYPE的宏为板子类型文件夹名称
if(CONFIG_BOARD_TYPE_LC_SZP_DEV)
    set(BOARD_TYPE "lc_szp_dev")
# elseif(CONFIG_BOARD_TYPE_XXX_XXX)
#     set(BOARD_TYPE "xxx-xxx")
# 新增开发板配置
endif()

file(GLOB BOARD_TYPE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/board/${BOARD_TYPE}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/board/${BOARD_TYPE}/*.c
)
list(APPEND SRCS ${BOARD_TYPE_SOURCES})


SET(INCLUDE_DIRS 
    "."
    "drivers"
    "drivers/storage"
    "drivers/audio"
    "board"
)

idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS ${INCLUDE_DIRS}
)

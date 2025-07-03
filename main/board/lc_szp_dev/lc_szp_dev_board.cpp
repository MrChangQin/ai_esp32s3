#include "lc_szp_dev_board.h"
#include "audio_es8311_es7210.h"


LcSzpDevBoard::LcSzpDevBoard()
{
    i2c0_bus = Board::init_i2c(AUDIO_I2C_NUM, AUDIO_I2C_SDA_IO, AUDIO_I2C_SCL_IO);

    audio_hal = new AudioEs8311Es7210(i2c0_bus,
        AUDIO_I2C_NUM,
        AUDIO_IN_SAMPLE_RATE,
        AUDIO_OUT_SAMPLE_RATE,
        AUDIO_I2S_MCK_IO,
        AUDIO_I2S_BCK_IO,
        AUDIO_I2S_WS_IO,
        AUDIO_I2S_DO_IO,
        AUDIO_I2S_DI_IO,
        AUDIO_PA_EN_IO,
        true,
        ES8311_I2C_ADDR,
        ES7210_I2C_ADDR,
        true);
}


LcSzpDevBoard::~LcSzpDevBoard()
{


}



AudioHAL* LcSzpDevBoard::GetAudioHAL()
{
    return audio_hal;
}


REGISTER_BOARD(LcSzpDevBoard);

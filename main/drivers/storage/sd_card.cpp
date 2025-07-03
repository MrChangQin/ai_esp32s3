#include "sd_card.h"


/**
 * @brief Construct a new Sd Card:: Sd Card object  
 * 
 */
SdCard::SdCard(){
    esp_err_t ret;
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,  // Format if mount fails
        .max_files = SD_CARD_MAX_FILES,  // Maximum number of open files
        .allocation_unit_size = 16 * 1024 // 16KB allocation unit size
    };
    sdmmc_card_t *card; // SD card handle
    const char mount_point[] = SD_CARD_MOUNT_POINT; // Mount point
    sdmmc_host_t host = SDMMC_HOST_DEFAULT(); // Default host configuration
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT(); // Default slot configuration
    slot_config.width = 1; // 1-bit data width
    slot_config.cmd = SD_CARD_PIN_CMD; // Command pin
    slot_config.clk = SD_CARD_PIN_CLK; // Clock pin
    slot_config.d0 = SD_CARD_PIN_D0; // Data pin
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP; // Enable internal pull-up resistors

    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SD card!");
        ESP_LOGD(TAG, "SD card error: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SD card mounted successfully");
    }

    sdmmc_card_print_info(stdout, card); // Print card information
}


/**
 * @brief Destroy the Sd Card:: Sd Card object
 * 
 */
SdCard::~SdCard() {
    // do something later...
}


//
esp_err_t SdCard::open(const char *filename, const char *mode) {
    char filepath[50]; // Ensure the filepath is large enough
    sprintf(filepath, "%s/%s", SD_CARD_MOUNT_POINT, filename);
    
    m_file = fopen(filepath, mode);
    if (m_file == NULL) {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "File opened successfully: %s", filepath);
    return ESP_OK;
    
}


//
esp_err_t SdCard::close() {
    if (m_file != NULL) {
        fclose(m_file);
        m_file = NULL;
        ESP_LOGI(TAG, "File closed successfully");
    }
    return ESP_OK;
}


// 
esp_err_t SdCard::write_file(const char *data, size_t size) {
    return fwrite(data, 1, size, m_file) == size ? ESP_OK : ESP_FAIL; // Write data to file
}


//
esp_err_t SdCard::read_file(char *output, size_t output_size) {
    if (m_file == NULL) {
        ESP_LOGE(TAG, "File not opened");
        return ESP_ERR_INVALID_STATE;
    }

    long current_pos = ftell(m_file); // Get current position
    ESP_LOGD(TAG, "Current file position: %ld", current_pos);
    if (fseek(m_file, 0, SEEK_END) != 0) { // Move to end of file
        ESP_LOGE(TAG, "Failed to seek to end of file");
        return ESP_FAIL;
    }

    long file_size = ftell(m_file); // Get file size
    ESP_LOGD(TAG, "File size: %ld", file_size);
    if (file_size < 0) {
        ESP_LOGE(TAG, "Failed to get file size");
        return ESP_FAIL;
    }
    
    if (fseek(m_file, 0, SEEK_SET) != 0) { // Move to start of file
        ESP_LOGE(TAG, "Failed to seek to start of file");
        return ESP_FAIL;
    }
    
    if (output_size < (size_t)(file_size + 1)) { // Check if output buffer is large enough
        ESP_LOGE(TAG, "Output buffer size is too small (requested %ld bytes, available %zu bytes)", file_size + 1, output_size);
        return ESP_ERR_INVALID_SIZE;
    }

    size_t bytes_read = fread(output, 1, file_size, m_file); // Read file content
    output[bytes_read] = '\0';

    ESP_LOGI(TAG, "Read %zu/%ld bytes: %.*s", bytes_read, file_size, (int)(bytes_read > 50 ? 50 : bytes_read), output);

    if (bytes_read != (size_t)file_size) {
        ESP_LOGW(TAG, "Only read %zu bytes, expected %ld bytes", bytes_read, file_size);
    }

    return ESP_OK;
}


//
esp_err_t SdCard::read_line(int line_num, char *output, size_t output_size) {
    char line[1000];  // Buffer to store each line
    int current_line = 0;

    char filepath[50];
    char data1[100];

    if (output_size > (sizeof(line)-1)) { // Check if output buffer is large enough
        ESP_LOGE(TAG, "Output buffer too small");
        return ESP_FAIL;
    }

    if (output_size < 1) { // Check if output buffer is too small
        ESP_LOGE(TAG, "Output buffer size is too small");
        return ESP_FAIL;
    }

    while (fgets(line, sizeof(line), m_file) != NULL) {
        current_line++;
        if (current_line == line_num) {  // Check if we reached the desired line
            size_t line_length = strlen(line); // Get the length of the line
            if (line_length > 0 && line[line_length - 1] == '\n') {
                line[line_length - 1] = '\0'; // Remove the newline character
            }
            strncpy(output, line, output_size - 1); // Copy the line to output buffer
            output[output_size - 1] = '\0';  // Ensure null termination
            return ESP_OK;
        }
    }

    if (current_line < line_num) {
        ESP_LOGE(TAG, "File has only %d lines, requested line %d", current_line, line_num);
        return ESP_FAIL; // Requested line number exceeds the number of lines in the file
    }

    return ESP_FAIL;
}


//
esp_err_t SdCard::seek(size_t offset, SeekMode mode) { 
    if (!m_file) {
        ESP_LOGE(TAG, "File not opened");
        return ESP_ERR_INVALID_STATE;
    }

    int origin;
    switch (mode) {
        case SeekMode::Set: origin = SEEK_SET; break;
        case SeekMode::Cur: origin = SEEK_CUR; break;
        case SeekMode::End: origin = SEEK_END; break;
        default: return ESP_ERR_INVALID_ARG;
    }

    if (fseek(m_file, offset, origin) != 0) {
        ESP_LOGE(TAG, "Failed to seek file");
        return ESP_FAIL;
    }

    return ESP_OK;
}


/**
 * @param filename : 文件路径
 * @param data : 要写入的数据
 * @param isAppend : 是否追加写入，默认为false
 * @return esp_err_t
 */
esp_err_t SdCard::sdCardWriteFile(const char *filename, const char *data, bool isAppend) {
    esp_err_t ret;
    char filepath[50]; // Ensure the filepath is large enough
    char data1[100]; // Ensure the data buffer is large enough
    sprintf(filepath, "%s/%s", SD_CARD_MOUNT_POINT, filename);
    FILE *fp = NULL;
    if (isAppend) {
        fp = fopen(filepath, "a");
    } 
    else {
        fp = fopen(filepath, "w");
    }
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing"); 
        return ESP_FAIL;
    }
    fprintf(fp, data); // 写入内容
    fclose(fp);          // 关闭文件
    ESP_LOGI(TAG, "File written");
    return ESP_OK;
}


/**
 * @brief   读取指定文件的指定行
 * @param filename : 文件名
 * @param output : 输出缓冲区
 * @param output_size : 输出缓冲区大小
 * @return esp_err_t 
 */
esp_err_t SdCard::sdCardReadFile(const char *filename, char *output, size_t output_size) {
    char filepath[50];
    char data1[100];
    sprintf(filepath, "%s/%s", SD_CARD_MOUNT_POINT, filename);

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    fseek (fp, 0, SEEK_END); // 移动文件指针到文件末尾
    long file_size = ftell(fp); // 获取文件大小
    fseek (fp, 0, SEEK_SET); // 重置文件指针到开头

    if (file_size < 0) {  // 文件大小小于0
        ESP_LOGE(TAG, "Failed to get file size");
        fclose(fp);
        return ESP_FAIL;
    }

    if (file_size < (size_t)(file_size + 1)) {  // 文件大小大于输出缓冲区大小
        ESP_LOGE(TAG, "File size is too large (requested %ld bytes, output buffer size %zu bytes)", file_size + 1, output_size);
        fclose(fp);
        return ESP_FAIL;
    }

    output[file_size] = '\0'; // 确保输出缓冲区以'\0'结尾 

    fclose(fp);
    ESP_LOGI(TAG, "Read file %s successful, size: %ld bytes", filename, file_size);
    return ESP_OK;
}


/**
 * @brief   读取指定文件的指定行
 * @param filename : 文件名
 * @param line_num : 要读取的行号，从1开始
 * @param output : 输出缓冲区
 * @param output_size : 输出缓冲区大小
 * @return esp_err_t 
 */
esp_err_t SdCard::sdCardReadLine(const char *filename, int line_num, char *output, size_t output_size) {
    char line[1000];  // Buffer to store each line
    int current_line = 0;

    char filepath[50];
    char data1[100];
    sprintf(filepath, "%s/%s", SD_CARD_MOUNT_POINT, filename);

    if (output_size > (sizeof(line)-1)) { // Check if output buffer is large enough
        ESP_LOGE(TAG, "Output buffer too small");
        return ESP_FAIL;
    }

    if (output_size < 1) { // Check if output buffer is too small
        ESP_LOGE(TAG, "Output buffer size is too small");
        return ESP_FAIL;
    }

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        current_line++;
        if (current_line == line_num) {  // Check if we reached the desired line
            size_t line_length = strlen(line); // Get the length of the line
            if (line_length > 0 && line[line_length - 1] == '\n') {
                line[line_length - 1] = '\0'; // Remove the newline character
            }
            strncpy(output, line, output_size - 1); // Copy the line to output buffer
            output[output_size - 1] = '\0';  // Ensure null termination
            fclose(fp);
            return ESP_OK;
        }
    }

    fclose(fp);

    if (current_line < line_num) {
        ESP_LOGE(TAG, "File has only %d lines, requested line %d", current_line, line_num);
        return ESP_FAIL; // Requested line number exceeds the number of lines in the file
    }

    return ESP_FAIL;
}

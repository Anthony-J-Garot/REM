#ifndef ESP_LOG_H
#define ESP_LOG_H

#include <stdio.h>

// Silence or redirect ESP_LOGX macros to standard printf for host debugging
#define ESP_LOGI(tag, fmt, ...) printf("[INFO] [%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, fmt, ...) printf("[ERROR] [%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) printf("[WARN] [%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) // No-op for debug level on host

#endif

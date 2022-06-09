#ifndef JK_10_02_LOG_H
#define JK_10_02_LOG_H

#include <stdio.h>

#define LOGI(fmt, ...) printf("[I] "fmt"\n", ##__VA_ARGS__)

#define LOGE(fmt, ...) printf("[E] "fmt"\n", ##__VA_ARGS__)

#endif

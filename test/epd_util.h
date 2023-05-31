/*
 * Copyright (c) 2023 SK Telecom Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __EPD_UTIL_H__
#define __EPD_UTIL_H__

#include <stddef.h>

#include "endpoint_detector.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Debug message */
#define HLINE_20 "--------------------"
#define HLINE (HLINE_20 HLINE_20 HLINE_20 HLINE_20 "\n")

char *util_strdup_printf(const char *fmt, ...);
int util_byte_to_frame_index(int byte_pos);
double util_frame_index_to_secs(size_t sample_index);
const char *util_epd_status_to_string(EpdStatus epd_status);

int util_setup_output_path(const char *path);
FILE *util_open_report_file(const char *path);

void util_msg(FILE *report_file, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

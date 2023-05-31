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

#ifndef __EPD_PROCESS_H__
#define __EPD_PROCESS_H__

#include <stdbool.h>

#include "endpoint_detector.h"

#ifdef __cplusplus
extern "C" {
#endif

struct frame_info {
	/* Audio sample from file load */
	char input[EPD_FRAME_DATA_SIZE];
	size_t input_length;
	double sec;

	/* Audio sample from EPD output */
	char *output;
	int output_length;

	EpdStatus epd_status;

	bool used;

	/* Debug hint message */
	char *data_hint;
	const char *hint;
};

struct frame_info **epd_process_load_file(FILE *report_file, const char *path,
					  int *frame_count);
void epd_process_unload(struct frame_info **frames, int frame_count);

int epd_process_run(struct frame_info **frames, int frame_count);

void epd_process_dump_result(FILE *report_file, struct frame_info **frames,
			     int frame_count);
int epd_process_save_file(FILE *report_file, struct frame_info **frames,
			  int frame_count, const char *output_directory);

#ifdef __cplusplus
}
#endif

#endif

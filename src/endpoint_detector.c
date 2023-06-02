/*
 * Copyright (c) 2019 SK Telecom Co., Ltd. All rights reserved.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "endpoint_detector.h"
#include "libEpdApi.h"

static void *epd_handle;

int epd_client_start(const char *model_file, EpdParam param)
{
	struct stat stat_buf;

	epdClientSetDebugOutput(0);

	if (!model_file) {
		fprintf(stderr, "model_file is empty\n");
		return -1;
	}

	if (stat(model_file, &stat_buf) < 0) {
		fprintf(stderr, "errno[%d]: %s (%s)\n", errno, strerror(errno),
			model_file);
		return -1;
	}

	if (epd_handle != NULL)
		epdClientChannelRELEASE(epd_handle);

	epd_handle = epdClientChannelSTART(model_file, param.sample_rate,
					   param.input_type, param.output_type,
					   1, param.max_speech_duration_secs,
					   param.time_out_secs,
					   param.pause_length_msecs);

	return (epd_handle == NULL) ? -1 : 0;
}

EpdStatus epd_client_run(char *data, int size)
{
	if (epd_handle == NULL)
		return EPD_INVALID_HANDLE;

	return (EpdStatus)epdClientChannelRUN(epd_handle, (short *)data, size,
					      0);
}

int epd_client_get_output_size(void)
{
	if (epd_handle == NULL)
		return -1;

	return epdClientChannelGetOutputDataSize(epd_handle);
}

int epd_client_get_output(char *buffer, int buffer_size)
{
	if (epd_handle == NULL)
		return -1;

	return epdClientChannelGetOutputData(epd_handle, buffer, buffer_size);
}

int epd_client_get_msec_start_detect(void)
{
	if (epd_handle == NULL)
		return -1;

	return epdClientGetSpeechStartDetectPoint(epd_handle);
}

int epd_client_get_msec_end_detect(void)
{
	if (epd_handle == NULL)
		return -1;

	return epdClientGetSpeechEndDetectPoint(epd_handle);
}

int epd_client_get_speech_boundary(int *start_time, int *end_time)
{
	if (epd_handle == NULL)
		return -1;

	return epdClientChannelGetSpeechBoundary(epd_handle, start_time,
						 end_time, 0, 0);
}

int epd_client_save_speech_data(const char *path, const char *file)
{
	if (epd_handle == NULL)
		return -1;

	return epdClientSaveRecordedSpeechData(epd_handle, path, file);
}

void epd_client_set_log(int enable)
{
	epdClientSetDebugOutput(enable);
}

int epd_client_release(void)
{
	if (epd_handle == NULL)
		return -1;

	int ret = epdClientChannelRELEASE(epd_handle);
	epd_handle = NULL;

	return ret;
}

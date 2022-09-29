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

#include <stdio.h>
#include "speex.h"
#include "libSpeexApi.h"

static void *speex_handle;

typedef enum {
	DATA_LINEAR_PCM16 = 0,
	DATA_LINEAR_PCM8 = 1,
	DATA_A_LAW = 2,
	DATA_MU_LAW = 3,
	DATA_SPEEX_STREAM = 4,
	DATA_FEAT_STREAM = 5,
} DATA_TYPE;

int epd_speex_start(int sample_rate)
{
	if (sample_rate <= 0)
		return -1;

	if (speex_handle != NULL)
		speexRELEASE(speex_handle);

	speex_handle =
		speexSTART(sample_rate, DATA_LINEAR_PCM16, DATA_SPEEX_STREAM);
	if (speex_handle == NULL)
		return -1;

	return 0;
}

int epd_speex_release()
{
	int ret;

	if (speex_handle == NULL)
		return -1;

	ret = speexRELEASE(speex_handle);
	speex_handle = NULL;

	return ret;
}

int epd_speex_run(const void *data, int size)
{
	int ret;

	if (speex_handle == NULL || data == NULL)
		return -1;

	ret = speexRUN(speex_handle, (const myptr)data, size, 0);
	if (ret < 0)
		return -1;

	return speexGetOutputDataSize(speex_handle);
}

int epd_speex_get_encoded_data(char *buf, int buf_size)
{
	if (speex_handle == NULL || buf == NULL || buf_size == 0)
		return -1;

	speexGetOutputData(speex_handle, buf, buf_size);

	return 0;
}

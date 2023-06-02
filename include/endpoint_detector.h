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

#ifndef __ENDPOINT_DETECTOR_H__
#define __ENDPOINT_DETECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EPD_SAMPLE_RATE 16000 /* 16000 Hz (16 kHz) */
#define EPD_SAMPLE_BITS 16 /* signed 16 bit little endian */
#define EPD_CHANNEL 1 /* Mono */

#define EPD_BITS_PER_SECS (EPD_SAMPLE_RATE * EPD_SAMPLE_BITS * EPD_CHANNEL)
#define EPD_BYTES_PER_SECS (EPD_BITS_PER_SECS / 8)

#define EPD_FRAME_SIZE_MSEC 10 /* 10ms (0.01 sec) */
#define EPD_FRAME_DATA_SIZE (EPD_BYTES_PER_SECS / 1000 * EPD_FRAME_SIZE_MSEC)
#define EPD_FRAME_SAMPLE_SIZE (EPD_FRAME_DATA_SIZE / (EPD_SAMPLE_BITS / 8))

enum epd_data_type {
	EPD_DATA_TYPE_LINEAR_PCM16, /* 16-bit linear PCM, 16bits, 8/16kHz */
	EPD_DATA_TYPE_LINEAR_PCM8, /* 8-bit linear PCM, 8bits, 8/16kHz */
	EPD_DATA_TYPE_A_LAW, /* A-law, 8bits, 8/16kHz */
	EPD_DATA_TYPE_MU_LAW, /* mu-law, 8bits, 8/16kHz */
	EPD_DATA_TYPE_SPEEX_STREAM, /* speex codec, 8bits, 8/16kHz */
	EPD_DATA_TYPE_FEAT_STREAM /* speech feature stream, 8bits, 8/16kHz */
};

typedef struct _epd_param {
	int sample_rate;
	int max_speech_duration_secs;
	int time_out_secs;
	int pause_length_msecs;
	enum epd_data_type input_type;
	enum epd_data_type output_type;
} EpdParam;

typedef enum _epd_status {
	EPD_INVALID_HANDLE = -1,
	EPD_START_DETECTING = 0,
	EPD_START_DETECTED = 1,
	EPD_END_DETECTED = 2,
	EPD_END_DETECTING = 3,
	EPD_TIMEOUT = 4,
	EPD_MAXSPEECH = 5,
	EPD_END_CHECK = 6,
} EpdStatus;

int epd_client_start(const char *model_file, EpdParam param);
int epd_client_release(void);

EpdStatus epd_client_run(char *data, int size);

void epd_client_set_log(int enable);

int epd_client_get_output_size(void);
int epd_client_get_output(char *buffer, int buffer_size);
int epd_client_get_msec_start_detect(void);
int epd_client_get_msec_end_detect(void);

int epd_client_get_speech_boundary(int *start_time, int *end_time);
int epd_client_save_speech_data(const char *path, const char *file);

#ifdef __cplusplus
}
#endif

#endif /* __ENDPOINT_DETECTOR_H__ */

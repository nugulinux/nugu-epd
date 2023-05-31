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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epd_process.h"
#include "epd_util.h"

#define HINT_START_DETECTED "START DETECTED"
#define HINT_END_DETECTED "END DETECTED"

#define HINT_BUFFERED_DATA "Buffered data"
#define HINT_EVAL_END_POINT_START "Evaluating END-POINT..."
#define HINT_EVAL_END_POINT_FAIL "Evaluating result: Not END-POINT"
#define HINT_EVAL_END_POINT_SUCCESS "Evaluating result: END-POINT DETECTED !!!"

static int EPD_FRAME_INDEX_PCM_START;
static int EPD_FRAME_INDEX_PCM_END;
static int EPD_FRAME_INDEX_START_DETECTED;
static int EPD_FRAME_INDEX_END_DETECTED;

static void process_EPD_START_DETECTED(struct frame_info **frames,
				       int current_frame_id, int frame_count)
{
	struct frame_info *cur = frames[current_frame_id];
	int including_pcm_frames;
	int start;

	including_pcm_frames = util_byte_to_frame_index(cur->output_length);
	start = current_frame_id - including_pcm_frames;

	cur->data_hint = util_strdup_printf("(include %d(%d ~ %d) frames)",
					    including_pcm_frames, start + 1,
					    current_frame_id + 1);
	cur->hint = HINT_START_DETECTED;

	EPD_FRAME_INDEX_START_DETECTED = current_frame_id + 1;
	EPD_FRAME_INDEX_PCM_START = start + 1;

	for (; start < current_frame_id; start++) {
		frames[start]->used = true;
		frames[start]->data_hint = strdup(HINT_BUFFERED_DATA);
	}
}

static void process_EPD_END_DETECTING(struct frame_info **frames,
				      int current_frame_id, int frame_count)
{
	struct frame_info *cur = frames[current_frame_id];
	int cur_output_len;

	if (current_frame_id < 1)
		return;

	cur_output_len = frames[current_frame_id]->output_length;

	// EPD Output length of previous frame > 0
	if (frames[current_frame_id - 1]->output_length > 0) {
		if (cur_output_len == 0)
			cur->hint = HINT_EVAL_END_POINT_START;

		return;
	}

	// Get buffered result
	if (cur_output_len > EPD_FRAME_DATA_SIZE) {
		int including_pcm_frames;
		int start;

		cur->hint = HINT_EVAL_END_POINT_FAIL;

		including_pcm_frames =
			util_byte_to_frame_index(cur->output_length);
		start = current_frame_id - including_pcm_frames + 1;

		cur->data_hint =
			util_strdup_printf("(include %d(%d ~ %d) frames)",
					   including_pcm_frames, start + 1,
					   current_frame_id + 1);

		for (; start < current_frame_id; start++) {
			frames[start]->used = true;
			frames[start]->data_hint = strdup(HINT_BUFFERED_DATA);
		}
	} else if (cur_output_len == EPD_FRAME_DATA_SIZE) {
		cur->hint = HINT_EVAL_END_POINT_SUCCESS;
	}
}

static void process_EPD_END_DETECTED(struct frame_info **frames,
				     int current_frame_id, int frame_count)
{
	struct frame_info *cur = frames[current_frame_id];
	int i;
	int real_sample_index = 0;
	int pending_sample_index = 0;
	int count = 0;

	if (current_frame_id < 1)
		return;

	/* Check already END DETECTED or not */
	if (frames[current_frame_id - 1]->epd_status == EPD_END_DETECTED)
		return;

	cur->hint = HINT_END_DETECTED;

	for (i = current_frame_id - 1; i > 0; i--) {
		if (frames[i]->epd_status != EPD_END_DETECTING)
			break;

		if (pending_sample_index == 0) {
			if (frames[i]->output_length == 0)
				pending_sample_index = i + 1;
		} else {
			if (real_sample_index == 0) {
				if (frames[i]->output_length != 0) {
					real_sample_index = i + 1;
					break;
				}
			}
		}
	}

	if (real_sample_index == 0 || pending_sample_index == 0)
		return;

	count = current_frame_id - pending_sample_index;
	for (i = 0; i < count; i++) {
		frames[real_sample_index + i]->used = true;
		frames[real_sample_index + i]->data_hint = util_strdup_printf(
			"(move to %d)", pending_sample_index + i + 1);

		frames[pending_sample_index + i]->used = false;
		frames[pending_sample_index + i]->data_hint =
			util_strdup_printf("(from %d)",
					   real_sample_index + i + 1);
	}

	EPD_FRAME_INDEX_PCM_END = real_sample_index + count;
	EPD_FRAME_INDEX_END_DETECTED = current_frame_id + 1;
}

int epd_process_run(struct frame_info **frames, int frame_count)
{
	struct frame_info *cur;
	int i;

	if (frames == NULL || frame_count <= 0)
		return -1;

	EPD_FRAME_INDEX_PCM_START = 0;
	EPD_FRAME_INDEX_PCM_END = 0;
	EPD_FRAME_INDEX_START_DETECTED = 0;
	EPD_FRAME_INDEX_END_DETECTED = 0;

	for (i = 0; i < frame_count; i++) {
		cur = frames[i];

		cur->epd_status = epd_client_run(cur->input, cur->input_length);

		cur->output_length = epd_client_get_output_size();
		if (cur->output_length > 0) {
			int outbuf_len;

			cur->used = true;
			cur->output = malloc(cur->output_length);

			outbuf_len = epd_client_get_output(cur->output,
							   cur->output_length);

			if (outbuf_len != cur->output_length) {
				fprintf(stderr,
					"output data length mismtach (%d != %d)\n",
					outbuf_len, cur->output_length);
			}
		}

		switch (cur->epd_status) {
		case EPD_START_DETECTED:
			process_EPD_START_DETECTED(frames, i, frame_count);
			break;
		case EPD_END_DETECTING:
			process_EPD_END_DETECTING(frames, i, frame_count);
			break;
		case EPD_END_DETECTED:
			process_EPD_END_DETECTED(frames, i, frame_count);
		default:
			break;
		}
	}

	return 0;
}

struct frame_info **epd_process_load_file(FILE *report_file, const char *path,
					  int *frame_count)
{
	FILE *fp;
	long file_size;
	int i = 0;
	struct frame_info **frames;
	int count;
	size_t nread;

	if (!path || !frame_count)
		return NULL;

	fp = fopen(path, "rb");
	if (!fp) {
		fprintf(stderr, "fopen(%s) failed\n", path);
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);

	count = util_byte_to_frame_index(file_size);

	util_msg(report_file, "File: %s (%ld bytes)\n", path, file_size);
	util_msg(report_file, "Total frames: %d (%f secs)\n", count,
		 util_frame_index_to_secs(count));

	frames = malloc(sizeof(struct frame_info *) * count);
	memset(frames, 0, sizeof(struct frame_info *) * count);

	for (; !feof(fp); i++) {
		frames[i] = malloc(sizeof(struct frame_info));
		memset(frames[i], 0, sizeof(struct frame_info));

		nread = fread(frames[i]->input, 1, EPD_FRAME_DATA_SIZE, fp);
		if (nread == 0)
			break;

		frames[i]->input_length = nread;
		frames[i]->sec =
			((i + 1) * (double)EPD_FRAME_SIZE_MSEC) / 1000.0;
	}

	fclose(fp);

	*frame_count = count;
	return frames;
}

void epd_process_dump_result(FILE *report_file, struct frame_info **frames,
			     int frame_count)
{
	int i;
	int current_frame_id;
	struct frame_info *cur;

	if (frames == NULL || frame_count <= 0)
		return;

	util_msg(report_file, HLINE);

	for (i = 0; i < frame_count; i++) {
		cur = frames[i];
		current_frame_id = i + 1;

		if (cur->used == false)
			util_msg(report_file, "\e[31m");

		util_msg(report_file,
			 "[%5d / %5d frame, %8.5f sec %s output=%6d]",
			 current_frame_id, frame_count, cur->sec,
			 util_epd_status_to_string(cur->epd_status),
			 cur->output_length);

		if (cur->data_hint != NULL)
			util_msg(report_file, " %s", cur->data_hint);

		if (cur->used == false)
			util_msg(report_file, " (discarded)");

		if (cur->hint != NULL)
			util_msg(report_file, " \e[0m\e[1m%s\e[0m", cur->hint);

		util_msg(report_file, "\e[0m\n");
	}

	util_msg(report_file, HLINE);

	util_msg(report_file, "- EPD_FRAME_INDEX_PCM_START = %d\n",
		 EPD_FRAME_INDEX_PCM_START);
	util_msg(report_file, "- EPD_FRAME_INDEX_PCM_END = %d\n",
		 EPD_FRAME_INDEX_PCM_END);
	util_msg(report_file,
		 "- EPD_FRAME_INDEX_START_DETECTED = %d (%d msec)\n",
		 EPD_FRAME_INDEX_START_DETECTED,
		 epd_client_get_msec_start_detect());
	util_msg(report_file, "- EPD_FRAME_INDEX_END_DETECTED = %d (%d msec)\n",
		 EPD_FRAME_INDEX_END_DETECTED,
		 epd_client_get_msec_end_detect());

	util_msg(report_file, HLINE);
}

void epd_process_unload(struct frame_info **frames, int frame_count)
{
	int i;

	if (frames == NULL)
		return;

	for (i = 0; i < frame_count; i++) {
		if (frames[i] == NULL)
			continue;

		if (frames[i]->output)
			free(frames[i]->output);

		if (frames[i]->data_hint)
			free(frames[i]->data_hint);

		memset(frames[i], 0, sizeof(struct frame_info));
		free(frames[i]);
	}

	memset(frames, 0, sizeof(struct frame_info *) * frame_count);
	free(frames);
}

struct output_file {
	FILE *fp;
	const char *filename;
	size_t nwritten;
};

static struct output_file files[] = { //
	{ NULL, "output_all.raw", 0 },
	{ NULL, "output_START_DETECTED.raw", 0 },
	{ NULL, "output_END_DETECTED.raw", 0 }
};

static int prepare_output_files(const char *output_directory)
{
	size_t i;

	for (i = 0; i < sizeof(files) / sizeof(struct output_file); i++) {
		char *path;

		path = util_strdup_printf("%s/%s", output_directory,
					  files[i].filename);
		if (!path)
			return -1;

		files[i].nwritten = 0;
		files[i].fp = fopen(path, "wb");
		if (files[i].fp == NULL) {
			fprintf(stderr, "fopen('%s') failed\n", path);
			free(path);
			return -1;
		}

		free(path);
	}

	return 0;
}

static void close_output_files(FILE *report_file, const char *output_directory)
{
	size_t i;

	for (i = 0; i < sizeof(files) / sizeof(struct output_file); i++) {
		if (files[i].fp) {
			util_msg(report_file,
				 "- %s/%-25s %zd bytes (%d frames)\n",
				 output_directory, files[i].filename,
				 files[i].nwritten,
				 util_byte_to_frame_index(files[i].nwritten));
			fclose(files[i].fp);
			files[i].fp = NULL;
		}
	}
}

int epd_process_save_file(FILE *report_file, struct frame_info **frames,
			  int frame_count, const char *output_directory)
{
	int i;
	struct frame_info *cur;

	if (frames == NULL || frame_count <= 0 || output_directory == NULL)
		return -1;

	if (prepare_output_files(output_directory) < 0)
		return -1;

	epd_client_save_speech_data(output_directory, "epd_result.raw");

	for (i = 0; i < frame_count; i++) {
		size_t ret;

		cur = frames[i];
		if (!cur)
			continue;

		if (cur->output_length == 0 || cur->output == NULL)
			continue;

		ret = fwrite(cur->output, 1, cur->output_length, files[0].fp);
		files[0].nwritten += ret;

		if (cur->epd_status == EPD_START_DETECTED) {
			ret = fwrite(cur->output, 1, cur->output_length,
				     files[1].fp);
			files[1].nwritten += ret;
		} else if (cur->epd_status == EPD_END_DETECTING ||
			   cur->epd_status == EPD_END_DETECTED) {
			ret = fwrite(cur->output, 1, cur->output_length,
				     files[2].fp);
			files[2].nwritten += ret;
		}
	}

	close_output_files(report_file, output_directory);

	printf(HLINE);

	return 0;
}

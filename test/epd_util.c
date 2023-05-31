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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "epd_util.h"

char *util_strdup_printf(const char *fmt, ...)
{
	int s;
	char *rt;
	va_list va;

	va_start(va, fmt);
	s = vsnprintf(NULL, 0, fmt, va);
	va_end(va);

	rt = malloc(s + 1);
	if (!rt)
		return NULL;

	rt[s] = 0;

	va_start(va, fmt);
	vsnprintf(rt, s + 1, fmt, va);
	va_end(va);

	return rt;
}

int util_byte_to_frame_index(int byte_pos)
{
	return (byte_pos + (EPD_FRAME_DATA_SIZE / 2)) / EPD_FRAME_DATA_SIZE;
}

double util_frame_index_to_secs(size_t sample_index)
{
	return (double)sample_index * (double)EPD_FRAME_SIZE_MSEC / 1000.0;
}

const char *util_epd_status_to_string(EpdStatus epd_status)
{
	switch (epd_status) {
	case EPD_START_DETECTING:
		return "EPD_START_DETECTING";
	case EPD_START_DETECTED:
		return "EPD_START_DETECTED ";
	case EPD_END_DETECTED:
		return "EPD_END_DETECTED   ";
	case EPD_END_DETECTING:
		return "EPD_END_DETECTING  ";
	case EPD_TIMEOUT:
		return "EPD_TIMEOUT        ";
	case EPD_MAXSPEECH:
		return "EPD_MAXSPEECH      ";
	case EPD_END_CHECK:
		return "EPD_END_CHECK      ";
	default:
		break;
	}

	return "----- UNKNOWN -----";
}

int util_setup_output_path(const char *path)
{
	struct stat statbuf;

	memset(&statbuf, 0, sizeof(struct stat));

	if (stat(path, &statbuf) == 0) {
		switch (statbuf.st_mode & S_IFMT) {
		case S_IFDIR:
			return 0;
		default:
			fprintf(stderr, "%s path is not a directory\n", path);
			return -1;
		}
	}

	if (mkdir(path, 0755) < 0) {
		fprintf(stderr, "mkdir('%s') failed\n", path);
		return -1;
	}

	return 0;
}

FILE *util_open_report_file(const char *path)
{
	char *buf;
	FILE *fp;

	buf = util_strdup_printf("%s/report.txt", path);
	if (!buf)
		return NULL;

	fp = fopen(buf, "w");
	if (fp == NULL)
		fprintf(stderr, "fopen('%s') failed\n", buf);

	free(buf);

	return fp;
}

void util_msg(FILE *report_file, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);

	if (report_file == NULL)
		return;

	va_start(ap, fmt);
	vfprintf(report_file, fmt, ap);
	va_end(ap);
}

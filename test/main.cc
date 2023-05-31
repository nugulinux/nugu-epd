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

#include "epd_process.h"
#include "epd_util.h"

#define DEFAULT_OUTPUT_PATH "output"

int main(int argc, char* argv[])
{
    struct frame_info** frames;
    int frame_count = 0;
    EpdParam param;
    const char* output_path = DEFAULT_OUTPUT_PATH;
    FILE* report_file;

    param.sample_rate = EPD_SAMPLE_RATE;
    param.max_speech_duration_secs = 10;
    param.time_out_secs = 10;
    param.pause_length_msecs = 700;
    param.input_type = EPD_DATA_TYPE_LINEAR_PCM16;
    param.output_type = EPD_DATA_TYPE_LINEAR_PCM16;

    if (argc < 3) {
        printf("Usage: %s <model-file> <raw-pcm-file> [<output-path>]\n", argv[0]);
        return -1;
    }

    if (argc == 4)
        output_path = argv[3];

    if (util_setup_output_path(output_path) < 0)
        return -1;

    report_file = util_open_report_file(output_path);

    util_msg(report_file, HLINE);
    util_msg(report_file, "Sample rate: %d Hz / Sample bits: %d bit\n", EPD_SAMPLE_RATE, EPD_SAMPLE_BITS);
    util_msg(report_file, "Bitrate: %d bits (== %d Bytes per secs)\n", EPD_BITS_PER_SECS, EPD_BYTES_PER_SECS);
    util_msg(report_file, "Frame size: %d msec (== %d Bytes, == %d Samples)\n",
        EPD_FRAME_SIZE_MSEC, EPD_FRAME_DATA_SIZE, EPD_FRAME_SAMPLE_SIZE);
    util_msg(report_file, HLINE);

    if (epd_client_start(argv[1], param) < 0) {
        fprintf(stderr, "epd_client_start() failed\n");
        return -1;
    }

    epd_client_set_log(1);

    frames = epd_process_load_file(report_file, argv[2], &frame_count);
    if (frames == NULL) {
        epd_client_release();
        return -1;
    }

    epd_process_run(frames, frame_count);

    epd_process_dump_result(report_file, frames, frame_count);
    epd_process_save_file(report_file, frames, frame_count, output_path);

    epd_process_unload(frames, frame_count);

    if (report_file)
        fclose(report_file);

    epd_client_release();

    return 0;
}

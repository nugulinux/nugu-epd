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

#ifndef __ENDPOINT_DETECTOR_SPEEX_ENCODING_H__
#define __ENDPOINT_DETECTOR_SPEEX_ENCODING_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief start speex encoding (create handle)
 *
 * @param[in] sample_rate sample rate
 * @return result
 * @retval 0 success
 * @retval -1 failure
 */
int epd_speex_start(int sample_rate);

/**
 * @brief release speex encoding
 *
 * @return result
 * @retval 0 success
 * @retval -1 failure
 */
int epd_speex_release();

/**
 * @brief speex encoding from pcm data
 *
 * @param[in] data pcm data
 * @param[in] size size of pcm data
 * @return result
 * @retval >0 encoded data size
 * @retval -1 failure
 */
int epd_speex_run(const void *data, int size);

/**
 * @brief get speex encoded data
 *
 * @param[out] buf output buffer
 * @param[in] buf_size output buffer size
 * @return result
 * @retval 0 success
 * @retval -1 failure
 */
int epd_speex_get_encoded_data(char *buf, int buf_size);

#ifdef __cplusplus
}
#endif

#endif /* __ENDPOINT_DETECTOR_SPEEX_ENCODING_H__ */
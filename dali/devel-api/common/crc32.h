#ifndef __DALI_CRC32__
#define __DALI_CRC32__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// EXTERNAL INCLUDES
#include <inttypes.h>

namespace Dali
{

/**
 * @brief Create a CRC32 value out of given binary data
 * @param[in] initialCRC Initial checksum value
 * @param[in] buffer Pointer to data to be processed
 * @param[in] size Size of the buffer
 * @return Computed CRC32 value
 */
uint32_t CalculateCRC32( const void* buffer, unsigned int size, uint32_t initialCRC = 0 );

} // namespace Dali

#endif // __DALI_CRC32__

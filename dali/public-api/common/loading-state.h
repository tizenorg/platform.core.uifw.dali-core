#ifndef __DALI_LOADING_STATE_H__
#define __DALI_LOADING_STATE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief The status during resource loading operations.
 */
enum LoadingState
{
  ResourceLoading,          ///< The resource is loading
  ResourceLoadingSucceeded, ///< The resource loaded successfully
  ResourceLoadingFailed     ///< The resource failed to load
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_LOADING_STATE_H__

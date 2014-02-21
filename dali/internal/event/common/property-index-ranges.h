#ifndef __DALI_INTERNAL_PROPERTY_INDEX_RANGES_H__
#define __DALI_INTERNAL_PROPERTY_INDEX_RANGES_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

namespace Dali
{

namespace Internal
{

/*
 * Specifies the allowed ranges for different class types to cater for future allocation
 */

#define DEFAULT_PROPERTY_MAX_COUNT                  10000 ///< Default Property Range:    0 to 9999

#define DEFAULT_ACTOR_PROPERTY_MAX_COUNT            1000  ///< Actor Range:               0 to 999
#define RENDERABLE_ACTOR_DEFAULT_PROPERTY_MAX_COUNT 2000  ///< Renderable Actor Range: 1000 to 1999

#define DEFAULT_GESTURE_DETECTOR_PROPERTY_MAX_COUNT 1000  ///< GestureDetector Range:     0 to 999

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_INDEX_RANGES_H__

#ifndef __DALI_TAP_GESTURE_H__
#define __DALI_TAP_GESTURE_H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

/**
 * A TapGesture is emitted when the user taps the screen with the stated number of fingers a stated
 * number of times.  This is a discrete gesture so does not have any state information.
 * @see TapGestureDetector
 */
struct TapGesture : public Gesture
{
  // Construction & Destruction

  /**
   * Default Constructor
   */
  TapGesture();

  /**
   * Copy constructor
   */
  TapGesture( const TapGesture& rhs );

  /**
   * Assignment operator
   */
  TapGesture& operator=( const TapGesture& rhs );

  /**
   * Virtual destructor
   */
  virtual ~TapGesture();

  // Data

  /**
   * The number of taps in this tap gesture.
   */
  unsigned int numberOfTaps;

  /**
   * The number of touch points in this tap gesture, i.e. the number of fingers the user had on the
   * screen to generate the tap gesture.
   */
  unsigned int numberOfTouches;

  /**
   * This is the point, in screen coordinates, where the tap occurred.
   * If a multi-touch tap, then this is the centroid of all the touch points.
   */
  Vector2 screenPoint;

  /**
   * This is the point, in local actor coordinates, where the tap occurred.
   * If a multi-touch tap, then this is the centroid of all the touch points.
   * @return The point where tap has occurred (in local actor coordinates).
   */
  Vector2 localPoint;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_TAP_GESTURE_H__

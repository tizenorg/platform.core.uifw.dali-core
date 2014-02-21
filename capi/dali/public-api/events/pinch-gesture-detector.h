#ifndef __DALI_PINCH_GESTURE_DETECTOR_H__
#define __DALI_PINCH_GESTURE_DETECTOR_H__

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
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class PinchGestureDetector;
}

class PinchGesture;

/**
 * This class looks for pinching gestures involving two touches.  It tries to detect when the user moves two
 * touch points towards or away from each other.  Please see PinchGesture for more information.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * PinchGestureDetector detector = PinchGestureDetector::New();
 * detector.Attach(myActor);
 * detector.SignalDetected().Connect(this, &MyApplication::OnPinch);
 * @endcode
 *
 * @see PinchGesture
 */
class PinchGestureDetector : public GestureDetector
{
public: // Typedefs

  typedef SignalV2< void (Actor, PinchGesture) > DetectedSignalV2;

  //Signal Names
  static const char* const SIGNAL_PINCH_DETECTED;

public: // Creation & Destruction

  /**
   * Create an uninitialized PinchGestureDetector; this can be initialized with PinchGestureDetector::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PinchGestureDetector();

  /*
   * Create an initialized PinchGestureDetector.
   * @return A handle to a newly allocated Dali resource.
   */
  static PinchGestureDetector New();

  /**
   * Downcast an Object handle to PinchGestureDetector handle. If handle points to a PinchGestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a PinchGestureDetector object or an uninitialized handle
   */
  static PinchGestureDetector DownCast( BaseHandle handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~PinchGestureDetector();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Signals

  /**
   * This signal is emitted when the pinch gesture is detected on the attached actor.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor actor, PinchGesture gesture);
   * @endcode
   * @pre The gesture detector has been initialized.
   * @return The signal to connect to.
   */
  DetectedSignalV2& DetectedSignal();

public: // Not intended for Application developers

  /**
   * This constructor is used by Dali New() methods.
   * @param [in]  internal  A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL PinchGestureDetector(Internal::PinchGestureDetector* internal);

};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PINCH_GESTURE_DETECTOR_H__

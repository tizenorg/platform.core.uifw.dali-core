#ifndef __DALI_LONG_PRESS_GESTURE_DETECTOR_H__
#define __DALI_LONG_PRESS_GESTURE_DETECTOR_H__

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
class LongPressGestureDetector;
}

struct LongPressGesture;

/**
 * This class emits a signals when a long press gesture occurs that meets the requirements set by the
 * application. See LongPressGestureDetector::SetTouchesRequired.
 *
 * For any valid long press. Two signals will be emitted:
 * - First identifying the beginning (state = Started) i.e. when fingers held down for the required time.
 * - Second identifying the ending (state = Finished) i.e. when fingers are released.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * LongPressGestureDetector detector = LongPressGestureDetector::New();
 * detector.Attach(myActor);
 * detector.SignalDetected().Connect(this, &MyApplication::OnLongPress);
 * @endcode
 *
 * @see LongPressGesture
 */
class LongPressGestureDetector : public GestureDetector
{
public: // Typedefs

  typedef SignalV2< void (Actor, LongPressGesture) > DetectedSignalV2;

  //Signal Names
  static const char* const SIGNAL_LONG_PRESS_DETECTED;

public: // Creation & Destruction

  /**
   * Create an uninitialized LongPressGestureDetector; this can be initialized with LongPressGestureDetector::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  LongPressGestureDetector();

  /**
   * Create an initialized LongPressGestureDetector.
   * By default, this would create a gesture detector that requires only one touch.
   * @return A handle to a newly allocated Dali resource.
   */
  static LongPressGestureDetector New();

  /**
   * Create an initialized LongPressGestureDetector with the number of touches required.
   * A long press gesture will be emitted from this detector if the number of fingers touching the
   * screen is equal to the touches required.
   * @param[in]  touchesRequired  The number of touches required.
   * @return A handle to a newly allocated Dali resource.
   */
  static LongPressGestureDetector New(unsigned int touchesRequired);

  /**
   * Create an initialized LongPressGestureDetector with the minimum and maximum number of touches required.
   * A long press gesture will be emitted from this detector if the number of fingers touching the screen
   * falls between the minimum and maximum touches set.
   * @param[in]  minTouches  The minimum number of touches required.
   * @param[in]  maxTouches  The maximum number of touches required.
   * @return A handle to a newly allocated Dali resource.
   */
  static LongPressGestureDetector New(unsigned int minTouches, unsigned int maxTouches);

  /**
   * Downcast an Object handle to LongPressGestureDetector handle. If handle points to a LongPressGestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a LongPressGestureDetector object or an uninitialized handle
   */
  static LongPressGestureDetector DownCast( BaseHandle handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~LongPressGestureDetector();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Setters

  /**
   * Set the number of touches required. The number of touches corresponds to the number of fingers
   * a user has on the screen.
   * This sets the minimum and maximum touches to the input parameter.
   * @param[in]  touches  Touches required.
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetTouchesRequired(unsigned int touches);

  /**
   * Sets the minimum and maximum touches required.  The number of touches corresponds to the number
   * of fingers a user has on the screen.
   * @param[in]  minTouches  Minimum Touches required.
   * @param[in]  maxTouches  Maximum Touches required.
   * @pre The gesture detector has been initialized.
   * @note The default is '1'.
   */
  void SetTouchesRequired(unsigned int minTouches, unsigned int maxTouches);

public: // Getters

  /**
   * Retrieves the minimum number of touches required.
   * @return The minimum number of touches required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMinimumTouchesRequired() const;

  /**
   * Retrieves the maximum number of touches required.
   * @return The maximum number of touches required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMaximumTouchesRequired() const;

public: // Signals

  /**
   * This signal is emitted when the specified long press is detected on the attached actor.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(Actor actor, LongPressGesture gesture);
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
  explicit DALI_INTERNAL LongPressGestureDetector(Internal::LongPressGestureDetector* internal);

};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_LONG_PRESS_GESTURE_DETECTOR_H__

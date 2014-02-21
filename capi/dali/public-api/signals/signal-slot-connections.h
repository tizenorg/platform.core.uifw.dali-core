#ifndef __DALI_SIGNAL_SLOT_CONNECTIONS_H__
#define __DALI_SIGNAL_SLOT_CONNECTIONS_H__

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
#include <dali/public-api/signals/signal-slot-observers.h>

namespace Dali DALI_IMPORT_API
{

class CallbackBase;

/**
 * Slot connection is the connection information held by a connection tracker.
 *
 * A slot can have zero to many connection objects, depending
 * on how many signals it is connected to.
 *
 * A connection contains:
 * - Callback (slot)
 * - SlotObserver -interface provided by the signal
 *
 * It takes ownership of the callback, and will delete it when
 * the connection is destroyed.
 */
class SlotConnection
{
public:

  /**
   * Constructor
   * @param[in] slotObserver The slot observer.
   * @param[in] callback Ownership of this callback object is taken.
   */
  SlotConnection(SlotObserver* slotObserver, CallbackBase* callback);

  /**
   * Non-virtual destructor, not intended as a base class.
   */
  ~SlotConnection();

  /**
   * Retrieve the callback.
   * @return A pointer to the callback.
   */
  CallbackBase* GetCallback();

  /**
   * Retrieve the slot observer.
   * @return A pointer to the slot observer.
   */
  SlotObserver* GetSlotObserver();

private:

  SlotConnection( const SlotConnection& );            ///< undefined copy constructor
  SlotConnection& operator=( const SlotConnection& ); ///< undefined assignment operator

private:

  SlotObserver* mSlotObserver; ///< a pointer to the slot observer (not owned)
  CallbackBase* mCallback;     ///< callback, has ownership
};

/**
 * SignalConnection is the connection information held by the signal.
 *
 * A signal can have zero to many connections, depending on how
 * many slots are connected to this signal.
 *
 * A connection contains:
 * - Callback (slot)
 * - SignalObserver - interface provided by a slot owning object.
 *
 * It holds a pointer to the callback, but does not own it.
 *
 */
class SignalConnection
{
public:

  /**
   * Constructor
   * @param[in] callback The callback which should be a C function.
   */
  SignalConnection( CallbackBase* callback );

  /**
   * Constructor
   * @param[in] signalObserver The signal observer.
   * @param[in] callback The callback which should be a member function of the signalObserver.
   */
  SignalConnection( SignalObserver* signalObserver, CallbackBase* callback );

  /**
   * Non-virtual destructor, not intended as a base class.
   */
  ~SignalConnection();

  /**
   * Disconnect the signal from the slot.
   * @param[in] slotObserver The signal disconnecting from the slot.
   */
  void Disconnect( SlotObserver* slotObserver );

  /**
   * Retrieve the callback.
   * @return A pointer to the callback.
   */
  CallbackBase* GetCallback();

private:

  SignalConnection( const SignalConnection& );            ///< undefined copy constructor
  SignalConnection& operator=( const SignalConnection& ); ///< undefined assignment operator

private:

  SignalObserver* mSignalObserver; ///< a pointer to the signal observer (not owned)
  CallbackBase* mCallback;         ///< The callback. This is not owned, the corresponding SlotConnection has ownership.
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_SIGNAL_SLOT_CONNECTIONS_H__

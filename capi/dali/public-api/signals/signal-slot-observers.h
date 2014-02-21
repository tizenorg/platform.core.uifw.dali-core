#ifndef __DALI_SIGNAL_SLOT_OBSERVERS_H__
#define __DALI_SIGNAL_SLOT_OBSERVERS_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

class SlotObserver;
class CallbackBase;

/**
 *
 * Abstract interface used by a signal to inform a slot it is disconnecting.
 * This will happen if the object owning the signal is destroyed.
 *
 */
class SignalObserver
{
public:

  /**
   * Constructor
   */
  SignalObserver();

  /**
   * Virtual destructor
   */
  virtual ~SignalObserver();

  /**
   * This method is called when the signal is disconnecting.
   * @param[in] slotObserver The signal that has disconnected.
   * @param[in] callback The callback attached to the signal disconnected.
   */
  virtual void SignalDisconnected( SlotObserver* slotObserver, CallbackBase* callback ) = 0;
};

/**
 * Interface used by a slot to inform a signal it is disconnecting.
 * This is used by the slot if wants to disconnect or is deleted.
 */
class SlotObserver
{
public:

  /**
   * Constructor
   */
  SlotObserver();

  /**
   * Virtual destructor
   */
  virtual ~SlotObserver();

  /**
   * This method is called when the slot is disconnecting.
   * @param[in] callback The callback attached to the signal disconnected.
   */
  virtual void SlotDisconnected( CallbackBase* callback ) = 0;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_SIGNAL_SLOT_OBSERVERS_H__

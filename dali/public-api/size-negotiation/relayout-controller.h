#ifndef __DALI_RELAYOUT_CONTROLLER_H__
#define __DALI_RELAYOUT_CONTROLLER_H__

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/actors/actor.h>

namespace Dali
{

namespace Internal
{
class RelayoutController;
}

/**
 * RelayoutController
 * This singleton class provides functionality to trigger relayouting of actors in the dali scene graph.
 */
class DALI_IMPORT_API RelayoutController : public Dali::BaseHandle
{
public:

  /**
   * Constructor.
   * We should only create a unique instance.
   */
  RelayoutController();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RelayoutController();

  /**
   * @brief Get the singleton of RelayoutController object.
   *
   * @return A handle to the RelayoutController control.
   */
  static RelayoutController Get();

  /**
   * @brief Request the actor to have a pre-relayout callback made on it.
   *
   * This will invoke the OnPreRelayout method on actor at the end of the frame, before the
   * size negotiation algorithm is run
   *
   * @param[in] actor The actor to request pre-relayout on
   */
  static void RequestPreRelayout( Dali::Actor actor );

  /**
   * @brief Request to relayout the given actor and all sub-actors of it.
   *
   * This flags the actor and all actors dependent on it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   *
   * @param[in] actor The actor to request relayout on
   * @param[in] dimension The dimension(s) to request the relayout on. Defaults to all dimensions
   */
  static void RequestRelayout( Dali::Actor actor, Dimension dimension = ALL_DIMENSIONS );

  /**
   * @brief Request to relayout of all actors in the sub-tree below the given actor.
   *
   * This flags the actor and all actors below it for relayout. The actual
   * relayout is performed at the end of the frame. This means that multiple calls to relayout
   * will not cause multiple relayouts to occur.
   *
   * @param[in] actor The actor to request relayout on
   */
  void RequestRelayoutTree( Dali::Actor& actor );

public:  // Intended for internal use only

  RelayoutController( Internal::RelayoutController* impl );

};

} // namespace Dali

#endif // __DALI_RELAYOUT_CONTROLLER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// FILE HEADER

#include "relayout-controller.h"

// INTERNAL INCLUDES

#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

RelayoutController::RelayoutController()
{

}

RelayoutController::~RelayoutController()
{

}

// Static function
RelayoutController RelayoutController::Get()
{
  return RelayoutController( Internal::RelayoutController::Get() );
}

// Static function
void RelayoutController::RequestPreRelayout( Dali::Actor actor )
{
  Internal::RelayoutController* relayoutController = Internal::RelayoutController::Get();
  if( relayoutController )
  {
    relayoutController->RequestPreRelayout( actor );
  }
}

// Static function
void RelayoutController::RequestRelayout( Dali::Actor actor, Dimension dimension )
{
  Internal::RelayoutController* relayoutController = Internal::RelayoutController::Get();
  if( relayoutController )
  {
    relayoutController->RequestRelayout( actor, dimension );
  }
}

void RelayoutController::RequestRelayoutTree( Dali::Actor& actor )
{
  GetImpl(*this).RequestRelayoutTree( actor );
}

RelayoutController::RelayoutController(Internal::RelayoutController* impl)
  : BaseHandle(impl)
{
}

} // namespace Dali


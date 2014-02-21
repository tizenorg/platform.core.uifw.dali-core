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

// CLASS HEADER
#include <dali/public-api/events/touch-point.h>

namespace Dali
{

TouchPoint::TouchPoint(int id, State state, float screenX, float screenY)
: deviceId(id),
  state(state),
  local(screenX, screenY),
  screen(screenX, screenY)
{
}

TouchPoint::TouchPoint(int id, State state, float screenX, float screenY, float localX, float localY)
: deviceId(id),
  state(state),
  local(localX, localY),
  screen(screenX, screenY)
{
}

TouchPoint::~TouchPoint()
{
}

} // namespace Dali

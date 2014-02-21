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
#include <dali/internal/event/events/key-event-processor.h>

// INTERNAL INCLUDES
#include <dali/public-api/events/key-event.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/integration-api/events/key-event-integ.h>

namespace Dali
{

namespace Internal
{

KeyEventProcessor::KeyEventProcessor(Stage& stage)
: mStage(stage)
{
}

KeyEventProcessor::~KeyEventProcessor()
{
}

void KeyEventProcessor::ProcessKeyEvent(const Integration::KeyEvent& event)
{
  KeyEvent keyEvent(event.keyName, event.keyString, event.keyCode, event.keyModifier, event.cursorOffset, event.numberOfChars, static_cast<KeyEvent::State>(event.state));

  // Emit the key event signal from stage.
  mStage.EmitKeyEventSignal(keyEvent);
}

} // namespace Internal

} // namespace Dali

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

// CLASS HEADER
#include <dali/integration-api/events/key-event-integ.h>

namespace Dali
{

namespace Integration
{

KeyEvent::KeyEvent()
: Event(Key),
  keyName(),
  keyString(),
  keyCode(-1),
  keyModifier(0),
  time(0),
  state(KeyEvent::Down)
{
}

KeyEvent::KeyEvent(const std::string& keyName, const std::string& keyString, int keyCode, int keyModifier, unsigned long timeStamp, const State& keyState)
: Event(Key),
  keyName(keyName),
  keyString(keyString),
  keyCode(keyCode),
  keyModifier(keyModifier),
  time(timeStamp),
  state(keyState)
{
}

KeyEvent::~KeyEvent()
{
}

} // namespace Integration

} // namespace Dali

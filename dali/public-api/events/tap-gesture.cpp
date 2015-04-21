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
#include <dali/public-api/events/tap-gesture.h>

namespace Dali
{

TapGesture::TapGesture()
: Gesture(Gesture::Tap, Gesture::Clear),
  numberOfTaps(1),
  numberOfTouches(1)
{
}

TapGesture::TapGesture( const TapGesture& rhs )
: Gesture( rhs ),
  numberOfTaps( rhs.numberOfTaps ),
  numberOfTouches( rhs.numberOfTouches ),
  screenPoint( rhs.screenPoint ),
  localPoint( rhs.localPoint )
{
}

TapGesture& TapGesture::operator=( const TapGesture& rhs )
{
  Gesture::operator=(rhs);
  numberOfTaps = rhs.numberOfTaps;
  numberOfTouches = rhs.numberOfTouches;
  screenPoint = rhs.screenPoint;
  localPoint = rhs.localPoint;

  return *this;
}

TapGesture::~TapGesture()
{
}

} // namespace Dali

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
#include <dali/public-api/common/constants.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/degree.h>

namespace Dali
{

const float ParentOrigin::TOP     = 0.0f;
const float ParentOrigin::BOTTOM  = 1.0f;
const float ParentOrigin::LEFT    = 0.0f;
const float ParentOrigin::RIGHT   = 1.0f;
const float ParentOrigin::MIDDLE  = 0.5f;

const Vector3 ParentOrigin::BACK_TOP_LEFT     (0.0f, 0.0f, 0.0f);
const Vector3 ParentOrigin::BACK_TOP_CENTER   (0.5f, 0.0f, 0.0f);
const Vector3 ParentOrigin::BACK_TOP_RIGHT    (1.0f, 0.0f, 0.0f);
const Vector3 ParentOrigin::BACK_CENTER_LEFT  (0.0f, 0.5f, 0.0f);
const Vector3 ParentOrigin::BACK_CENTER       (0.5f, 0.5f, 0.0f);
const Vector3 ParentOrigin::BACK_CENTER_RIGHT (1.0f, 0.5f, 0.0f);
const Vector3 ParentOrigin::BACK_BOTTOM_LEFT  (0.0f, 1.0f, 0.0f);
const Vector3 ParentOrigin::BACK_BOTTOM_CENTER(0.5f, 1.0f, 0.0f);
const Vector3 ParentOrigin::BACK_BOTTOM_RIGHT (1.0f, 1.0f, 0.0f);

const Vector3 ParentOrigin::TOP_LEFT     (0.0f, 0.0f, 0.5f);
const Vector3 ParentOrigin::TOP_CENTER   (0.5f, 0.0f, 0.5f);
const Vector3 ParentOrigin::TOP_RIGHT    (1.0f, 0.0f, 0.5f);
const Vector3 ParentOrigin::CENTER_LEFT  (0.0f, 0.5f, 0.5f);
const Vector3 ParentOrigin::CENTER       (0.5f, 0.5f, 0.5f);
const Vector3 ParentOrigin::CENTER_RIGHT (1.0f, 0.5f, 0.5f);
const Vector3 ParentOrigin::BOTTOM_LEFT  (0.0f, 1.0f, 0.5f);
const Vector3 ParentOrigin::BOTTOM_CENTER(0.5f, 1.0f, 0.5f);
const Vector3 ParentOrigin::BOTTOM_RIGHT (1.0f, 1.0f, 0.5f);

const Vector3 ParentOrigin::FRONT_TOP_LEFT     (0.0f, 0.0f, 1.0f);
const Vector3 ParentOrigin::FRONT_TOP_CENTER   (0.5f, 0.0f, 1.0f);
const Vector3 ParentOrigin::FRONT_TOP_RIGHT    (1.0f, 0.0f, 1.0f);
const Vector3 ParentOrigin::FRONT_CENTER_LEFT  (0.0f, 0.5f, 1.0f);
const Vector3 ParentOrigin::FRONT_CENTER       (0.5f, 0.5f, 1.0f);
const Vector3 ParentOrigin::FRONT_CENTER_RIGHT (1.0f, 0.5f, 1.0f);
const Vector3 ParentOrigin::FRONT_BOTTOM_LEFT  (0.0f, 1.0f, 1.0f);
const Vector3 ParentOrigin::FRONT_BOTTOM_CENTER(0.5f, 1.0f, 1.0f);
const Vector3 ParentOrigin::FRONT_BOTTOM_RIGHT (1.0f, 1.0f, 1.0f);

const Vector3 ParentOrigin::DEFAULT( ParentOrigin::TOP_LEFT );

const float AnchorPoint::TOP     = 0.0f;
const float AnchorPoint::BOTTOM  = 1.0f;
const float AnchorPoint::LEFT    = 0.0f;
const float AnchorPoint::RIGHT   = 1.0f;
const float AnchorPoint::MIDDLE  = 0.5f;

const Vector3 AnchorPoint::BACK_TOP_LEFT     (0.0f, 0.0f, 0.0f);
const Vector3 AnchorPoint::BACK_TOP_CENTER   (0.5f, 0.0f, 0.0f);
const Vector3 AnchorPoint::BACK_TOP_RIGHT    (1.0f, 0.0f, 0.0f);
const Vector3 AnchorPoint::BACK_CENTER_LEFT  (0.0f, 0.5f, 0.0f);
const Vector3 AnchorPoint::BACK_CENTER       (0.5f, 0.5f, 0.0f);
const Vector3 AnchorPoint::BACK_CENTER_RIGHT (1.0f, 0.5f, 0.0f);
const Vector3 AnchorPoint::BACK_BOTTOM_LEFT  (0.0f, 1.0f, 0.0f);
const Vector3 AnchorPoint::BACK_BOTTOM_CENTER(0.5f, 1.0f, 0.0f);
const Vector3 AnchorPoint::BACK_BOTTOM_RIGHT (1.0f, 1.0f, 0.0f);

const Vector3 AnchorPoint::TOP_LEFT     (0.0f, 0.0f, 0.5f);
const Vector3 AnchorPoint::TOP_CENTER   (0.5f, 0.0f, 0.5f);
const Vector3 AnchorPoint::TOP_RIGHT    (1.0f, 0.0f, 0.5f);
const Vector3 AnchorPoint::CENTER_LEFT  (0.0f, 0.5f, 0.5f);
const Vector3 AnchorPoint::CENTER       (0.5f, 0.5f, 0.5f);
const Vector3 AnchorPoint::CENTER_RIGHT (1.0f, 0.5f, 0.5f);
const Vector3 AnchorPoint::BOTTOM_LEFT  (0.0f, 1.0f, 0.5f);
const Vector3 AnchorPoint::BOTTOM_CENTER(0.5f, 1.0f, 0.5f);
const Vector3 AnchorPoint::BOTTOM_RIGHT (1.0f, 1.0f, 0.5f);

const Vector3 AnchorPoint::FRONT_TOP_LEFT     (0.0f, 0.0f, 1.0f);
const Vector3 AnchorPoint::FRONT_TOP_CENTER   (0.5f, 0.0f, 1.0f);
const Vector3 AnchorPoint::FRONT_TOP_RIGHT    (1.0f, 0.0f, 1.0f);
const Vector3 AnchorPoint::FRONT_CENTER_LEFT  (0.0f, 0.5f, 1.0f);
const Vector3 AnchorPoint::FRONT_CENTER       (0.5f, 0.5f, 1.0f);
const Vector3 AnchorPoint::FRONT_CENTER_RIGHT (1.0f, 0.5f, 1.0f);
const Vector3 AnchorPoint::FRONT_BOTTOM_LEFT  (0.0f, 1.0f, 1.0f);
const Vector3 AnchorPoint::FRONT_BOTTOM_CENTER(0.5f, 1.0f, 1.0f);
const Vector3 AnchorPoint::FRONT_BOTTOM_RIGHT (1.0f, 1.0f, 1.0f);

const Vector3 AnchorPoint::DEFAULT( AnchorPoint::CENTER );

const Vector4 Color::BLACK( 0.0f, 0.0f, 0.0f, 1.0f );
const Vector4 Color::WHITE( 1.0f, 1.0f, 1.0f, 1.0f );

const Vector4 Color::RED(   1.0f, 0.0f, 0.0f, 1.0f );
const Vector4 Color::GREEN( 0.0f, 1.0f, 0.0f, 1.0f );
const Vector4 Color::BLUE(  0.0f, 0.0f, 1.0f, 1.0f );

const Vector4 Color::YELLOW(  1.0f, 1.0f, 0.0f, 1.0f );
const Vector4 Color::MAGENTA( 1.0f, 0.0f, 1.0f, 1.0f );
const Vector4 Color::CYAN(    0.0f, 1.0f, 1.0f, 1.0f );

const Vector4 Color::TRANSPARENT( 0.0f, 0.0f, 0.0f, 0.0f );

// epsilon constants
const float Math::MACHINE_EPSILON_0     = Epsilon<0>::value;
const float Math::MACHINE_EPSILON_1     = Epsilon<1>::value;
const float Math::MACHINE_EPSILON_10    = Epsilon<10>::value;
const float Math::MACHINE_EPSILON_100   = Epsilon<100>::value;
const float Math::MACHINE_EPSILON_1000  = Epsilon<1000>::value;
const float Math::MACHINE_EPSILON_10000 = Epsilon<10000>::value;

const std::string String::EMPTY("");

} // namespace Dali

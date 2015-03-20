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

// INTERNAL INCLUDES
#include <dali/internal/update/animation/interpolator-functions.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>


namespace
{

bool LerpBoolean( const bool& current, const bool& target, float progress )
{
  // Not particularly useful for boolean values
  if (progress >= 1.0f)
  {
    return target;
  }

  return current;
}

float LerpFloat( const float& current, const float& target, float progress )
{
  return current + ((target - current) * progress);
}

int LerpInteger( const int& current, const int& target, float progress )
{
  return static_cast<int>( current + ( (target - current) * progress ) + 0.5f );
}

Dali::Vector2 LerpVector2( const Dali::Vector2& current, const Dali::Vector2& target, float progress )
{
  return current + ((target - current) * progress);
}

Dali::Vector3 LerpVector3( const Dali::Vector3& current, const Dali::Vector3& target, float progress )
{
  return current + ((target - current) * progress);
}

Dali::Vector4 LerpVector4( const Dali::Vector4& current, const Dali::Vector4& target, float progress )
{
  return current + ((target - current) * progress);
}

Dali::Quaternion SlerpQuaternion( const Dali::Quaternion& current, const Dali::Quaternion& target, float progress )
{
  return Dali::Quaternion::Slerp(current, target, progress);
}

Dali::Matrix LerpMatrix( const Dali::Matrix& current, const Dali::Matrix& target, float progress )
{
  if (progress > 0.5f)
  {
    return target;
  }

  return current;
}

Dali::Matrix3 LerpMatrix3( const Dali::Matrix3& current, const Dali::Matrix3& target, float progress )
{
  if (progress > 0.5f)
  {
    return target;
  }

  return current;
}

} // unnamed namespace

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

CallbackBase* GetDefaultInterpolator( Property::Type type )
{
  CallbackBase* function = NULL;

  switch ( type )
  {
    case Property::BOOLEAN:
    {
      function = MakeCallback( LerpBoolean );
      break;
    }

    case Property::FLOAT:
    {
      function = MakeCallback( LerpFloat );
      break;
    }

    case Property::INTEGER:
    {
      function = MakeCallback( LerpInteger );
      break;
    }

    case Property::VECTOR2:
    {
      function = MakeCallback( LerpVector2 );
      break;
    }

    case Property::VECTOR3:
    {
      function = MakeCallback( LerpVector3 );
      break;
    }

    case Property::VECTOR4:
    {
      function = MakeCallback( LerpVector4 );
      break;
    }

    case Property::ORIENTATION:
    {
      function = MakeCallback( SlerpQuaternion );
      break;
    }

    case Property::MATRIX3:
    {
      function = MakeCallback( LerpMatrix3 );
      break;
    }

    case Property::MATRIX:
    {
      function = MakeCallback( LerpMatrix );
      break;
    }

    default:
      break;
  }

  return function;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

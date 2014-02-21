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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/animation/interpolator-functions.h>

namespace Dali
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

Vector2 LerpVector2( const Vector2& current, const Vector2& target, float progress )
{
  return current + ((target - current) * progress);
}

Vector3 LerpVector3( const Vector3& current, const Vector3& target, float progress )
{
  return current + ((target - current) * progress);
}

Vector4 LerpVector4( const Vector4& current, const Vector4& target, float progress )
{
  return current + ((target - current) * progress);
}

Quaternion SlerpQuaternion( const Quaternion& current, const Quaternion& target, float progress )
{
  return Quaternion::Slerp(current, target, progress);
}

Matrix LerpMatrix( const Matrix& current, const Matrix& target, float progress )
{
  if (progress > 0.5f)
  {
    return target;
  }

  return current;
}

Matrix3 LerpMatrix3( const Matrix3& current, const Matrix3& target, float progress )
{
  if (progress > 0.5f)
  {
    return target;
  }

  return current;
}

AnyInterpolator GetDefaultInterpolator( Property::Type type )
{
  AnyInterpolator function;

  switch ( type )
  {
    case Property::BOOLEAN:
    {
      function = boost::function<bool (const bool&, const bool&, float)>( &LerpBoolean );
      break;
    }

    case Property::FLOAT:
    {
      function = boost::function<float (const float&, const float&, float)>( &LerpFloat );
      break;
    }

    case Property::VECTOR2:
    {
      function = boost::function<Vector2 (const Vector2&, const Vector2&, float)>( &LerpVector2 );
      break;
    }

    case Property::VECTOR3:
    {
      function = boost::function<Vector3 (const Vector3&, const Vector3&, float)>( &LerpVector3 );
      break;
    }

    case Property::VECTOR4:
    {
      function = boost::function<Vector4 (const Vector4&, const Vector4&, float)>( &LerpVector4 );
      break;
    }

    case Property::ROTATION:
    {
      function = boost::function<Quaternion (const Quaternion&, const Quaternion&, float)>( &SlerpQuaternion );
      break;
    }

    case Property::MATRIX3:
    {
      function = boost::function<Matrix3 (const Matrix3&, const Matrix3&, float)>(&LerpMatrix3);
      break;
    }

    case Property::MATRIX:
    {
      function = boost::function<Matrix (const Matrix&, const Matrix&, float)>(&LerpMatrix);
      break;
    }

    default:
      break;
  }

  return function;
}


} // namespace Dali

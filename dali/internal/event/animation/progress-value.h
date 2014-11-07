#ifndef __DALI_INTERNAL_PROGRESS_VALUE_H__
#define __DALI_INTERNAL_PROGRESS_VALUE_H__

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
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/degree.h>

namespace Dali
{

namespace Internal
{

/**
 * Progress / value pair for animating channels (properties) with keyframes
 */
template <typename T>
class ProgressValue
{
public:
  ProgressValue (float progress, T value)
  : mProgress(progress),
    mValue (value)
  {
  }

  ~ProgressValue ()
  {
  }

  float GetProgress () const
  {
    return mProgress;
  }

  const T& GetValue () const
  {
    return mValue;
  }

public:
  float mProgress;   ///< Progress this value applies to animation channel
  T mValue;          ///< value this animation channel should take
};

typedef ProgressValue<Quaternion>                       ProgressQuaternion;
typedef std::vector<ProgressQuaternion>                 ProgressQuaternionContainer;

typedef ProgressValue<AngleAxis>                        ProgressAngleAxis;
typedef std::vector<AngleAxis>                          ProgressAngleAxisContainer;

typedef ProgressValue<bool>                             ProgressBoolean;
typedef std::vector<ProgressBoolean>                    ProgressBooleanContainer;

typedef ProgressValue<float>                            ProgressNumber;
typedef std::vector<ProgressNumber>                     ProgressNumberContainer;

typedef ProgressValue<int>                              ProgressInteger;
typedef std::vector<ProgressInteger>                    ProgressIntegerContainer;

typedef ProgressValue<Vector2>                          ProgressVector2;
typedef std::vector<ProgressVector2>                    ProgressVector2Container;

typedef ProgressValue<Vector3>                          ProgressVector3;
typedef std::vector<ProgressVector3>                    ProgressVector3Container;

typedef ProgressValue<Vector4>                          ProgressVector4;
typedef std::vector<ProgressVector4>                    ProgressVector4Container;


template <typename T>
inline T Interpolate( const T& a, const T&b, float progress )
{
  return (a + (b - a) * progress);
}

//Specialization for boolean interpolation
template <>
inline bool Interpolate (const bool& a, const bool& b, float progress)
{
  return progress < 0.5f ? a : b;
}

//Specialization for integer interpolation
template <>
inline int Interpolate(const int& a, const int& b, float progress)
{
  return static_cast<int>(a + (b - a) * progress + 0.5f);
}

//Specialization for quaternion
template <>
inline Quaternion Interpolate (const Quaternion& a, const Quaternion& b, float progress)
{
  return Quaternion::Slerp(a, b, progress);
}

//Specialization for AngleAxis
template <>
inline AngleAxis Interpolate ( const AngleAxis& a, const AngleAxis& b, float progress)
{
  Quaternion q1(Radian(a.angle), a.axis);
  Quaternion q2(Radian(b.angle), b.axis);

  Quaternion iq = Quaternion::Slerp(q1, q2, progress);
  AngleAxis result;
  iq.ToAxisAngle(result.axis, result.angle);
  return result;
}

/* Cubic Interpolation between values p1 and p2. p0 and p3 are prev and next values
 * and are used as control points to calculate tangent of the curve at interpolation points.
 *
 * f(t) = a3*t^3 + a2*t^2 + a1*t + a0
 * Constraints: f(0)=p1   f(1)=p2   f'(0)=p2-p0   f'(1)=p3-p1
 */
template <typename T>
inline T CubicInterpolate( const T& p0, const T& p1, const T&  p2, const T&  p3, float progress )
{
  T a3 = -p0 + p1 - p2 + p3;
  return static_cast<T>( a3*progress*progress*progress + (p0-p1-a3)*progress*progress + (p2-p0)*progress + p1 );
}

//Specialization for bool.
template <>
inline bool CubicInterpolate( const bool& p0, const bool& p1, const bool&  p2, const bool&  p3, float progress )
{
  return Interpolate( p1, p2, progress );
}

//Specialization for Quaternion.
template <>
inline Quaternion CubicInterpolate( const Quaternion& p0, const Quaternion& p1, const Quaternion&  p2, const Quaternion&  p3, float progress )
{
  return Interpolate( p1, p2, progress );
}

//Specialization for AngleAxis.
template <>
inline AngleAxis CubicInterpolate( const AngleAxis& p0, const AngleAxis& p1, const AngleAxis&  p2, const AngleAxis&  p3, float progress )
{
  return Interpolate( p1, p2, progress );
}

} // namespace Internal

} // namespace Dali

#endif //__DALI_PROGRESS_VALUE_H__
